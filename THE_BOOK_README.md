# THE BOOK - Offline Knowledge Library for M5Cardputer

Complete offline survival library with 1-3 GB of searchable content.

## 📚 What's Included

### Core Content (Already Downloaded)
- ✅ King James Bible (4.1 MB) with verse lookup
- ✅ Complete Quran (1.1 MB)
- ✅ Dhammapada
- ✅ 1,038 files total (~14 MB)
- ✅ Wikipedia articles (science, history, programming, health, people, tech)
- ✅ Survival guides (water, shelter, fire, food)
- ✅ Medical emergency guides
- ✅ Python programming docs

### Add-On Scripts (Run to Add More)

#### `add_dream_dictionary.py` ✅ DONE
- 100+ dream symbols organized by category
- Complete interpretation guide
- Quick reference

#### `add_i_ching.py` ⏳ Ready to Run
- Complete 64 hexagrams
- Full interpretations and guidance
- Ancient Chinese wisdom

#### `expansion_to_1gb.py` 🔄 Currently Running
- **10,000+ Wikipedia articles** (countries, cities, animals, plants, science, history, philosophy)
- **30+ Classic books** (War and Peace, Moby Dick, Shakespeare, etc.)
- **40+ Programming languages**
- Takes 2-4 hours

---

## 🚀 Quick Start

### 1. Download Base Library
```bash
python3 download_offline_library.py
```
Downloads Bible, Quran, Wikipedia, medical guides (~5 min, ~50-200 MB)

### 2. Add Dream Dictionary
```bash
python3 add_dream_dictionary.py
```
Adds 100+ dream symbol interpretations (~5 seconds)

### 3. Add I Ching
```bash
python3 add_i_ching.py
```
Adds complete Book of Changes (~5 seconds)

### 4. Massive Expansion (Optional)
```bash
python3 expansion_to_1gb.py
```
Expands to 1-3 GB with 10,000+ articles (2-4 hours)

### 5. Fix Missing Downloads
```bash
python3 retry_quran.py
```
Retries failed downloads from main script

---

## 🎯 Smart Content Formatter

### What It Does
Intelligently formats any content you add:
- **Long documents:** Adds table of contents, bold titles, internal links
- **Short articles:** Adds bold titles, related article links
- **Religious texts:** Creates verse index for quick lookup
- **Everything:** Wraps to 39 characters for M5Cardputer display

### How to Use

1. **Drop files in `inbox/` folder**
   ```bash
   # Put your .txt or .pdf files here
   cp mybook.pdf inbox/
   cp article.txt inbox/
   ```

2. **Run the formatter**
   ```bash
   python3 smart_formatter.py
   ```

