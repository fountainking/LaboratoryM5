#!/usr/bin/env python3
"""
The Book Content Generator for M5Cardputer
Downloads and formats Wikipedia articles, tech docs, survival guides, and religious texts
"""

import os
import requests
import re
from pathlib import Path
import time

# Display settings for M5Cardputer
MAX_LINE_LENGTH = 39

def create_folder_structure():
    """Create the folder structure for The Book"""
    base_path = Path("the_book")
    categories = ["wikipedia", "tech", "survival", "religious"]

    for category in categories:
        (base_path / category).mkdir(parents=True, exist_ok=True)

    print("✓ Created folder structure")
    return base_path

def wrap_text(text, max_length=MAX_LINE_LENGTH):
    """Wrap text to fit M5Cardputer display"""
    lines = []
    for paragraph in text.split('\n'):
        if not paragraph.strip():
            lines.append("")
            continue

        words = paragraph.split()
        current_line = ""

        for word in words:
            if len(current_line + " " + word) <= max_length:
                if current_line:
                    current_line += " " + word
                else:
                    current_line = word
            else:
                if current_line:
                    lines.append(current_line)
                current_line = word

        if current_line:
            lines.append(current_line)

    return lines

def clean_text(text):
    """Clean text for display"""
    # Remove special characters but keep basic punctuation
    text = re.sub(r'[^\x00-\x7F]+', '', text)  # Remove non-ASCII
    text = re.sub(r'\s+', ' ', text)  # Normalize whitespace
    return text.strip()

def download_wikipedia_article(title):
    """Download a Wikipedia article in plain text"""
    try:
        url = f"https://en.wikipedia.org/api/rest_v1/page/summary/{title}"
        headers = {
            'User-Agent': 'TheBookGenerator/1.0 (Educational M5Cardputer Project)'
        }
        response = requests.get(url, headers=headers, timeout=10)

        if response.status_code == 200:
            data = response.json()

            # Get extract (summary)
            content = data.get('extract', '')

            if not content:
                return None

            # Clean and format
            content = clean_text(content)

            # Create article content
            article = f"{data.get('title', title)}\n\n{content}\n"

            return {
                'title': data.get('title', title),
                'content': article,
                'preview': content[:60] + "..." if len(content) > 60 else content
            }

    except Exception as e:
        print(f"  ✗ Failed to download {title}: {e}")

    return None

def download_wikipedia_articles():
    """Download popular Wikipedia articles"""
    print("\n📚 Downloading Wikipedia articles...")

    articles = [
        "Python_(programming_language)",
        "JavaScript",
        "Linux",
        "Artificial_intelligence",
        "Solar_System",
        "Photosynthesis",
        "World_War_II",
        "Albert_Einstein",
        "Climate_change",
        "Human_body",
        "Internet",
        "Quantum_mechanics",
        "Evolution",
        "DNA",
        "Gravity",
        "Electricity",
        "Computer",
        "Periodic_table",
        "Mathematics",
        "Philosophy"
    ]

    downloaded = []

    for article_name in articles:
        print(f"  Downloading: {article_name.replace('_', ' ')}")
        article_data = download_wikipedia_article(article_name)

        if article_data:
            downloaded.append(article_data)
            time.sleep(0.5)  # Be nice to Wikipedia's servers

    print(f"✓ Downloaded {len(downloaded)} Wikipedia articles")
    return downloaded

