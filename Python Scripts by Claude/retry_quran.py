#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
RETRY QURAN DOWNLOAD
Quick script to retry downloading Quran and other religious texts
"""

import requests
import time
from pathlib import Path

BASE_DIR = Path("the_book")
MAX_LINE_WIDTH = 39

HEADERS = {
    'User-Agent': 'M5CardputerOfflineLibrary/1.0 (Educational)'
}

def wrap_text(text, width=MAX_LINE_WIDTH):
    """Wrap text to M5Cardputer display width"""
    lines = []
    for paragraph in text.split('\n'):
        if not paragraph.strip():
            lines.append('')
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

print("=" * 70)
print("RETRY QURAN AND RELIGIOUS TEXTS DOWNLOAD")
print("=" * 70)
print()

# Create directory if needed
(BASE_DIR / "religious/quran").mkdir(parents=True, exist_ok=True)
(BASE_DIR / "religious/other").mkdir(parents=True, exist_ok=True)

# Try multiple sources for Quran
quran_sources = [
    {
        "name": "Project Gutenberg (English Translation)",
        "url": "https://www.gutenberg.org/files/2800/2800-0.txt",
        "filename": "Complete_Quran.txt"
    },
    {
        "name": "Project Gutenberg (Alternative)",
        "url": "https://www.gutenberg.org/cache/epub/2800/pg2800.txt",
        "filename": "Complete_Quran.txt"
    }
]

print("Attempting to download Quran...\n")

quran_success = False

for source in quran_sources:
    print(f"  Trying: {source['name']}")
    try:
        response = requests.get(source['url'], headers=HEADERS, timeout=60)

        if response.status_code == 200:
            content = response.text

            # Clean Project Gutenberg headers
            start = content.find("***START")
            if start != -1:
                start = content.find('\n', start) + 1
            end = content.find("***END")
            if end != -1:
                content = content[start:end]

            # Save
            path = BASE_DIR / "religious/quran" / source['filename']
            with open(path, 'w', encoding='utf-8') as f:
                f.write(wrap_text(content))

            print(f"    SUCCESS! Downloaded ({len(content)//1000}KB)")
            print(f"    Saved to: {path}")
            quran_success = True
            break
        else:
            print(f"    FAILED: HTTP {response.status_code}")

    except Exception as e:
        print(f"    FAILED: {e}")
        continue

if not quran_success:
    print("\n  WARNING: All sources failed. Possible reasons:")
    print("     - Project Gutenberg is temporarily down")
    print("     - Network connection issue")
    print("     - Firewall blocking connection")
    print("\n  Try again later or download manually from:")
    print("     https://www.gutenberg.org/ebooks/2800")

print()

# Also retry other religious texts
print("Checking other religious texts...\n")

other_texts = {
    "Bhagavad_Gita.txt": "https://www.gutenberg.org/files/2388/2388-0.txt",
    "Tao_Te_Ching.txt": "https://www.gutenberg.org/files/216/216-0.txt",
    "Dhammapada.txt": "https://www.gutenberg.org/files/2017/2017-0.txt"
}

for filename, url in other_texts.items():
    # Check if already exists
    path = BASE_DIR / "religious/other" / filename

    if path.exists():
        print(f"  OK: {filename} already exists")
        continue

    print(f"  Downloading {filename}...")
    try:
        response = requests.get(url, headers=HEADERS, timeout=30)

        if response.status_code == 200:
            content = response.text

            # Clean headers
            start = content.find("***START")
            if start != -1:
                start = content.find('\n', start) + 1
            end = content.find("***END")
            if end != -1:
                content = content[start:end]

            with open(path, 'w', encoding='utf-8') as f:
                f.write(wrap_text(content))

            print(f"    Done ({len(content)//1000}KB)")
        else:
            print(f"    FAILED: HTTP {response.status_code}")

        time.sleep(0.5)

    except Exception as e:
        print(f"    FAILED: {e}")

print()
print("=" * 70)
print("RETRY COMPLETE")
print("=" * 70)
print()

if quran_success:
    print("SUCCESS: Quran downloaded!")
else:
    print("FAILED: Quran download failed - try manual download")

print()
print("Check the_book/religious/ folder for all files")
print("=" * 70)