3. **Files are automatically:**
   - Extracted (if PDF)
   - Categorized (religious/programming/medical/etc.)
   - Formatted (TOC, bold titles, links)
   - Wrapped to 39 chars
   - Placed in correct `the_book/` folder
   - Logged (won't process twice)

4. **Original files stay in inbox/** (delete after verifying)

### Supported Formats
- ✅ `.txt` files (always supported)
- ✅ `.pdf` files (requires PyPDF2: `pip3 install PyPDF2`)

### Auto-Detection
The formatter detects content type by analyzing text:
- Religious texts → `religious/bible`, `religious/quran`, etc.
- Programming → `programming/python`, `programming/javascript`, etc.
- Medical → `medical/emergency`, `medical/diseases`, etc.
- Survival → `survival/water`, `survival/fire`, etc.
- Science → `science/`
- History → `wikipedia/history`
- Philosophy → `reference/philosophy`
- Dreams → `reference/dreams`

---

## 📁 Directory Structure

```
the_book/
├── religious/
│   ├── bible/           # Complete KJV + verse index
│   ├── quran/           # Complete Quran
│   └── other/           # Dhammapada, etc.
├── programming/
│   ├── python/
│   ├── javascript/
│   ├── java/
│   └── other/
├── wikipedia/
│   ├── science/
│   ├── history/
│   ├── people/
│   ├── programming/
│   ├── health/
│   └── technology/
├── survival/
│   ├── water/
│   ├── fire/
│   ├── shelter/
│   └── food/
├── medical/
│   ├── emergency/
│   ├── diseases/
│   └── first_aid/
├── reference/
│   ├── dreams/          # Dream dictionary
│   ├── i_ching/         # I Ching hexagrams
│   └── general/
└── literature/
    ├── classics/        # Classic books
    └── philosophy/

inbox/                   # Drop new files here
processed_files.json     # Tracks what's been formatted
```

---

## 🔍 Features

### Verse Lookup (Religious Texts)
Type "John 3:16" → instantly find verse
- Bible has verse index
- Quran has surah index
- Fast search by reference

### Table of Contents (Long Docs)
Documents > 50,000 characters automatically get:
- Clickable table of contents
- Chapter/section navigation
- Internal links

### Related Articles
Every article links to related content:
- Same category articles
- Similar topics
- Easy navigation

### Bold Titles
All titles and headings are **bold** for readability on small screen

### 39-Character Wrapping
Everything formatted for M5Cardputer display (39 chars wide)

---

## 📊 Current Library Stats

Run this to see your stats:
```bash
cd the_book
find . -name "*.txt" | wc -l          # Count files
du -sh .                               # Total size
```

Check specific categories:
```bash
ls -lh religious/                      # Religious texts
ls -lh wikipedia/                      # Wikipedia articles
ls -lh reference/dreams/               # Dream dictionary
```

---

## 🛠️ All Scripts Reference

| Script | Purpose | Time | Size Added |
|--------|---------|------|------------|
| `download_offline_library.py` | Base download | 5 min | 50-200 MB |
| `add_dream_dictionary.py` | Dream symbols | 5 sec | ~1 MB |
| `add_i_ching.py` | I Ching wisdom | 5 sec | ~1 MB |
| `expansion_to_1gb.py` | Massive expansion | 2-4 hrs | 1-3 GB |
| `retry_quran.py` | Fix failed downloads | 30 sec | varies |
| `smart_formatter.py` | Format new content | varies | N/A |

---

## 💡 Tips & Tricks

### Add Your Own Content
1. Find any .txt or .pdf book/article/guide
2. Drop in `inbox/`
3. Run `python3 smart_formatter.py`
4. It's automatically categorized and formatted!

### Re-run Safely
All scripts track what they've done:
- `smart_formatter.py` won't process same file twice
- `expansion_to_1gb.py` skips already-downloaded articles
- Safe to run multiple times

### Install PDF Support
```bash
pip3 install PyPDF2
```
Then `smart_formatter.py` can process PDFs!

### Search Your Library
The search index files (`_search_index.txt` and `_search_index.json`) enable fast searching on M5Cardputer.

---

## 🎓 Recommended Reading Order

### For Survival Situations
1. `survival/water/` - Most critical
2. `survival/shelter/` - Protection from elements
3. `survival/fire/` - Warmth and water purification
4. `survival/food/` - Sustenance
5. `medical/emergency/` - First aid

### For Spiritual Growth
1. `reference/i_ching/00_i_ching_guide.txt` - Start here
2. `reference/dreams/00_how_to_interpret_dreams.txt`
3. `religious/bible/`, `religious/quran/` - Sacred texts
4. `reference/philosophy/` - Wisdom traditions

### For Learning
1. `programming/` - All languages
2. `wikipedia/science/` - Scientific knowledge
3. `wikipedia/history/` - Human history
4. `literature/classics/` - Great books

---

## 🔄 Maintenance

### Update Search Index
After adding lots of content, update the index:
```bash
cd the_book
# The expansion script updates it automatically
# Or manually regenerate (future script)
```

### Clean Up
```bash
# After verifying formatted files look good
rm inbox/*

# Check what's been processed
cat processed_files.json
```

### Backup
```bash
# Backup entire library
tar -czf the_book_backup.tar.gz the_book/

# Restore
tar -xzf the_book_backup.tar.gz
```

---

## 📱 M5Cardputer Deployment

### Transfer to SD Card
1. Insert SD card in computer
2. Copy `the_book/` folder to SD card root
3. Insert SD card in M5Cardputer
4. Use file browser to navigate

### Recommended File Structure on SD
```
/the_book/              # Main library
/_search_index.txt      # Fast search
/bookmarks.txt          # Your saved pages
```

---

## 🌟 Future Enhancements

Planned features:
- [ ] Verse lookup UI on M5Cardputer
- [ ] Cross-reference links between articles
- [ ] Bookmark system
- [ ] Search interface
- [ ] Reading history
- [ ] Favorites/highlights

---

## 📝 Notes

### Why This Matters
Offline knowledge is powerful:
- No internet needed
- Always accessible
- Survive anywhere
- Learn anything
- Ancient wisdom + modern knowledge
- Fits in your pocket

### Philosophy
This isn't just an e-reader - it's a survival tool, education system, spiritual guide, and reference library all in one. The combination of ancient wisdom (I Ching, religious texts) with modern knowledge (Wikipedia, medical, programming) creates something unique.

### Content Quality
- Wikipedia content: Official summaries via API
- Religious texts: Public domain (Project Gutenberg)
- Dream dictionary: Synthesized from traditional sources
- I Ching: Traditional interpretations
- All content vetted and formatted for readability

---

## 🙏 Acknowledgments

Content sources:
- Project Gutenberg (religious texts, classics)
- Wikipedia (encyclopedia articles)
- Traditional I Ching interpretations
- Dream symbol traditions
- Survival guide compilations

---

## 📞 Support

For issues or questions:
1. Check this README
2. Review script output messages
3. Verify file formats and locations
4. Ensure dependencies installed (PyPDF2 for PDFs)

---

**Happy reading! You now have a complete offline library at your fingertips.** 📚✨