def create_tech_content():
    """Create tech/coding reference content"""
    print("\n💻 Creating tech/coding content...")

    articles = []

    # Python basics
    python_content = """Python Quick Reference

Python is a high-level programming
language known for easy syntax.

BASIC SYNTAX:
# Variables
x = 10
name = "Hello"

# Conditionals
if x > 5:
    print("Greater")
elif x == 5:
    print("Equal")
else:
    print("Less")

# Loops
for i in range(5):
    print(i)

while x > 0:
    x -= 1

# Functions
def greet(name):
    return f"Hello {name}"

# Lists
my_list = [1, 2, 3, 4]
my_list.append(5)

# Dictionaries
person = {"name": "John", "age": 30}

# File handling
with open("file.txt", "r") as f:
    content = f.read()

COMMON LIBRARIES:
- requests: HTTP requests
- numpy: Numerical computing
- pandas: Data analysis
- flask: Web framework
- matplotlib: Plotting
"""

    articles.append({
        'title': 'Python Reference',
        'content': python_content,
        'preview': 'Python programming quick reference'
    })

    # Git commands
    git_content = """Git Quick Reference

Git is a version control system for
tracking code changes.

BASIC COMMANDS:
# Initialize repository
git init

# Clone repository
git clone <url>

# Check status
git status

# Add files
git add .
git add file.txt

# Commit changes
git commit -m "message"

# Push to remote
git push origin main

# Pull from remote
git pull origin main

# Create branch
git branch feature-name
git checkout feature-name

# Merge branch
git checkout main
git merge feature-name

# View history
git log
git log --oneline

# Undo changes
git reset --hard HEAD
git checkout -- file.txt

# Stash changes
git stash
git stash pop

WORKFLOW:
1. git pull
2. Make changes
3. git add .
4. git commit -m "description"
5. git push
"""

    articles.append({
        'title': 'Git Commands',
        'content': git_content,
        'preview': 'Git version control command reference'
    })

    # Linux commands
    linux_content = """Linux Command Reference

Essential commands for Linux/Unix
terminal usage.

FILE OPERATIONS:
ls          # List files
ls -la      # Detailed list
cd dir      # Change directory
pwd         # Print working directory
cp a b      # Copy file
mv a b      # Move/rename file
rm file     # Remove file
rm -rf dir  # Remove directory
mkdir dir   # Make directory
touch file  # Create empty file
cat file    # Display file
less file   # View file (paginated)
head file   # First 10 lines
tail file   # Last 10 lines
find . -name "*.txt"  # Find files

PERMISSIONS:
chmod 755 file    # Change permissions
chmod +x script   # Make executable
chown user file   # Change owner
ls -l            # View permissions

SYSTEM:
ps aux        # List processes
top           # Process monitor
kill PID      # Kill process
df -h         # Disk usage
free -h       # Memory usage
uname -a      # System info

NETWORKING:
ping host     # Test connection
curl url      # Download/request
wget url      # Download file
ssh user@host # Remote connect

TEXT PROCESSING:
grep "text" file    # Search in file
sed 's/old/new/' file  # Replace text
awk '{print $1}' file  # Column extract
wc -l file          # Count lines
"""

    articles.append({
        'title': 'Linux Commands',
        'content': linux_content,
        'preview': 'Essential Linux terminal commands'
    })

    # JavaScript reference
    js_content = """JavaScript Quick Reference

JavaScript is the language of the web,
used for interactive websites.

BASICS:
// Variables
let x = 10;
const name = "John";
var old = true;  // old style

// Conditionals
if (x > 5) {
    console.log("Greater");
} else if (x === 5) {
    console.log("Equal");
} else {
    console.log("Less");
}

// Loops
for (let i = 0; i < 5; i++) {
    console.log(i);
}

while (x > 0) {
    x--;
}

// Functions
function greet(name) {
    return `Hello ${name}`;
}

const add = (a, b) => a + b;

// Arrays
const arr = [1, 2, 3];
arr.push(4);
arr.map(x => x * 2);
arr.filter(x => x > 2);

// Objects
const person = {
    name: "John",
    age: 30,
    greet() {
        return `Hi ${this.name}`;
    }
};

// DOM Manipulation
document.getElementById('id');
document.querySelector('.class');
element.innerHTML = 'text';
element.addEventListener('click', fn);

// Async/Await
async function fetchData() {
    const response = await fetch(url);
    const data = await response.json();
    return data;
}
"""

    articles.append({
        'title': 'JavaScript Reference',
        'content': js_content,
        'preview': 'JavaScript programming reference'
    })

    print(f"✓ Created {len(articles)} tech articles")
    return articles

