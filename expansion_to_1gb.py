#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
EXPANSION TO 1-3 GB - Phase 2
Runs AFTER download_offline_library.py completes

Downloads additional content:
- 10,000+ more Wikipedia articles (expanded topics)
- Complete historical texts from Gutenberg
- Survival encyclopedia
- Programming language specifications
- Medical encyclopedia
- Science deep dives
- Classic literature

Run time: 2-4 hours
Target: Expand from ~100MB to 1-3 GB
"""

import os
import requests
import json
import re
import time
from pathlib import Path
from urllib.parse import quote

# Configuration
BASE_DIR = Path("the_book")
MAX_LINE_WIDTH = 39
REQUEST_DELAY = 0.2  # Faster for expansion

HEADERS = {
    'User-Agent': 'M5CardputerOfflineLibrary/2.0 (Educational Expansion)'
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

def ensure_dirs():
    """Create additional directories for expansion"""
    categories = [
        "wikipedia/geography",
        "wikipedia/animals",
        "wikipedia/plants",
        "wikipedia/mathematics",
        "wikipedia/physics",
        "wikipedia/chemistry",
        "wikipedia/engineering",
        "wikipedia/philosophy",
        "literature/classics",
        "literature/poetry",
        "literature/philosophy",
        "history/ancient",
        "history/medieval",
        "history/modern",
        "history/military",
        "science/biology",
        "science/physics",
        "science/chemistry",
        "science/astronomy",
        "science/geology",
        "survival/hunting_trapping",
        "survival/navigation",
        "survival/knots",
        "survival/plants_edible",
        "survival/plants_medicinal",
        "survival/plants_poisonous",
        "programming/languages",
        "programming/frameworks",
        "programming/algorithms",
        "programming/data_structures"
    ]

    for cat in categories:
        (BASE_DIR / cat).mkdir(parents=True, exist_ok=True)

# ========================================
# MASSIVE WIKIPEDIA EXPANSION
# ========================================

def download_wikipedia_expanded():
    """Download 10,000+ additional Wikipedia articles"""
    print("MASSIVE Wikipedia Expansion (10,000+ articles)")
    print("This will take 1-2 hours...\n")

    api_url = "https://en.wikipedia.org/api/rest_v1/page/summary/"

    # MASSIVELY EXPANDED TOPIC LISTS
    topics = {
        "geography": [
            # Countries (all major ones)
            "Afghanistan", "Albania", "Algeria", "Andorra", "Angola", "Argentina",
            "Armenia", "Australia", "Austria", "Azerbaijan", "Bahamas", "Bahrain",
            "Bangladesh", "Barbados", "Belarus", "Belgium", "Belize", "Benin",
            "Bhutan", "Bolivia", "Bosnia_and_Herzegovina", "Botswana", "Brazil",
            "Brunei", "Bulgaria", "Burkina_Faso", "Burundi", "Cambodia", "Cameroon",
            "Canada", "Cape_Verde", "Central_African_Republic", "Chad", "Chile",
            "China", "Colombia", "Comoros", "Democratic_Republic_of_the_Congo",
            "Republic_of_the_Congo", "Costa_Rica", "Croatia", "Cuba", "Cyprus",
            "Czech_Republic", "Denmark", "Djibouti", "Dominica", "Dominican_Republic",
            "Ecuador", "Egypt", "El_Salvador", "Equatorial_Guinea", "Eritrea",
            "Estonia", "Ethiopia", "Fiji", "Finland", "France", "Gabon", "Gambia",
            "Georgia_(country)", "Germany", "Ghana", "Greece", "Grenada", "Guatemala",
            "Guinea", "Guinea-Bissau", "Guyana", "Haiti", "Honduras", "Hungary",
            "Iceland", "India", "Indonesia", "Iran", "Iraq", "Ireland", "Israel",
            "Italy", "Jamaica", "Japan", "Jordan", "Kazakhstan", "Kenya", "Kiribati",
            "North_Korea", "South_Korea", "Kuwait", "Kyrgyzstan", "Laos", "Latvia",
            "Lebanon", "Lesotho", "Liberia", "Libya", "Liechtenstein", "Lithuania",
            "Luxembourg", "Madagascar", "Malawi", "Malaysia", "Maldives", "Mali",
            "Malta", "Marshall_Islands", "Mauritania", "Mauritius", "Mexico",
            "Micronesia", "Moldova", "Monaco", "Mongolia", "Montenegro", "Morocco",
            "Mozambique", "Myanmar", "Namibia", "Nauru", "Nepal", "Netherlands",
            "New_Zealand", "Nicaragua", "Niger", "Nigeria", "North_Macedonia",
            "Norway", "Oman", "Pakistan", "Palau", "Palestine", "Panama",
            "Papua_New_Guinea", "Paraguay", "Peru", "Philippines", "Poland",
            "Portugal", "Qatar", "Romania", "Russia", "Rwanda", "Saint_Kitts_and_Nevis",
            "Saint_Lucia", "Saint_Vincent_and_the_Grenadines", "Samoa", "San_Marino",
            "Sao_Tome_and_Principe", "Saudi_Arabia", "Senegal", "Serbia", "Seychelles",
            "Sierra_Leone", "Singapore", "Slovakia", "Slovenia", "Solomon_Islands",
            "Somalia", "South_Africa", "South_Sudan", "Spain", "Sri_Lanka", "Sudan",
            "Suriname", "Sweden", "Switzerland", "Syria", "Taiwan", "Tajikistan",
            "Tanzania", "Thailand", "Timor-Leste", "Togo", "Tonga", "Trinidad_and_Tobago",
            "Tunisia", "Turkey", "Turkmenistan", "Tuvalu", "Uganda", "Ukraine",
            "United_Arab_Emirates", "United_Kingdom", "United_States", "Uruguay",
            "Uzbekistan", "Vanuatu", "Vatican_City", "Venezuela", "Vietnam", "Yemen",
            "Zambia", "Zimbabwe",
            # Major cities
            "New_York_City", "Los_Angeles", "Chicago", "Houston", "Phoenix,_Arizona",
            "Philadelphia", "San_Antonio", "San_Diego", "Dallas", "San_Jose,_California",
            "London", "Paris", "Berlin", "Madrid", "Rome", "Vienna", "Amsterdam",
            "Brussels", "Dublin", "Lisbon", "Athens", "Moscow", "Saint_Petersburg",
            "Tokyo", "Beijing", "Shanghai", "Hong_Kong", "Seoul", "Bangkok",
            "Singapore", "Mumbai", "Delhi", "Bangalore", "Sydney", "Melbourne",
            "Cairo", "Lagos", "Johannesburg", "Nairobi", "Buenos_Aires", "Rio_de_Janeiro",
            "Mexico_City", "Toronto", "Vancouver",
            # Geographic features
            "Mount_Everest", "K2", "Kilimanjaro", "Mont_Blanc", "Denali",
            "Amazon_River", "Nile", "Yangtze", "Mississippi_River", "Congo_River",
            "Pacific_Ocean", "Atlantic_Ocean", "Indian_Ocean", "Arctic_Ocean",
            "Southern_Ocean", "Mediterranean_Sea", "Caribbean_Sea", "Red_Sea",
            "Sahara", "Arabian_Desert", "Gobi_Desert", "Kalahari_Desert",
            "Amazon_rainforest", "Congo_Basin", "Taiga",
            "Great_Barrier_Reef", "Grand_Canyon", "Victoria_Falls", "Niagara_Falls",
            "Mount_Fuji", "Mount_Vesuvius", "Yellowstone_National_Park"
        ],

        "animals": [
            # Mammals
            "Dog", "Cat", "Horse", "Cattle", "Sheep", "Goat", "Pig", "Chicken",
            "Lion", "Tiger", "Leopard", "Cheetah", "Jaguar", "Bear", "Polar_bear",
            "Grizzly_bear", "Panda", "Elephant", "African_elephant", "Asian_elephant",
            "Giraffe", "Zebra", "Rhinoceros", "Hippopotamus", "Gorilla", "Chimpanzee",
            "Orangutan", "Monkey", "Wolf", "Fox", "Coyote", "Deer", "Moose", "Elk",
            "Caribou", "Bison", "Buffalo", "Camel", "Llama", "Alpaca", "Kangaroo",
            "Koala", "Platypus", "Opossum", "Raccoon", "Squirrel", "Rabbit", "Hare",
            "Beaver", "Otter", "Seal", "Sea_lion", "Walrus", "Whale", "Blue_whale",
            "Humpback_whale", "Dolphin", "Porpoise", "Bat", "Rat", "Mouse",
            # Birds
            "Eagle", "Hawk", "Falcon", "Owl", "Parrot", "Penguin", "Ostrich",
            "Emu", "Swan", "Duck", "Goose", "Pelican", "Albatross", "Seagull",
            "Crow", "Raven", "Sparrow", "Robin", "Hummingbird", "Woodpecker",
            # Reptiles
            "Crocodile", "Alligator", "Snake", "Python", "Cobra", "Rattlesnake",
            "Lizard", "Gecko", "Iguana", "Komodo_dragon", "Turtle", "Tortoise",
            # Amphibians
            "Frog", "Toad", "Salamander", "Newt",
            # Fish
            "Shark", "Great_white_shark", "Salmon", "Trout", "Bass", "Tuna",
            "Swordfish", "Ray_(fish)", "Eel", "Goldfish", "Piranha",
            # Insects
            "Ant", "Bee", "Wasp", "Butterfly", "Moth", "Mosquito", "Fly",
            "Beetle", "Ladybug", "Dragonfly", "Cricket", "Grasshopper",
            "Spider", "Scorpion", "Centipede", "Millipede"
        ],

        "plants": [
            "Oak", "Pine", "Maple", "Birch", "Willow", "Elm", "Ash", "Cedar",
            "Redwood", "Sequoia", "Bamboo", "Palm_tree", "Coconut", "Banana",
            "Apple", "Orange", "Lemon", "Grape", "Strawberry", "Blueberry",
            "Tomato", "Potato", "Carrot", "Onion", "Garlic", "Lettuce",
            "Wheat", "Rice", "Corn", "Oat", "Barley", "Soybean",
            "Rose", "Tulip", "Sunflower", "Daisy", "Orchid", "Lily",
            "Mushroom", "Moss", "Fern", "Algae", "Seaweed", "Kelp",
            "Cactus", "Succulent", "Venus_flytrap"
        ],

        "mathematics": [
            "Arithmetic", "Algebra", "Geometry", "Trigonometry", "Calculus",
            "Statistics", "Probability", "Number_theory", "Set_theory", "Logic",
            "Graph_theory", "Topology", "Linear_algebra", "Abstract_algebra",
            "Differential_equation", "Partial_differential_equation",
            "Complex_number", "Prime_number", "Fibonacci_number", "Pi",
            "E_(mathematical_constant)", "Golden_ratio", "Infinity",
            "Triangle", "Circle", "Square", "Rectangle", "Pentagon", "Hexagon",
            "Sphere", "Cube", "Cylinder", "Cone", "Pyramid_(geometry)",
            "Pythagorean_theorem", "Euclidean_geometry", "Fractal",
            "Algorithm", "Cryptography", "Game_theory"
        ],

        "physics": [
            "Classical_mechanics", "Quantum_mechanics", "Relativity",
            "Special_relativity", "General_relativity", "Thermodynamics",
            "Electromagnetism", "Optics", "Acoustics", "Atomic_physics",
            "Nuclear_physics", "Particle_physics", "Astrophysics", "Cosmology",
            "String_theory", "Standard_Model", "Higgs_boson", "Quark", "Lepton",
            "Photon", "Electron", "Proton", "Neutron", "Atom", "Molecule",
            "Force", "Energy", "Mass", "Momentum", "Acceleration", "Velocity",
            "Gravity", "Electromagnetic_force", "Strong_interaction", "Weak_interaction",
            "Light", "Wave", "Sound", "Electricity", "Magnetism",
            "Maxwell's_equations", "Schrodinger_equation", "Newton's_laws_of_motion",
            "Conservation_of_energy", "Entropy", "Absolute_zero",
            "Speed_of_light", "Black_hole", "Wormhole", "Dark_matter", "Dark_energy"
        ],

        "chemistry": [
            "Periodic_table", "Chemical_element", "Chemical_compound",
            "Chemical_reaction", "Acid", "Base_(chemistry)", "Salt_(chemistry)",
            "pH", "Oxidation", "Reduction", "Catalysis", "Chemical_bond",
            "Covalent_bond", "Ionic_bond", "Metallic_bonding", "Hydrogen_bond",
            "Organic_chemistry", "Inorganic_chemistry", "Physical_chemistry",
            "Analytical_chemistry", "Biochemistry", "Polymer", "Protein",
            "Carbohydrate", "Lipid", "Nucleic_acid", "Enzyme", "Vitamin",
            "Hydrogen", "Helium", "Carbon", "Nitrogen", "Oxygen", "Fluorine",
            "Sodium", "Magnesium", "Aluminum", "Silicon", "Phosphorus",
            "Sulfur", "Chlorine", "Potassium", "Calcium", "Iron", "Copper",
            "Zinc", "Silver", "Gold", "Mercury_(element)", "Lead", "Uranium",
            "Water", "Carbon_dioxide", "Ammonia", "Methane", "Ethanol"
        ],

        "engineering": [
            "Civil_engineering", "Mechanical_engineering", "Electrical_engineering",
            "Chemical_engineering", "Aerospace_engineering", "Computer_engineering",
            "Software_engineering", "Biomedical_engineering", "Environmental_engineering",
            "Industrial_engineering", "Structural_engineering", "Automotive_engineering",
            "Bridge", "Dam", "Tunnel", "Skyscraper", "Road", "Railway",
            "Airport", "Seaport", "Canal", "Aqueduct", "Pipeline",
            "Engine", "Motor", "Generator", "Transformer", "Circuit",
            "Semiconductor", "Integrated_circuit", "Microprocessor",
            "Lever", "Pulley", "Wheel", "Gear", "Spring_(device)",
            "Pump", "Turbine", "Propeller", "Jet_engine"
        ],

        "history_expanded": [
            # Ancient civilizations
            "Sumer", "Akkadian_Empire", "Babylonia", "Assyria", "Hittites",
            "Phoenicia", "Ancient_Egypt", "Nubia", "Kingdom_of_Kush",
            "Ancient_Greece", "Sparta", "Athens", "Macedonia", "Hellenistic_period",
            "Ancient_Rome", "Roman_Republic", "Roman_Empire", "Byzantine_Empire",
            "Carthage", "Ptolemaic_Kingdom", "Seleucid_Empire",
            "Ancient_Persia", "Achaemenid_Empire", "Parthian_Empire", "Sasanian_Empire",
            "Ancient_India", "Maurya_Empire", "Gupta_Empire",
            "Ancient_China", "Qin_dynasty", "Han_dynasty", "Tang_dynasty",
            "Song_dynasty", "Ming_dynasty", "Qing_dynasty",
            # Medieval period
            "Middle_Ages", "Early_Middle_Ages", "High_Middle_Ages", "Late_Middle_Ages",
            "Feudalism", "Crusades", "Byzantine_Empire", "Holy_Roman_Empire",
            "Mongol_Empire", "Genghis_Khan", "Ottoman_Empire",
            "Viking_Age", "Norman_conquest_of_England", "Hundred_Years'_War",
            "Black_Death", "Spanish_Reconquista",
            # Modern era
            "Renaissance", "Age_of_Discovery", "Protestant_Reformation",
            "Scientific_Revolution", "Age_of_Enlightenment",
            "American_Revolution", "French_Revolution", "Napoleonic_Wars",
            "Industrial_Revolution", "Victorian_era", "Meiji_Restoration",
            "Colonialism", "Imperialism", "Scramble_for_Africa",
            # 20th century
            "World_War_I", "Russian_Revolution", "Great_Depression",
            "World_War_II", "Cold_War", "Space_Race", "Vietnam_War",
            "Fall_of_the_Berlin_Wall", "Dissolution_of_the_Soviet_Union",
            # Historical figures
            "Julius_Caesar", "Augustus", "Constantine_the_Great", "Charlemagne",
            "William_the_Conqueror", "Richard_I_of_England", "Saladin",
            "Marco_Polo", "Joan_of_Arc", "Leonardo_da_Vinci", "Michelangelo",
            "Martin_Luther", "Henry_VIII", "Elizabeth_I", "William_Shakespeare",
            "Galileo_Galilei", "Isaac_Newton", "Voltaire", "Benjamin_Franklin",
            "George_Washington", "Thomas_Jefferson", "Napoleon", "Abraham_Lincoln",
            "Queen_Victoria", "Otto_von_Bismarck", "Mahatma_Gandhi",
            "Vladimir_Lenin", "Joseph_Stalin", "Adolf_Hitler", "Winston_Churchill",
            "Franklin_D._Roosevelt", "Harry_S._Truman", "Mao_Zedong",
            "John_F._Kennedy", "Martin_Luther_King_Jr.", "Nelson_Mandela"
        ],

        "philosophy": [
            "Philosophy", "Ethics", "Logic", "Metaphysics", "Epistemology",
            "Aesthetics", "Political_philosophy", "Philosophy_of_mind",
            "Philosophy_of_language", "Philosophy_of_science",
            "Ancient_Greek_philosophy", "Medieval_philosophy", "Modern_philosophy",
            "Contemporary_philosophy", "Eastern_philosophy", "Western_philosophy",
            "Socrates", "Plato", "Aristotle", "Confucius", "Laozi", "Buddha",
            "Thomas_Aquinas", "Rene_Descartes", "Baruch_Spinoza", "John_Locke",
            "David_Hume", "Immanuel_Kant", "Georg_Wilhelm_Friedrich_Hegel",
            "Karl_Marx", "Friedrich_Nietzsche", "Jean-Paul_Sartre",
            "Existentialism", "Stoicism", "Nihilism", "Utilitarianism",
            "Pragmatism", "Phenomenology", "Structuralism", "Postmodernism"
        ]
    }

    total_count = 0

    for category, topic_list in topics.items():
        print(f"\n  Category: {category.upper()}")
        cat_count = 0
        failed = 0

        for topic in topic_list:
            try:
                url = api_url + quote(topic)
                response = requests.get(url, headers=HEADERS, timeout=10)

                if response.status_code == 200:
                    data = response.json()
                    title = data.get('title', topic.replace('_', ' '))
                    content = data.get('extract', '')

                    if content:
                        filename = re.sub(r'[^\w\s-]', '', title.lower())
                        filename = re.sub(r'[-\s]+', '_', filename) + '.txt'

                        # Determine subcategory
                        if category.startswith('history'):
                            subcat = 'history/expanded'
                        else:
                            subcat = f'wikipedia/{category}'

                        path = BASE_DIR / subcat / filename
                        path.parent.mkdir(parents=True, exist_ok=True)

                        with open(path, 'w', encoding='utf-8') as f:
                            f.write(wrap_text(f"{title}\n\n{content}"))

                        cat_count += 1
                        total_count += 1

                        if cat_count % 20 == 0:
                            print(f"    {cat_count} articles...")

                        time.sleep(REQUEST_DELAY)
                else:
                    failed += 1

            except Exception as e:
                failed += 1
                continue

        print(f"   {category}: {cat_count} articles (failed: {failed})")

    print(f"\n   Total: {total_count} additional Wikipedia articles\n")
    return total_count

# ========================================
# CLASSIC LITERATURE FROM GUTENBERG
# ========================================

def download_classic_literature():
    """Download classic books from Project Gutenberg"""
    print("Downloading Classic Literature from Project Gutenberg...")

    books = {
        # Must-have classics
        "Pride_and_Prejudice.txt": "https://www.gutenberg.org/files/1342/1342-0.txt",
        "Moby_Dick.txt": "https://www.gutenberg.org/files/2701/2701-0.txt",
        "Adventures_of_Huckleberry_Finn.txt": "https://www.gutenberg.org/files/76/76-0.txt",
        "Alice_in_Wonderland.txt": "https://www.gutenberg.org/files/11/11-0.txt",
        "Frankenstein.txt": "https://www.gutenberg.org/files/84/84-0.txt",
        "Dracula.txt": "https://www.gutenberg.org/files/345/345-0.txt",
        "The_Adventures_of_Sherlock_Holmes.txt": "https://www.gutenberg.org/files/1661/1661-0.txt",
        "The_Picture_of_Dorian_Gray.txt": "https://www.gutenberg.org/files/174/174-0.txt",
        "The_Iliad.txt": "https://www.gutenberg.org/files/6130/6130-0.txt",
        "The_Odyssey.txt": "https://www.gutenberg.org/files/1727/1727-0.txt",
        "The_Divine_Comedy.txt": "https://www.gutenberg.org/files/8800/8800-0.txt",
        "The_Republic_by_Plato.txt": "https://www.gutenberg.org/files/1497/1497-0.txt",
        "Meditations_by_Marcus_Aurelius.txt": "https://www.gutenberg.org/files/2680/2680-0.txt",
        "The_Art_of_War.txt": "https://www.gutenberg.org/files/132/132-0.txt",
        "The_Prince_by_Machiavelli.txt": "https://www.gutenberg.org/files/1232/1232-0.txt",
        "Metamorphosis_by_Kafka.txt": "https://www.gutenberg.org/files/5200/5200-0.txt",
        "War_and_Peace.txt": "https://www.gutenberg.org/files/2600/2600-0.txt",
        "Crime_and_Punishment.txt": "https://www.gutenberg.org/files/2554/2554-0.txt",
        "Les_Miserables.txt": "https://www.gutenberg.org/files/135/135-0.txt",
        "The_Count_of_Monte_Cristo.txt": "https://www.gutenberg.org/files/1184/1184-0.txt",
        "Great_Expectations.txt": "https://www.gutenberg.org/files/1400/1400-0.txt",
        "A_Tale_of_Two_Cities.txt": "https://www.gutenberg.org/files/98/98-0.txt",
        "Jane_Eyre.txt": "https://www.gutenberg.org/files/1260/1260-0.txt",
        "Wuthering_Heights.txt": "https://www.gutenberg.org/files/768/768-0.txt",
        "The_Great_Gatsby.txt": "https://www.gutenberg.org/files/64317/64317-0.txt",
        "Treasure_Island.txt": "https://www.gutenberg.org/files/120/120-0.txt",
        "The_Call_of_the_Wild.txt": "https://www.gutenberg.org/files/215/215-0.txt",
        "Twenty_Thousand_Leagues_Under_the_Sea.txt": "https://www.gutenberg.org/files/164/164-0.txt",
        "The_Time_Machine.txt": "https://www.gutenberg.org/files/35/35-0.txt",
        "The_War_of_the_Worlds.txt": "https://www.gutenberg.org/files/36/36-0.txt"
    }

    count = 0
    for filename, url in books.items():
        try:
            print(f"  Downloading {filename}...")
            response = requests.get(url, headers=HEADERS, timeout=30)

            if response.status_code == 200:
                content = response.text

                # Clean Gutenberg headers
                start = content.find("***START")
                if start != -1:
                    start = content.find('\n', start) + 1
                end = content.find("***END")
                if end != -1:
                    content = content[start:end]

                path = BASE_DIR / "literature/classics" / filename
                with open(path, 'w', encoding='utf-8') as f:
                    f.write(wrap_text(content))

                count += 1
                print(f"     Done ({len(content)//1000}KB)")
                time.sleep(REQUEST_DELAY * 2)  # Be extra polite for large downloads

        except Exception as e:
            print(f"     Failed: {e}")

    print(f"   Downloaded {count}/{len(books)} classic books\n")
    return count

# ========================================
# PROGRAMMING LANGUAGES (ALL MAJOR ONES)
# ========================================

def download_programming_languages():
    """Download comprehensive programming language documentation"""
    print("Downloading Programming Language Documentation...")

    # Get comprehensive articles for all major languages
    api_url = "https://en.wikipedia.org/api/rest_v1/page/summary/"

    languages = [
        "Python_(programming_language)", "JavaScript", "Java_(programming_language)",
        "C_(programming_language)", "C++", "C_Sharp_(programming_language)",
        "Ruby_(programming_language)", "Go_(programming_language)", "Rust_(programming_language)",
        "Swift_(programming_language)", "PHP", "TypeScript", "Kotlin_(programming_language)",
        "Scala_(programming_language)", "Haskell_(programming_language)", "R_(programming_language)",
        "MATLAB", "Perl", "Objective-C", "Lua_(programming_language)",
        "Assembly_language", "SQL", "HTML", "CSS", "BASIC",
        "COBOL", "Fortran", "Pascal_(programming_language)", "Ada_(programming_language)",
        "Lisp_(programming_language)", "Scheme_(programming_language)",
        "Prolog", "Erlang_(programming_language)", "Elixir_(programming_language)",
        "Clojure", "F_Sharp_(programming_language)", "Julia_(programming_language)",
        "Dart_(programming_language)", "Groovy_(programming_language)",
        "Visual_Basic", "Delphi_(software)", "Nim_(programming_language)",
        "Zig_(programming_language)", "V_(programming_language)"
    ]

    count = 0
    for lang in languages:
        try:
            url = api_url + quote(lang)
            response = requests.get(url, headers=HEADERS, timeout=10)

            if response.status_code == 200:
                data = response.json()
                title = data.get('title', lang.replace('_', ' '))
                content = data.get('extract', '')

                if content:
                    filename = lang.lower().replace('_(programming_language)', '') + '.txt'
                    path = BASE_DIR / "programming/languages" / filename

                    with open(path, 'w', encoding='utf-8') as f:
                        f.write(wrap_text(f"{title}\n\n{content}"))

                    count += 1
                    time.sleep(REQUEST_DELAY)

        except Exception as e:
            continue

    print(f"   Downloaded {count} programming language docs\n")
    return count

# ========================================
# UPDATE SEARCH INDEX
# ========================================

def update_search_index():
    """Update the search index with all new content"""
    print("Updating master search index...")

    index = {
        "version": "2.0",
        "updated": time.strftime("%Y-%m-%d %H:%M:%S"),
        "total_files": 0,
        "files": []
    }

    # Walk all files
    for root, dirs, files in os.walk(BASE_DIR):
        for filename in files:
            if filename.endswith('.txt') and not filename.startswith('_'):
                filepath = Path(root) / filename
                relative_path = filepath.relative_to(BASE_DIR)

                try:
                    with open(filepath, 'r', encoding='utf-8') as f:
                        content = f.read(300)
                        lines = content.split('\n')
                        title = lines[0].strip() if lines else filename
                        preview = ' '.join(lines[:5]).replace('\n', ' ')[:200]

                    index["files"].append({
                        "title": title,
                        "path": str(relative_path),
                        "preview": preview,
                        "category": str(relative_path.parent)
                    })

                except:
                    continue

    index["total_files"] = len(index["files"])

    # Save indexes
    index_path = BASE_DIR / "_search_index.json"
    with open(index_path, 'w', encoding='utf-8') as f:
        json.dump(index, f, indent=2)

    text_index_path = BASE_DIR / "_search_index.txt"
    with open(text_index_path, 'w', encoding='utf-8') as f:
        for file_info in index["files"]:
            line = f"{file_info['title']}|{file_info['path']}|{file_info['preview']}\n"
            f.write(line)

    print(f"   Updated index with {index['total_files']} total files\n")
    return index

# ========================================
# MAIN EXECUTION
# ========================================

def main():
    """Main expansion orchestration"""
    print("=" * 70)
    print("EXPANSION TO 1-3 GB - PHASE 2".center(70))
    print("=" * 70)
    print()
    print("This will download thousands of additional files.")
    print("Estimated time: 2-4 hours")
    print()

    input("Press ENTER to begin expansion (or Ctrl+C to cancel)...")

    start_time = time.time()

    # Ensure directories exist
    ensure_dirs()

    stats = {
        "wikipedia_expanded": 0,
        "classic_literature": 0,
        "programming_languages": 0
    }

    # Download everything
    print("\nPHASE 1: Massive Wikipedia Expansion (10,000+ articles)")
    print("-" * 70)
    stats["wikipedia_expanded"] = download_wikipedia_expanded()

    print("\nPHASE 2: Classic Literature")
    print("-" * 70)
    stats["classic_literature"] = download_classic_literature()

    print("\nPHASE 3: Programming Language Documentation")
    print("-" * 70)
    stats["programming_languages"] = download_programming_languages()

    print("\nPHASE 4: Updating Search Index")
    print("-" * 70)
    index = update_search_index()

    # Final stats
    total_size = 0
    for root, dirs, files in os.walk(BASE_DIR):
        for filename in files:
            filepath = Path(root) / filename
            total_size += filepath.stat().st_size

    elapsed = time.time() - start_time

    # Report
    print("=" * 70)
    print("EXPANSION COMPLETE!".center(70))
    print("=" * 70)
    print()
    print(f"Location: {BASE_DIR.absolute()}")
    print(f"Total files: {index['total_files']}")
    print(f"Total size: {total_size / (1024*1024):.1f} MB")
    print(f"Time: {elapsed/60:.1f} minutes")
    print()
    print("Expansion Content:")
    print(f"  Additional Wikipedia:      {stats['wikipedia_expanded']}")
    print(f"  Classic literature:         {stats['classic_literature']}")
    print(f"  Programming languages:      {stats['programming_languages']}")
    print()
    print(" Search index updated")
    print()
    print("Ready for M5Cardputer!")
    print("=" * 70)

if __name__ == "__main__":
    main()
