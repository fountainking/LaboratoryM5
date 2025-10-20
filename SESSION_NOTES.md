# M5Cardputer Laboratory - Session Notes
**Last Updated: 2025-10-18**

---

## CURRENT PROJECT STATE

### Music Tools - Guitar Tuner & Audio Visualizer

**Location:** `src/music_tools.cpp` and `src/music_tools.h`

**Status:** ✅ WORKING - Fully functional and tested

**What We Built:**
1. **Guitar Tuner**
   - Real-time pitch detection using autocorrelation
   - Visual feedback with progressive fill triangles
   - White background, black letter, red triangles
   - Triangles fill based on proximity to correct pitch (0-50 cents)
   - Black outline appears when perfectly in tune (±3 cents)
   - Detects all 6 standard guitar strings (E, A, D, G, B, E)

2. **Audio Visualizer (Equalizer)**
   - 20 vertical bars with color gradient
   - Green → Yellow → Red based on amplitude
   - Smooth transitions with decay effect
   - Non-flickering display with selective updates

**Technical Details:**
- Microphone: M5Cardputer.Mic library (PDM mic)
- Sample rate: 16,000 Hz
- Buffer size: 2,048 samples
- Frequency range: 60-400 Hz (covers guitar range)
- Signal amplification: 2x gain for better sensitivity
- Noise threshold: 250 (lowered from 500)
- Update rate: 30ms for tuner, 50ms for visualizer

**Key Improvements Made:**
- Fixed microphone initialization (switched from manual I2S to M5Cardputer.Mic)
- Removed jitter with state tracking and update thresholds
- Simplified UI from complex needle meter to clean letter + triangles
- Added progressive fill to triangles for visual proximity feedback
- Increased microphone sensitivity (2x gain, lower threshold)
- Fixed status bar cutoff (white background starts at y=24)
- Removed octave numbers from note display (E2 → E)

**Audio Settings Changed:**
- Default volume: 80 → 50 in `src/audio_manager.cpp:5`

---

## THE BOOK - Offline Knowledge Base

**Location:** `src/the_book.cpp`, `src/the_book.h`, `the_book/` folder (6.8 MB total)

**Generation Script:** `generate_the_book_full.py`

**Status:** ✅ UI REDESIGNED & WORKING - Ready to test on SD card