def create_survival_content():
    """Create survival guide content"""
    print("\n🏕️  Creating survival content...")

    articles = []

    # First aid
    first_aid = """First Aid Basics

Essential emergency medical procedures
for common injuries and conditions.

EMERGENCY PRIORITIES (ABC):
A - Airway: Ensure breathing passage
B - Breathing: Check for respiration
C - Circulation: Check pulse, stop
    bleeding

SEVERE BLEEDING:
1. Apply direct pressure with cloth
2. Elevate wound above heart
3. Don't remove embedded objects
4. Apply pressure to pressure points
5. Use tourniquet only if life-
   threatening

CHOKING (Heimlich Maneuver):
1. Stand behind person
2. Fist above navel, below ribs
3. Grasp fist with other hand
4. Quick upward thrusts
5. Repeat until object dislodged

BURNS:
1st degree: Cool water, aloe
2nd degree: Cool water, cover,
  don't pop blisters
3rd degree: Call 911, cover loosely,
  don't remove clothing

CPR:
1. Check responsiveness
2. Call 911
3. 30 chest compressions (2 inches)
4. 2 rescue breaths
5. Repeat until help arrives
Rate: 100-120 compressions/min

SHOCK:
Signs: Pale, rapid pulse, confusion
Treatment: Lay flat, elevate legs,
  keep warm, don't give food/water

SNAKE BITE:
- Keep calm, immobilize limb
- Remove jewelry/tight clothing
- Keep bite below heart
- DO NOT: cut, suck, ice, tourniquet
- Seek medical help immediately

FRACTURES:
- Don't move unless necessary
- Immobilize joint above and below
- Apply ice wrapped in cloth
- Elevate if possible
- Seek medical attention
"""

    articles.append({
        'title': 'First Aid',
        'content': first_aid,
        'preview': 'Essential emergency medical procedures'
    })

    # Water purification
    water = """Water Purification

Methods to make water safe for
drinking in survival situations.

BOILING (Most Reliable):
1. Bring to rolling boil
2. Boil for 1 minute (3 min at
   high altitude)
3. Let cool before drinking
Kills: All pathogens

CHEMICAL TREATMENT:
Iodine Tablets:
- 2 tablets per liter
- Wait 30 minutes
- Neutralize with Vitamin C

Bleach (unscented):
- 2 drops per liter (clear water)
- 4 drops per liter (cloudy water)
- Wait 30 minutes
- Should smell slightly of chlorine

FILTRATION:
Commercial filter: Follow instructions
DIY filter layers (top to bottom):
1. Cloth
2. Charcoal
3. Sand
4. Gravel
5. Pebbles
Note: Still boil after filtering

SOLAR DISINFECTION (SODIS):
1. Clear plastic bottle
2. Fill with water
3. Place in direct sun 6 hours
   (2 days if cloudy)
Works: UV kills pathogens

SIGNS OF CONTAMINATION:
- Cloudy appearance
- Bad smell
- Dead animals nearby
- Stagnant water
- Downstream from human activity

WATER SOURCES (Best to Worst):
1. Rain water
2. Running streams/rivers
3. Springs
4. Lakes/ponds
5. Swamps/stagnant water

ALWAYS purify unless certain safe!
"""

    articles.append({
        'title': 'Water Purification',
        'content': water,
        'preview': 'Methods to purify water for drinking'
    })

    # Fire starting
    fire = """Fire Starting Methods

Essential techniques for starting
fire in various conditions.

FIRE TRIANGLE (All Required):
1. Heat (spark/friction)
2. Fuel (wood/tinder)
3. Oxygen (air flow)

TINDER (catches spark):
- Dry grass, leaves
- Bark (birch, cedar)
- Cotton balls
- Dryer lint
- Char cloth
- Fine wood shavings

KINDLING (small sticks):
- Pencil-thick dry twigs
- Small dry branches
- Pine needles
- Split softwood

FUEL (sustains fire):
- Finger-thick branches
- Arm-thick logs
- Hardwood for long burning

FRICTION METHODS:
Bow Drill:
1. Spindle in board notch
2. Bow wrapped around spindle
3. Pressure with hand piece
4. Rapid back-forth motion
5. Ember forms in notch

Hand Drill:
1. Vertical spindle in board
2. Roll rapidly between palms
3. Downward pressure
4. Very difficult, requires practice

MODERN METHODS:
Matches: Keep dry, waterproof
Lighter: Most reliable
Ferro rod: Works when wet
Magnifying glass: Needs sun

FIRE LAY STRUCTURES:
Teepee: Quick, hot fire
Log cabin: Long-lasting
Lean-to: Wind protection
Dakota hole: Concealed, efficient

SAFETY:
- Clear area around fire
- Keep water nearby
- Never leave unattended
- Fully extinguish before leaving
- Check local fire regulations
"""

    articles.append({
        'title': 'Fire Starting',
        'content': fire,
        'preview': 'Methods to start fire in survival'
    })

    # Shelter building
    shelter = """Shelter Building

Basic shelter construction for
protection from elements.

PRIORITIES:
1. Location (dry, level, protected)
2. Insulation from ground
3. Protection from wind/rain
4. Ventilation

LEAN-TO SHELTER:
Materials: Branches, leaves/bark
1. Find or make ridgepole between
   two trees (6ft high)
2. Lean branches against ridgepole
   at 45° angle
3. Cover with leaves, bark, debris
4. Make thick (12+ inches)
5. Create bed of leaves inside

A-FRAME SHELTER:
1. Ridge pole between two supports
2. Lean branches on both sides
3. Cover both sides with debris
4. Smaller, better insulated

DEBRIS HUT (Best Insulation):
1. Ridge pole from tree to ground
2. Rib branches along both sides
3. Pile debris 3+ feet thick
4. Small entrance, barely fit inside
5. Very warm, weatherproof

EMERGENCY SHELTERS:
Snow cave: Dig into snow bank,
  small entrance, vent hole
Tarp: Versatile, many configurations
Natural: Rock overhangs, fallen logs

BEDDING:
- Never sleep directly on ground
- 6+ inches of leaves/pine needles
- Keeps you insulated from cold

LOCATION TIPS:
Avoid: Valley bottoms (cold air)
       Hilltops (wind)
       Under dead trees
       Dry riverbeds
Choose: Slightly elevated
        Natural windbreak
        Near resources (water, wood)

EMERGENCY RULE:
Shelter before fire before water
Hypothermia kills faster than
dehydration!
"""

    articles.append({
        'title': 'Shelter Building',
        'content': shelter,
        'preview': 'Building emergency shelters'
    })

    print(f"✓ Created {len(articles)} survival articles")
    return articles

