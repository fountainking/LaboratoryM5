# The Book - Workflow Guide

## Adding New Content

### Quick Start - One Command!

```bash
python3 tools/complete_build.py
```

This single command:
- ✅ Cleans up old files
- ✅ Builds indexes for all categories
- ✅ Formats everything for M5Cardputer
- ✅ Shows you which categories are ready

### Step-by-Step

**1. Add Your Content**
```bash
# Just drop .txt files into the appropriate folder:
the_book/
  ├── wikipedia/        # Wikipedia articles
  ├── science/          # Science topics
  ├── history/          # Historical content
  ├── medical/          # Medical/health info
  ├── survival/         # Survival guides
  ├── religious/        # Religious texts
  ├── literature/       # Books and classics
  ├── programming/      # Code tutorials
  └── [any new folder]/ # Create your own!
```

**2. Run the Build**
```bash
python3 tools/complete_build.py
```

**3. Copy to SD Card**
```bash
# Copy the entire the_book/ folder to your SD card
cp -r the_book/ /Volumes/YOUR_SD_CARD/
```

**4. Update Firmware** (if you added new categories)
```bash
# The build script will show you what to add
# Edit src/the_book.cpp and update categories[] array
# Then compile and upload
pio run --target upload
```

## Customizing Formatting

### Text Size & Display

Edit `formatting_config.json`:

```json
{
  "display": {
    "line_width": 39,          // Characters per line
    "text_size": 2,            // 1=small, 2=medium, 3=large
    "use_bold_text": true,     // Make body text bold
    "lines_per_page": 13       // Lines visible on screen
  }
}
```

### Other Options

```json
{
  "title_style": {
    "make_bold": true,         // Bold titles
    "make_uppercase": true,    // UPPERCASE titles
    "underline_char": "="      // Underline with = chars
  },

  "features": {
    "add_category_breadcrumb": true,    // Show category path
    "add_table_of_contents": true,      // TOC for long docs
    "add_related_articles": true,       // Related links
    "add_metadata_footer": true         // Word count, etc
  }
}
```

After editing, run: `python3 tools/complete_build.py`

## File Organization Tips

### Good Structure
```
the_book/science/physics/quantum_mechanics.txt  ✅
the_book/religious/quran/Complete_Quran.txt     ✅
the_book/literature/classics/Moby_Dick.txt      ✅
```

### What Gets Indexed
- ✅ All `.txt` files in category folders
- ✅ Files in subfolders (like `quran/Complete_Quran.txt`)
- ❌ Files starting with `_` (private files)
- ❌ `index.txt` files (auto-generated)

## Troubleshooting

**"Index not found" error on device:**
- Run `python3 tools/complete_build.py`
- Make sure you copied the_book/ folder to SD card

**"Can't find article" error:**
- Check that your files are .txt format
- Ensure files don't start with underscore
- Rebuild indexes: `python3 tools/build_indexes.py`

**Text too small/large:**
- Edit `formatting_config.json`
- Change `text_size`: 1 (small), 2 (medium), or 3 (large)
- Run `python3 tools/complete_build.py`

**Need to add a new category:**
1. Create folder in `the_book/`
2. Add .txt files
3. Run `python3 tools/complete_build.py`
4. Update firmware with the categories shown
5. Upload new firmware

## Advanced Usage

### Manual Steps

```bash
# Just build indexes (no formatting)
python3 tools/build_indexes.py

# Just format content (no index rebuild)
python3 smart_formatter.py
```

### Git Workflow

```bash
# After adding content
git add the_book/
git commit -m "Added new science articles"
git push

# After testing changes
git add formatting_config.json
git commit -m "Increased text size for readability"
git push
```

## That's It!

The workflow is designed to be simple:
1. Drop files in the_book/
2. Run `python3 tools/complete_build.py`
3. Copy to SD card
4. Enjoy!

Questions? Check the repo issues or README.
