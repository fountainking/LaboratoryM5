#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
INDEX BUILDER for The Book
Automatically generates index.txt files for all categories

USAGE:
  python3 tools/build_indexes.py
"""

import os
import re
from pathlib import Path

BASE_DIR = Path("the_book")

def extract_title_from_file(filepath):
    """Extract title from filename or first line of file"""
    # Try filename first
    title = filepath.stem.replace('_', ' ').replace('-', ' ').title()

    # Try to get better title from file content
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            for line in f:
                line = line.strip()
                if not line:
                    continue
                # Look for markdown bold title
                if line.startswith('**') and line.endswith('**'):
                    title = line.replace('**', '').strip()
                    break
                # Look for all-caps title
                if line.isupper() and 3 < len(line) < 100:
                    title = line.title()
                    break
                # Use first non-empty line if no better match
                if len(line) > 3:
                    title = line[:100]  # Limit length
                    break
    except:
        pass

    return title

def extract_preview(filepath, max_chars=100):
    """Extract preview text from file"""
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read(500)  # Read first 500 chars

            # Remove markdown formatting
            content = re.sub(r'\*\*(.+?)\*\*', r'\1', content)
            content = re.sub(r'[=\-]{3,}', '', content)

            # Get first meaningful line
            lines = content.split('\n')
            for line in lines:
                line = line.strip()
                if len(line) > 20:  # Skip very short lines
                    preview = line[:max_chars]
                    if len(line) > max_chars:
                        preview += "..."
                    return preview

            return "No preview available"
    except:
        return "No preview available"

def build_index_for_folder(folder_path):
    """Build index.txt for a specific folder"""
    print(f"Building index for: {folder_path}")

    # Find all .txt files (excluding existing index.txt)
    txt_files = []
    for file in folder_path.rglob("*.txt"):
        if file.name != "index.txt" and not file.name.startswith('_'):
            txt_files.append(file)

    if not txt_files:
        print(f"  ⚠️  No content files found")
        return False

    print(f"  Found {len(txt_files)} files")

    # Build index entries
    entries = []
    for filepath in sorted(txt_files):
        # Get relative path from folder
        rel_path = filepath.relative_to(folder_path)

        title = extract_title_from_file(filepath)
        preview = extract_preview(filepath)

        # Format: TITLE|FILENAME|LINE|PREVIEW
        entry = f"{title}|{rel_path}|0|{preview}"
        entries.append(entry)

    # Write index.txt
    index_path = folder_path / "index.txt"

    # Get category name
    category = folder_path.name.replace('_', ' ').title()

    # Build index content (WITHOUT formatting - that's handled by smart_formatter.py)
    index_content = f"INDEX\n\n"
    index_content += f"Category: {category}\n"
    index_content += "---------------------------------------\n\n"

    for entry in entries:
        index_content += entry + "\n"

    # Write to file
    with open(index_path, 'w', encoding='utf-8') as f:
        f.write(index_content)

    print(f"  ✓ Created index with {len(entries)} entries")
    return True

def main():
    """Build indexes for all folders in the_book/"""
    print("=" * 70)
    print("INDEX BUILDER - The Book")
    print("=" * 70)
    print()

    if not BASE_DIR.exists():
        print(f"ERROR: {BASE_DIR} directory not found")
        return

    # Find all subdirectories
    folders = [f for f in BASE_DIR.iterdir() if f.is_dir() and not f.name.startswith('.')]

    print(f"Found {len(folders)} categories\n")

    built = 0
    for folder in sorted(folders):
        if build_index_for_folder(folder):
            built += 1
        print()

    print("=" * 70)
    print(f"✓ Built {built} indexes")
    print("=" * 70)
    print()
    print("Next step: Run smart_formatter.py to format all content")

if __name__ == "__main__":
    main()
