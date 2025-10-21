#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
SMART CONTENT FORMATTER v2.0
Intelligent content processing pipeline for The Book

FEATURES:
- Scans the_book/ folder recursively
- Formats TXT files in-place based on configuration
- Tracks formatting state with embedded markers
- Auto-detects when config changes and reformats all files
- Fully configurable via formatting_config.json
- Idempotent: safe to run multiple times

USAGE:
1. Edit formatting_config.json to customize formatting
2. Run: python3 smart_formatter.py
3. All files in the_book/ will be formatted according to config
4. Run again anytime - already formatted files are skipped
5. Change config and run again - all files will be reformatted
"""

import os
import re
import json
import hashlib
from pathlib import Path
from datetime import datetime

# Configuration
CONFIG_FILE = Path("formatting_config.json")
BASE_DIR = Path("the_book")
FORMAT_MARKER = "<!-- FORMATTED:"  # Marker to identify formatted files

def load_config():
    """Load formatting configuration"""
    if not CONFIG_FILE.exists():
        print(f"ERROR: Config file not found: {CONFIG_FILE}")
        print("Please create formatting_config.json first")
        exit(1)

    with open(CONFIG_FILE, 'r') as f:
        return json.load(f)

def get_config_hash(config):
    """Generate hash of config to detect changes"""
    # Create a stable string representation of config
    config_str = json.dumps(config, sort_keys=True)
    return hashlib.md5(config_str.encode()).hexdigest()[:8]

def is_formatted(text, config_hash):
    """Check if file is already formatted with current config"""
    marker = f"{FORMAT_MARKER} {config_hash} -->"
    return marker in text

def add_format_marker(text, config_hash):
    """Add invisible marker to track formatting state"""
    marker = f"{FORMAT_MARKER} {config_hash} -->"

    # Remove any old markers first
    text = re.sub(r'<!-- FORMATTED:.*?-->', '', text)

    # Add new marker at the end
    return text.rstrip() + f"\n\n{marker}\n"

def wrap_text(text, width):
    """Wrap text to specified display width"""
    lines = []
    for paragraph in text.split('\n'):
        if not paragraph.strip():
            lines.append('')
            continue

        # Check if line is already a special format (markers, etc)
        if paragraph.startswith('<!--'):
            lines.append(paragraph)
            continue

        words = paragraph.split()
        current_line = ""
        for word in words:
            test_line = current_line + " " + word if current_line else word
            if len(test_line) <= width:
                current_line = test_line
            else:
                if current_line:
                    lines.append(current_line)
                current_line = word
        if current_line:
            lines.append(current_line)
    return '\n'.join(lines)

def apply_color(text, color_code):
    """Apply ANSI color code if enabled"""
    if not color_code:
        return text
    return f"\033[{color_code}m{text}\033[0m"

def create_divider(char, length):
    """Create a divider line"""
    return char * length

def detect_category_from_path(filepath):
    """Extract category from file path"""
    parts = filepath.relative_to(BASE_DIR).parts[:-1]  # Exclude filename
    return '/'.join(parts) if parts else 'general'

def create_toc(text, title, config):
    """Create table of contents for long documents"""
    if not config['features']['add_table_of_contents']:
        return None

    lines = text.split('\n')
    toc_entries = []

    header_cfg = config['header_detection']

    for i, line in enumerate(lines):
        line_stripped = line.strip()

        # Skip empty lines and markers
        if not line_stripped or line_stripped.startswith('<!--'):
            continue

        # Detect headers based on config
        if header_cfg['all_caps_as_header']:
            if (line_stripped.isupper() and
                header_cfg['min_header_length'] < len(line_stripped) < header_cfg['max_header_length']):
                toc_entries.append(line_stripped)

        # Check chapter patterns
        for pattern in header_cfg['chapter_patterns']:
            if re.match(pattern, line_stripped, re.IGNORECASE):
                toc_entries.append(line_stripped)
                break

    if not toc_entries or len(toc_entries) < 3:
        return None

    # Build TOC
    sep = create_divider(
        config['separators']['section_divider'],
        config['separators']['divider_length']
    )

    toc = "TABLE OF CONTENTS\n"
    toc += f"{title}\n\n"
    for entry in toc_entries[:20]:  # Limit to first 20 entries
        toc += f"  • {entry}\n"
    toc += f"\n{sep}\n\n"

    return toc

def format_title(title, config):
    """Format title based on config"""
    style = config['title_style']

    if style['make_uppercase']:
        title = title.upper()

    if style['make_bold']:
        title = f"**{title}**"

    result = title

    if style['underline_char']:
        underline = style['underline_char'] * len(title.replace('*', ''))
        result = f"{title}\n{underline}"

    return result

def add_bold_titles(text, config):
    """Add markdown-style bold to titles based on config"""
    if not config['special_formatting']['bold_all_caps_lines']:
        return text

    lines = text.split('\n')
    formatted = []
    header_cfg = config['header_detection']

    for line in lines:
        line_stripped = line.strip()

        # Check if this is a header
        if (line_stripped.isupper() and
            header_cfg['min_header_length'] < len(line_stripped) < header_cfg['max_header_length'] and
            not line_stripped.startswith('**')):
            formatted.append(f"**{line_stripped}**")
        else:
            formatted.append(line)

    return '\n'.join(formatted)

def find_related_articles(category, title, config):
    """Find related articles in the same category"""
    if not config['features']['add_related_articles']:
        return []

    related = []
    max_related = config['thresholds']['max_related_articles']

    category_path = BASE_DIR / category
    if category_path.exists():
        for file in category_path.glob("*.txt"):
            if file.stem != title and len(related) < max_related:
                related.append(file.stem.replace('_', ' ').replace('-', ' ').title())

    return related

def format_content(text, filepath, config):
    """Apply all formatting based on config"""
    length = len(text)
    title = filepath.stem.replace('_', ' ').replace('-', ' ').title()
    category = detect_category_from_path(filepath)

    formatted = ""
    sep = create_divider(
        config['separators']['section_divider'],
        config['separators']['divider_length']
    )
    subsep = create_divider(
        config['separators']['subsection_divider'],
        config['separators']['divider_length']
    )

    # Title
    formatted_title = format_title(title, config)
    formatted += f"{formatted_title}\n\n"

    # Category breadcrumb
    if config['features']['add_category_breadcrumb']:
        breadcrumb = category.replace('/', ' › ').title()
        formatted += f"Category: {breadcrumb}\n{subsep}\n\n"

    # Table of contents for long documents
    if length > config['thresholds']['long_document_toc']:
        toc = create_toc(text, title, config)
        if toc:
            formatted += toc

    # Main content with bold titles
    formatted += add_bold_titles(text, config)

    # Related articles
    related = find_related_articles(category, filepath.stem, config)
    if related:
        formatted += f"\n\n{sep}\n"
        formatted += "**RELATED ARTICLES:**\n\n"
        for article in related:
            formatted += f"  • {article}\n"

    # Metadata footer
    if config['features']['add_metadata_footer']:
        formatted += f"\n\n{sep}\n"
        formatted += f"Processed: {datetime.now().strftime('%Y-%m-%d %H:%M')}\n"
        formatted += f"Words: ~{len(text.split()):,}\n"
        formatted += f"Category: {breadcrumb if config['features']['add_category_breadcrumb'] else category}\n"

    return formatted

def process_file(filepath, config, config_hash, stats):
    """Process a single file"""
    try:
        # Read file
        with open(filepath, 'r', encoding='utf-8') as f:
            original_text = f.read()

        # Check if already formatted with current config
        if is_formatted(original_text, config_hash):
            stats['skipped'] += 1
            return True

        # Remove old format marker if exists
        text = re.sub(r'<!-- FORMATTED:.*?-->\n*', '', original_text).strip()

        # Check if file is empty
        if not text:
            print(f"  ⚠️  Skipping empty file: {filepath.name}")
            stats['skipped'] += 1
            return True

        print(f"  Formatting: {filepath.relative_to(BASE_DIR)}")

        # Format content
        formatted = format_content(text, filepath, config)

        # Wrap for display
        wrapped = wrap_text(formatted, config['display']['line_width'])

        # Add format marker
        final_content = add_format_marker(wrapped, config_hash)

        # Write back to file
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write(final_content)

        stats['formatted'] += 1
        return True

    except Exception as e:
        print(f"  ❌ ERROR processing {filepath.name}: {e}")
        stats['failed'] += 1
        return False

def scan_and_format(config, config_hash):
    """Scan all files and format as needed"""
    print(f"\nScanning: {BASE_DIR}/")
    print(f"Config hash: {config_hash}")
    print()

    # Find all text files
    all_files = list(BASE_DIR.rglob("*.txt"))

    # Filter out index and search files
    text_files = [
        f for f in all_files
        if not f.name.startswith('_') and not f.name.endswith('_index.txt')
    ]

    if not text_files:
        print("No files found to process")
        return

    print(f"Found {len(text_files)} files\n")

    stats = {
        'formatted': 0,
        'skipped': 0,
        'failed': 0
    }

    # Process each file
    for filepath in text_files:
        process_file(filepath, config, config_hash, stats)

    return stats

def main():
    """Main processing function"""
    print("=" * 70)
    print("SMART CONTENT FORMATTER v2.0")
    print("=" * 70)
    print()

    # Load config
    print("Loading configuration...")
    config = load_config()
    config_hash = get_config_hash(config)

    print(f"✓ Config loaded: v{config['version']}")
    print(f"✓ Line width: {config['display']['line_width']} chars")
    print(f"✓ Features: TOC={config['features']['add_table_of_contents']}, "
          f"Related={config['features']['add_related_articles']}, "
          f"Metadata={config['features']['add_metadata_footer']}")

    # Check if the_book exists
    if not BASE_DIR.exists():
        print(f"\nERROR: {BASE_DIR}/ directory not found")
        return

    # Scan and format
    stats = scan_and_format(config, config_hash)

    if not stats:
        return

    # Summary
    print()
    print("=" * 70)
    print("FORMATTING COMPLETE")
    print("=" * 70)
    print()
    print(f"  ✓ Formatted: {stats['formatted']}")
    print(f"  ⊘ Skipped (already formatted): {stats['skipped']}")
    print(f"  ✗ Failed: {stats['failed']}")
    print()

    if stats['formatted'] > 0:
        print("📝 All files formatted according to current config")
        print()

    if stats['skipped'] > 0:
        print("💡 TIP: Edit formatting_config.json and re-run to reformat all files")
        print()

    print("✨ The Book is ready for M5Cardputer!")
    print("=" * 70)

if __name__ == "__main__":
    main()
