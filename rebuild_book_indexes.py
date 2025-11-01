#!/usr/bin/env python3
"""
Rebuild all Book indexes to ensure no entries are missing
Creates searchable index.txt files for each category
"""

import os
import re
from pathlib import Path

# Base path
BOOK_PATH = Path("/Users/jamesfauntleroy/Documents/PlatformIO/Projects/LaboratoryM5/the_book")

# Categories to index
CATEGORIES = [
    "agriculture",
    "diy_technical",
    "edible_plants",
    "history",
    "literature",
    "medical",
    "poisonous_plants",
    "programming",
    "programming_languages",
    "reference",
    "religious",
    "science",
    "survival",
    "tech",
    "wikipedia"
]

def extract_title_from_file(filepath):
    """Extract title from filename or first line of file"""
    # Try to read first line
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            first_lines = [f.readline().strip() for _ in range(5)]
            # Look for markdown header or title
            for line in first_lines:
                if line.startswith('#') or line.startswith('**'):
                    title = line.strip('#* ').strip()
                    if title and len(title) < 100:
                        return title
    except:
        pass

    # Fallback to filename
    return filepath.stem.replace('_', ' ').title()

def extract_preview(filepath):
    """Extract preview text from file (first meaningful line)"""
    try:
        with open(filepath, 'r', encoding='utf-8', errors='ignore') as f:
            lines = f.readlines()[:20]
            for line in lines:
                clean = line.strip()
                # Skip headers, separators, empty lines
                if clean and len(clean) > 20 and not clean.startswith('#') and not clean.startswith('=') and not clean.startswith('-'):
                    # Truncate to reasonable length
                    preview = clean[:80]
                    return preview
    except:
        pass
    return "Article content"

def scan_directory(category_path):
    """Scan directory recursively for .txt files"""
    entries = []

    for item in category_path.rglob('*.txt'):
        # Skip index files
        if item.name == 'index.txt':
            continue

        # Get relative path from category
        rel_path = item.relative_to(category_path)

        # Extract title and preview
        title = extract_title_from_file(item)
        preview = extract_preview(item)

        # Create index entry: TITLE|FILENAME|LINE|PREVIEW
        entry = f"{title}|{rel_path}|0|{preview}"
        entries.append(entry)

    return sorted(entries)

def rebuild_index(category):
    """Rebuild index for a category"""
    category_path = BOOK_PATH / category

    if not category_path.exists():
        print(f"⚠️  Skipping {category} - directory not found")
        return

    print(f"📚 Indexing {category}...")

    # Scan for all text files
    entries = scan_directory(category_path)

    if not entries:
        print(f"   No entries found in {category}")
        return

    # Write index file
    index_path = category_path / "index.txt"
    with open(index_path, 'w', encoding='utf-8') as f:
        f.write(f"**INDEX**\n\n")
        f.write(f"Category: {category.replace('_', ' ').title()}\n")
        f.write("---------------------------------------\n\n")

        for entry in entries:
            f.write(entry + "\n")

        f.write(f"\n=======================================\n")
        f.write(f"Total entries: {len(entries)}\n")

    print(f"   ✅ Created {len(entries)} entries")

def main():
    print("🔨 Rebuilding Book indexes...\n")

    total_entries = 0

    for category in CATEGORIES:
        rebuild_index(category)
        # Count entries
        index_file = BOOK_PATH / category / "index.txt"
        if index_file.exists():
            with open(index_file, 'r') as f:
                content = f.read()
                count = content.count('|')
                total_entries += count

    print(f"\n✅ Done! Total indexed entries: {total_entries}")
    print(f"📁 Indexes saved to {BOOK_PATH}/*/index.txt")

if __name__ == "__main__":
    main()
