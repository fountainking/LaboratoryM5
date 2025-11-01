#!/usr/bin/env python3
"""
Create verse-by-verse index for Bible and Quran
Allows direct verse lookup like "John 3:16" or "Genesis 1:1"
"""

import re
from pathlib import Path

BOOK_PATH = Path("/Users/jamesfauntleroy/Documents/PlatformIO/Projects/LaboratoryM5/the_book")

# Bible book name variations and abbreviations
BIBLE_BOOKS = {
    "Genesis": ["gen", "gn"],
    "Exodus": ["exod", "ex"],
    "Leviticus": ["lev", "lv"],
    "Numbers": ["num", "nm"],
    "Deuteronomy": ["deut", "dt"],
    "Joshua": ["josh", "jos"],
    "Judges": ["judg", "jdg"],
    "Ruth": ["ruth", "ru"],
    "1 Samuel": ["1sam", "1sm"],
    "2 Samuel": ["2sam", "2sm"],
    "1 Kings": ["1kgs", "1ki"],
    "2 Kings": ["2kgs", "2ki"],
    "1 Chronicles": ["1chr", "1ch"],
    "2 Chronicles": ["2chr", "2ch"],
    "Ezra": ["ezr"],
    "Nehemiah": ["neh", "ne"],
    "Esther": ["esth", "es"],
    "Job": ["job", "jb"],
    "Psalms": ["ps", "psa", "psalm"],
    "Proverbs": ["prov", "pr"],
    "Ecclesiastes": ["eccl", "ec"],
    "Song of Solomon": ["song", "ss"],
    "Isaiah": ["isa", "is"],
    "Jeremiah": ["jer", "je"],
    "Lamentations": ["lam", "la"],
    "Ezekiel": ["ezek", "eze"],
    "Daniel": ["dan", "dn"],
    "Hosea": ["hos", "ho"],
    "Joel": ["joel", "jl"],
    "Amos": ["amos", "am"],
    "Obadiah": ["obad", "ob"],
    "Jonah": ["jonah", "jon"],
    "Micah": ["mic", "mi"],
    "Nahum": ["nah", "na"],
    "Habakkuk": ["hab", "hb"],
    "Zephaniah": ["zeph", "zep"],
    "Haggai": ["hag", "hg"],
    "Zechariah": ["zech", "zec"],
    "Malachi": ["mal", "ml"],
    "Matthew": ["matt", "mt"],
    "Mark": ["mark", "mr", "mk"],
    "Luke": ["luke", "lk"],
    "John": ["john", "jn"],
    "Acts": ["acts", "ac"],
    "Romans": ["rom", "ro"],
    "1 Corinthians": ["1cor", "1co"],
    "2 Corinthians": ["2cor", "2co"],
    "Galatians": ["gal", "ga"],
    "Ephesians": ["eph", "ep"],
    "Philippians": ["phil", "php"],
    "Colossians": ["col", "co"],
    "1 Thessalonians": ["1thess", "1th"],
    "2 Thessalonians": ["2thess", "2th"],
    "1 Timothy": ["1tim", "1ti"],
    "2 Timothy": ["2tim", "2ti"],
    "Titus": ["titus", "tit"],
    "Philemon": ["phlm", "phm"],
    "Hebrews": ["heb", "he"],
    "James": ["jas", "jm"],
    "1 Peter": ["1pet", "1pe"],
    "2 Peter": ["2pet", "2pe"],
    "1 John": ["1john", "1jn"],
    "2 John": ["2john", "2jn"],
    "3 John": ["3john", "3jn"],
    "Jude": ["jude", "jud"],
    "Revelation": ["rev", "re"]
}

def index_bible():
    """Create verse index for Bible"""
    bible_file = BOOK_PATH / "religious/bible/Complete_KJV_Bible.txt"

    if not bible_file.exists():
        print("⚠️  Bible file not found")
        return

    print("📖 Indexing Bible verses...")

    with open(bible_file, 'r', encoding='utf-8', errors='ignore') as f:
        content = f.read()

    # Find verse patterns like "1:1 In the beginning..."
    verse_pattern = re.compile(r'^(\d+):(\d+)\s+(.{20,120})', re.MULTILINE)

    verses = []
    current_book = None
    line_num = 0

    for line in content.split('\n'):
        line_num += 1

        # Detect book headers
        for book_name in BIBLE_BOOKS.keys():
            if book_name in line and ("Book of" in line or "Gospel" in line or "Epistle" in line):
                current_book = book_name
                print(f"   Found book: {book_name}")
                break

        # Match verses
        match = verse_pattern.match(line.strip())
        if match and current_book:
            chapter = match.group(1)
            verse = match.group(2)
            text = match.group(3).strip()

            # Create searchable entry
            # Format: BOOK CHAPTER:VERSE|bible/Complete_KJV_Bible.txt|LINE|TEXT
            entry = f"{current_book} {chapter}:{verse}|bible/Complete_KJV_Bible.txt|{line_num}|{text}"
            verses.append(entry)

            # Also add abbreviated versions
            for abbrev in BIBLE_BOOKS.get(current_book, []):
                entry_abbrev = f"{abbrev} {chapter}:{verse}|bible/Complete_KJV_Bible.txt|{line_num}|{text}"
                verses.append(entry_abbrev)

    print(f"   ✅ Indexed {len(verses)} Bible verse entries")

    # Write verse index
    verse_index_path = BOOK_PATH / "religious/bible_verse_index.txt"
    with open(verse_index_path, 'w', encoding='utf-8') as f:
        f.write("**BIBLE VERSE INDEX**\n\n")
        f.write("Search format: Book Chapter:Verse (e.g., 'John 3:16', 'Gen 1:1')\n")
        f.write("="*60 + "\n\n")
        for entry in verses:
            f.write(entry + "\n")

    print(f"   💾 Saved to bible_verse_index.txt")

def index_quran():
    """Create verse index for Quran"""
    quran_file = BOOK_PATH / "religious/quran/Complete_Quran.txt"

    if not quran_file.exists():
        print("⚠️  Quran file not found")
        return

    print("📗 Indexing Quran verses...")

    # TODO: Index Quran verses (Surah:Ayah format)
    # Format similar to Bible but with Surah numbers

    print("   ⚠️  Quran verse indexing needs Surah:Ayah pattern detection")

def main():
    print("🔍 Creating scripture verse indexes...\n")
    index_bible()
    index_quran()
    print("\n✅ Scripture indexing complete!")

if __name__ == "__main__":
    main()
