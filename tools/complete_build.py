#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
COMPLETE BUILD SYSTEM for The Book
One command to build everything!

USAGE:
  python3 tools/complete_build.py

WHAT IT DOES:
  1. Cleans up old files (.DS_Store, old indexes)
  2. Builds fresh index.txt for all categories
  3. Formats all content with smart_formatter.py
  4. Lists all categories found (for firmware update)
"""

import os
import sys
import subprocess
from pathlib import Path

BASE_DIR = Path("the_book")

def print_header(text):
    print()
    print("=" * 70)
    print(text)
    print("=" * 70)
    print()

def cleanup():
    """Remove old/temporary files"""
    print_header("STEP 1: CLEANUP")

    removed = 0

    # Remove .DS_Store files
    for ds_store in BASE_DIR.rglob(".DS_Store"):
        print(f"  Removing: {ds_store}")
        ds_store.unlink()
        removed += 1

    # Remove old search indexes
    for old_index in BASE_DIR.glob("_search_index.*"):
        print(f"  Removing: {old_index}")
        old_index.unlink()
        removed += 1

    # Remove backup files
    for backup in BASE_DIR.rglob("*.backup"):
        print(f"  Removing: {backup}")
        backup.unlink()
        removed += 1

    print(f"\n✓ Removed {removed} old files")

def build_indexes():
    """Run index builder"""
    print_header("STEP 2: BUILD INDEXES")

    result = subprocess.run([sys.executable, "tools/build_indexes.py"])

    if result.returncode != 0:
        print("❌ Index building failed")
        return False

    return True

def format_content():
    """Run formatter"""
    print_header("STEP 3: FORMAT CONTENT")

    result = subprocess.run([sys.executable, "smart_formatter.py"])

    if result.returncode != 0:
        print("❌ Formatting failed")
        return False

    return True

def list_categories():
    """List all categories for firmware update"""
    print_header("CATEGORIES AVAILABLE")

    folders = sorted([f for f in BASE_DIR.iterdir() if f.is_dir() and not f.name.startswith('.')])

    print("The following categories are ready:\n")
    for i, folder in enumerate(folders, 1):
        # Count files
        files = list(folder.rglob("*.txt"))
        files = [f for f in files if f.name != "index.txt"]

        category_name = folder.name.replace('_', ' ').title()
        print(f"  {i:2}. {category_name:30} ({len(files)} articles)")

    print()
    print("=" * 70)
    print("FIRMWARE UPDATE NEEDED")
    print("=" * 70)
    print()
    print("To add these categories to your M5Cardputer firmware:")
    print()
    print("Edit src/the_book.cpp and update the categories[] array:")
    print()
    print("BookCategory categories[] = {")
    for folder in folders:
        category_name = folder.name.replace('_', ' ').title()
        path = f"/the_book/{folder.name}"
        print(f'  {{"{category_name}", "{path}", TFT_BLUE}},')
    print("};")
    print(f"const int totalCategories = {len(folders)};")
    print()

def main():
    """Master build process"""
    print()
    print("╔" + "═" * 68 + "╗")
    print("║" + " " * 20 + "THE BOOK - COMPLETE BUILD" + " " * 23 + "║")
    print("╚" + "═" * 68 + "╝")

    if not BASE_DIR.exists():
        print(f"\n❌ ERROR: {BASE_DIR} directory not found")
        return 1

    # Step 1: Cleanup
    cleanup()

    # Step 2: Build indexes
    if not build_indexes():
        return 1

    # Step 3: Format content
    if not format_content():
        return 1

    # Step 4: List categories
    list_categories()

    # Final summary
    print_header("✨ BUILD COMPLETE!")
    print("Your content is ready for the M5Cardputer!")
    print()
    print("NEXT STEPS:")
    print("  1. Copy the_book/ folder to your SD card")
    print("  2. Update firmware with new categories (see above)")
    print("  3. Compile and upload to M5Cardputer")
    print()

    return 0

if __name__ == "__main__":
    sys.exit(main())