def create_religious_content():
    """Create religious texts content"""
    print("\n📖 Creating religious texts...")

    articles = []

    # Note: For actual religious texts, you'd want to download from Project Gutenberg
    # or similar public domain sources. Here I'll create introductions.

    bible_intro = """The Bible - Introduction

The Bible is the sacred text of
Christianity, consisting of the Old
and New Testaments.

STRUCTURE:
Old Testament (39 books):
- Torah/Pentateuch (5 books)
  Genesis, Exodus, Leviticus,
  Numbers, Deuteronomy
- Historical books (12 books)
- Wisdom literature (5 books)
- Prophets (17 books)

New Testament (27 books):
- Gospels (4 books)
  Matthew, Mark, Luke, John
- Acts of the Apostles
- Epistles/Letters (21 books)
- Revelation

KEY THEMES:
- Creation and fall of humanity
- God's covenant with humanity
- Redemption through faith
- Love, forgiveness, salvation
- Life and teachings of Jesus Christ

MAJOR BOOKS:
Genesis: Creation, early history
Exodus: Moses, Ten Commandments
Psalms: Poetry and songs
Proverbs: Wisdom sayings
Isaiah: Prophecy
Matthew: Life of Jesus
John: Jesus as divine Word
Romans: Christian theology
Revelation: End times prophecy

Note: This is an introduction. For
full text, refer to physical Bible
or authorized digital versions.
"""

    articles.append({
        'title': 'Bible Introduction',
        'content': bible_intro,
        'preview': 'Introduction to the Christian Bible'
    })

    quran_intro = """The Quran - Introduction

The Quran is the central religious
text of Islam, believed to be God's
revelation to Prophet Muhammad.

STRUCTURE:
114 chapters (Surahs)
Arranged roughly by length
(longest to shortest)

MAJOR SURAHS:
Al-Fatiha (The Opening): Chapter 1
Al-Baqarah (The Cow): Longest
Al-Ikhlas (Sincerity): Monotheism
Al-Nas (Mankind): Protection
An-Nisa (Women): Social laws

KEY THEMES:
- Tawhid (Oneness of God)
- Prophethood
- Day of Judgment
- Moral and ethical guidance
- Stories of previous prophets
- Social justice and laws

FIVE PILLARS OF ISLAM:
1. Shahada: Declaration of faith
2. Salah: Five daily prayers
3. Zakat: Charitable giving
4. Sawm: Fasting during Ramadan
5. Hajj: Pilgrimage to Mecca

IMPORTANT CONCEPTS:
- Submission to Allah's will
- Compassion and mercy
- Equality before God
- Accountability for actions
- Paradise and Hell

PROPHETS MENTIONED:
Adam, Noah, Abraham, Moses,
Jesus, Muhammad (peace be upon
them all)

Note: This is an introduction. For
full text in Arabic and translation,
refer to authorized sources.
"""

    articles.append({
        'title': 'Quran Introduction',
        'content': quran_intro,
        'preview': 'Introduction to the Islamic Quran'
    })

    torah_intro = """The Torah - Introduction

The Torah is the central text of
Judaism, comprising the first five
books of the Hebrew Bible.

THE FIVE BOOKS:
1. Genesis (Bereishit)
   Creation, patriarchs, Joseph
2. Exodus (Shemot)
   Moses, Egypt, Ten Commandments
3. Leviticus (Vayikra)
   Priestly laws, sacrifices
4. Numbers (Bamidbar)
   Wilderness wanderings, census
5. Deuteronomy (Devarim)
   Moses' final speeches, laws

KEY THEMES:
- Covenant between God and Israel
- God's laws and commandments
- Jewish identity and peoplehood
- Ethics and morality
- Relationship with God

TEN COMMANDMENTS:
1. One God
2. No idols
3. Don't take God's name in vain
4. Remember Sabbath
5. Honor parents
6. Don't murder
7. Don't commit adultery
8. Don't steal
9. Don't bear false witness
10. Don't covet

MAJOR FIGURES:
Abraham: Father of faith
Moses: Lawgiver and prophet
Aaron: First high priest
Miriam: Prophet and leader

JEWISH FESTIVALS (from Torah):
Passover: Exodus from Egypt
Shavuot: Giving of Torah
Sukkot: Wilderness wanderings
Rosh Hashanah: New Year
Yom Kippur: Day of Atonement

Note: This is an introduction. For
complete text, refer to Hebrew Bible
or authorized translations.
"""

    articles.append({
        'title': 'Torah Introduction',
        'content': torah_intro,
        'preview': 'Introduction to the Jewish Torah'
    })

    buddhism = """Buddhism - Core Teachings

Buddhism is a spiritual tradition
founded by Siddhartha Gautama
(the Buddha) in ancient India.

FOUR NOBLE TRUTHS:
1. Life involves suffering (dukkha)
2. Suffering arises from craving
3. Suffering can cease
4. Path to end suffering exists

EIGHTFOLD PATH:
Right Understanding
Right Intention
Right Speech
Right Action
Right Livelihood
Right Effort
Right Mindfulness
Right Concentration

THREE MARKS OF EXISTENCE:
1. Impermanence (anicca)
   All things change
2. Suffering (dukkha)
   Dissatisfaction in existence
3. Non-self (anatta)
   No permanent, unchanging self

FIVE PRECEPTS:
1. Don't kill
2. Don't steal
3. No sexual misconduct
4. Don't lie
5. No intoxicants

KEY CONCEPTS:
Karma: Actions have consequences
Samsara: Cycle of rebirth
Nirvana: Liberation from suffering
Meditation: Mental cultivation
Compassion: Love for all beings

MEDITATION PRACTICES:
Mindfulness (Vipassana)
Loving-kindness (Metta)
Concentration (Samatha)

BRANCHES:
Theravada: "School of Elders"
Mahayana: "Great Vehicle"
Vajrayana: "Diamond Vehicle"

The goal is enlightenment through
wisdom, ethical conduct, and
mental discipline.
"""

    articles.append({
        'title': 'Buddhism Basics',
        'content': buddhism,
        'preview': 'Core teachings of Buddhism'
    })

    print(f"✓ Created {len(articles)} religious texts")
    return articles