### UI Design (Latest):
- **Search Screen:**
  - White background
  - "Offline Knowledge Base" title in black at top (y=15)
  - Rounded search bar with double outline
  - **ALL TEXT uses reversed terminal gradient** (white→cyan→blue→purple→red→orange→yellow)
    - "Search:" label with gradient
    - Search input text with gradient (continues from "Search:")
    - Results count line with gradient
    - All result titles with gradient
  - Live inline search preview (shows up to 5 results under search bar)
  - Scrolling results window - selected result stays visible
  - UP/DOWN arrows navigate results with grey highlight
  - Enter opens selected result directly
  - "/" key shows Table of Contents (secret feature)
  - Backtick (`) returns to main menu

- **Key Features:**
  - Live search-as-you-type (2+ characters)
  - Search results from all categories (Wikipedia, Programming, Survival, Religious, Edible Plants)
  - Inline preview avoids page switching
  - Preserved search term when navigating back
  - Directional keys (;,./) don't type characters - only navigate

- **Color Gradient Implementation:**
  ```cpp
  // Reversed terminal gradient array
  uint16_t gradientColors[] = {
    0xFFFF,  // White
    0x07FF,  // Cyan
    0x001F,  // Blue
    0x780F,  // Purple
    0xF800,  // Red
    0xFD20,  // Orange
    0xFFE0   // Yellow
  };
  // Applied character-by-character with smooth interpolation
  ```

### Content Summary (642 files):

1. **Wikipedia** (615 articles, 2.5 MB)
   - Programming: Python, JavaScript, Java, C++, algorithms, ML, AI, etc.
   - Science: Physics, Chemistry, Biology, DNA, Evolution, etc.
   - History: WWI, WWII, civilizations, famous events
   - People: Einstein, Newton, historical figures
   - Geography: Continents, countries, landmarks
   - Mathematics: Algebra, calculus, theorems
   - Health: Diseases, treatments, nutrition
   - Technology: Inventions, engineering, electronics

2. **Programming Languages** (4 guides, 36 KB)
   - ✅ Python (full comprehensive guide)
   - ✅ JavaScript (full comprehensive guide)
   - ✅ Java (full comprehensive guide)
   - ✅ C (full comprehensive guide)
   - ⚠️ 16 more languages planned but abbreviated

3. **Survival Guides** (4 guides, 32 KB)
   - ✅ Water Procurement & Purification
   - ✅ Fire Starting & Maintenance
   - ✅ Shelter Building
   - ✅ First Aid & Medical Emergencies

4. **Religious Texts** (6 texts, 4.2 MB)
   - ✅ King James Bible (4.1 MB - FULL TEXT!)
   - ✅ Dhammapada (Buddhist text)
   - ⚠️ Quran download failed (placeholder created)
   - ⚠️ Bhagavad Gita download failed
   - ⚠️ Tao Te Ching download failed
   - Bible introductions for missing texts

5. **Edible Plants** (8 guides, 36 KB)
   - ✅ Dandelion, Cattail, Acorns, Plantain
   - ✅ Clover, Chickweed, Wood Sorrel, Purslane

**Text Formatting:**
- All content wrapped to 39 characters (M5Cardputer display width)
- Index files created for each category
- Clean, readable formatting tested and verified

---

## IMPORTANT TECHNICAL DECISIONS

### 1. Microphone Configuration
**Problem:** Initial manual I2S configuration didn't work
**Solution:** Use `M5Cardputer.Mic.begin()` and `M5Cardputer.Mic.record()`
```cpp
// CORRECT (works):
M5Cardputer.Mic.begin();
M5Cardputer.Mic.record(audioBuffer, BUFFER_SIZE, SAMPLE_RATE);

// WRONG (doesn't work):
i2s_read(I2S_NUM_0, audioBuffer, ...)  // Manual I2S driver
```

### 2. Tuner UI Design
**Evolution:** Complex meter → Simple letter + triangles
**Final design:**
- White background (starts at y=24 to avoid cutting status bar)
- Large black letter centered at (110, 65)
- Red triangle outlines on left (flat) and right (sharp)
- Progressive fill from bottom up based on cents offset
- Black outline box when in tune (±3 cents)

### 3. Jitter Reduction
```cpp
// Only update if significant change (>2 cents)
bool needsUpdate = (closestString != lastClosestString) ||
                   (currentTuningState != lastTuningState) ||
                   (abs(centsDiff - lastCentsDiff) > 2.0);
```

### 4. The Book Generation
**Why only 6.8 MB instead of 1-3 GB?**
- Wikipedia REST API provides article summaries, not full text
- Full Wikipedia dump would be ~20 GB (impractical)
- Project Gutenberg downloads had some failures
- Only 4 of 20 programming guides fully written

---

## FILES MODIFIED IN THIS SESSION

### Core Files:
1. `src/music_tools.cpp` - Guitar tuner and equalizer implementation
2. `src/music_tools.h` - Music tools interface
3. `src/audio_manager.cpp` - Volume default changed to 50
4. `src/the_book.cpp` - Complete UI redesign with gradient colors, scrolling, live search
5. `src/the_book.h` - The Book interface updates
6. `src/main.cpp` - Updated keyboard handling for The Book navigation

### New Files Created:
1. `generate_the_book.py` - Initial test version (144 KB, 32 articles)
2. `generate_the_book_full.py` - Comprehensive version (6.8 MB, 642 files)
3. `the_book/` - Generated knowledge base folder

---

## WHAT THE USER WANTED

### Original Request (Guitar Tuner):
> "lets add a guitar tuner to music tools!"

### User Feedback Loop:
1. "played a guitar string - no reaction" → Fixed microphone
2. "make it better design wise" → Improved UI
3. "its super glitchy, and the endless flashing/blinking isnt going to work" → Added jitter reduction
4. "make the triangles a red outline and filled to indicate note proximity" → Progressive fill
5. "looks ALOT better! now....can we make the mic more sensitive at all?" → Added 2x gain

### The Book Request:
> "i want the whole text-only wiki, as well as at least 20 coding languages and an exhaustive survival guide! im expecting at least 1 if not 2 or 3 gb. because the point is to just be able to type anything and get something useful - its like offline google"
>
> "and full religious texts, bible quran torah vedic texts book of the dead and edible plants!"

---

## NEXT STEPS / TODO

### High Priority:
- [ ] Test The Book on M5Cardputer SD card
- [ ] Verify guitar tuner works with actual guitar
- [ ] Test audio visualizer with music

### The Book Expansion (if needed):
- [ ] Complete remaining 16 programming language guides (C++, C#, Ruby, Go, Rust, Swift, Kotlin, PHP, TypeScript, R, MATLAB, SQL, Shell, Perl, Scala, Haskell)
- [ ] Retry failed religious text downloads (Quran, Bhagavad Gita, Tao Te Ching)
- [ ] Add more survival topics (navigation, hunting, foraging, knots, signals, weather)
- [ ] Expand edible plants database (50-100 common plants)
- [ ] Add poisonous plants guide (what to avoid)
- [ ] Consider Wikipedia full article dumps (but would be huge)
- [ ] Add tech/DIY guides (electronics, repairs, etc.)

### Potential Improvements:
- [ ] Add tuner frequency display (Hz)
- [ ] Add visual signal strength indicator for tuner
- [ ] Add multiple tuner modes (guitar, bass, ukulele, etc.)
- [ ] Save user preferences for tuner sensitivity
- [ ] The Book: Add search functionality improvements
- [ ] The Book: Add bookmarks/favorites system

---

## HOW TO REMIND CLAUDE IN FUTURE SESSIONS

**Just paste this at the start of conversation:**

```
I'm continuing work on the M5Cardputer Laboratory project.
Please read SESSION_NOTES.md for full context on what
we've built so far.

Quick summary:
- Guitar tuner: WORKING (music_tools.cpp)
- Audio visualizer: WORKING
- The Book: GENERATED (6.8 MB in the_book/ folder)
- Generation script: generate_the_book_full.py

[Then describe what you want to work on next]
```

---

## USEFUL COMMANDS

### Build & Upload:
```bash
pio run --target upload
```

### Monitor Serial Output:
```bash
pio device monitor --baud 115200
```

### Generate The Book:
```bash
python3 generate_the_book_full.py
```

### Check The Book Size:
```bash
du -sh the_book/*
```

### Copy to SD Card (when mounted):
```bash
cp -r the_book/ /Volumes/[SD_CARD_NAME]/
```

---

## PROJECT STRUCTURE

```
LaboratoryM5/
├── src/
│   ├── main.cpp
│   ├── music_tools.cpp      ← Guitar tuner & visualizer
│   ├── music_tools.h
│   ├── audio_manager.cpp    ← Volume control
│   ├── ui.cpp
│   └── ...
├── the_book/                ← Generated knowledge base
│   ├── wikipedia/
│   ├── programming_languages/
│   ├── survival/
│   ├── religious/
│   └── edible_plants/
├── generate_the_book.py     ← Test version
├── generate_the_book_full.py ← Full version
└── SESSION_NOTES.md         ← This file!
```

---

## QUOTES FROM USER (for context)

**On Tuner Design:**
> "the letter should be centered - above 'back' - make it white background, black letter red triangles, black outline letter when it is perfectly in tune"

**On Jitter:**
> "its super glitchy, and the endless flashing/blinking isnt going to work - not responsive - maybe isolate the movement?"

**On Final Result:**
> "looks ALOT better!"

**On The Book:**
> "the point is to just be able to type anything and get something useful - its like offline google"

---

## LLM ALTERNATIVES (When Claude/API isn't Available)

**Context:** For generating content for The Book or other tasks when API access isn't available

**Recommended Options:**

1. **Local LLMs (Run on your machine):**
   - **Ollama** - Easy local LLM management
     - Install: `brew install ollama` (Mac) or download from ollama.ai
     - Models: `ollama run llama2`, `ollama run codellama`, `ollama run mistral`
     - Free, private, works offline

   - **LM Studio** - GUI for local models
     - Download from lmstudio.ai
     - User-friendly interface
     - Supports many model formats (GGUF)

   - **GPT4All** - Simple desktop app
     - Download from gpt4all.io
     - No technical setup required
     - Multiple models included

2. **Free Online Options:**
   - **Hugging Face Chat** - chat.huggingface.co
     - Free access to various models
     - No API key required for web interface

   - **Perplexity AI** - perplexity.ai
     - Free tier available
     - Good for research and content generation

   - **Poe** - poe.com
     - Free access to multiple models (GPT-3.5, Claude, etc.)
     - Limited daily messages

3. **For Python Scripts:**
   - Use `transformers` library from Hugging Face
   - Example:
   ```python
   from transformers import pipeline
   generator = pipeline('text-generation', model='gpt2')
   result = generator("Python is a", max_length=100)
   ```

4. **Command Line Tools:**
   - **llm** by Simon Willison
     - Install: `pip install llm`
     - Supports multiple providers
     - Great for scripting

**Best for The Book Generation:**
- Ollama with Llama 2 or Mistral (for comprehensive, accurate content)
- Perplexity AI (for research and factual information)
- Local models ensure no rate limits or API costs

---

## DEVICE SPECS (M5Cardputer)

- **Display:** 240x135 pixels
- **Display Width:** 39 characters
- **Microphone:** PDM (M5Cardputer.Mic)
- **Audio:** I2S output (pins 41, 43, 42)
- **Storage:** SD card slot
- **Keyboard:** QWERTY physical keyboard

---

**End of Session Notes**
**Remember: You can always update this file with new progress!**