def save_articles(base_path, category, articles):
    """Save articles and create index"""
    category_path = base_path / category
    index_entries = []

    for article in articles:
        # Create safe filename
        filename = re.sub(r'[^\w\s-]', '', article['title'])
        filename = re.sub(r'[-\s]+', '_', filename)
        filename = filename.lower() + '.txt'

        # Wrap content
        wrapped_lines = wrap_text(article['content'])

        # Save article
        filepath = category_path / filename
        with open(filepath, 'w', encoding='utf-8') as f:
            f.write('\n'.join(wrapped_lines))

        # Add to index: KEYWORD|FILENAME|LINE|PREVIEW
        index_entry = f"{article['title']}|{filename}|0|{article['preview']}"
        index_entries.append(index_entry)

    # Save index
    index_path = category_path / 'index.txt'
    with open(index_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(index_entries))

    print(f"✓ Saved {len(articles)} articles to /{category}/")

def main():
    """Main function to generate The Book"""
    print("=" * 50)
    print("The Book Content Generator")
    print("=" * 50)

    # Create folder structure
    base_path = create_folder_structure()

    # Download Wikipedia articles
    wiki_articles = download_wikipedia_articles()
    if wiki_articles and len(wiki_articles) > 0:
        save_articles(base_path, "wikipedia", wiki_articles)
    else:
        print("⚠️  No Wikipedia articles downloaded (check network)")

    # Create tech content
    tech_articles = create_tech_content()
    save_articles(base_path, "tech", tech_articles)

    # Create survival content
    survival_articles = create_survival_content()
    save_articles(base_path, "survival", survival_articles)

    # Create religious content
    religious_articles = create_religious_content()
    save_articles(base_path, "religious", religious_articles)

    # Summary
    print("\n" + "=" * 50)
    print("✓ The Book generation complete!")
    print("=" * 50)
    print(f"Total articles: {len(wiki_articles) + len(tech_articles) + len(survival_articles) + len(religious_articles)}")
    print(f"\nLocation: {base_path.absolute()}")
    print("\nNext steps:")
    print("1. Copy the 'the_book' folder to your SD card")
    print("2. Insert SD card into M5Cardputer")
    print("3. Access 'The Book' from the main menu")
    print("=" * 50)

if __name__ == "__main__":
    main()
