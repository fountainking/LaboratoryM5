#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
DREAM DICTIONARY GENERATOR
Adds comprehensive dream symbol dictionary to The Book
Inspired by DreamMoods.com style interpretation

Run after main download completes
Creates 100+ dream symbol files with meanings
"""

from pathlib import Path

BASE_DIR = Path("the_book")
MAX_LINE_WIDTH = 39

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
print("DREAM DICTIONARY GENERATOR")
print("=" * 70)
print()

# Create directory
dream_dir = BASE_DIR / "reference/dreams"
dream_dir.mkdir(parents=True, exist_ok=True)

# Comprehensive dream symbols
dream_symbols = {
    "Animals": {
        "Dog": "Loyalty, friendship, protection. Dogs represent faithful companions and unconditional love. May indicate trust issues or need for companionship. Aggressive dog = inner conflict.",
        "Cat": "Independence, intuition, feminine power. Cats symbolize mystery and spiritual awareness. May suggest need for independence or warning about deceit. Multiple cats = creativity.",
        "Snake": "Transformation, healing, hidden fears. Snakes represent change and renewal. Shedding skin = personal growth. Biting snake = toxic situation. Dead snake = overcoming fear.",
        "Bird": "Freedom, spirituality, aspirations. Birds represent transcendence and liberation. Flying = success. Caged = restriction. Dead bird = lost hopes. Singing = joy coming.",
        "Horse": "Power, freedom, drive. Horses symbolize raw energy and passion. Riding = control. Wild horse = untamed emotions. White horse = spiritual awareness.",
        "Spider": "Creativity, patience, feminine energy. Spiders weave fate and destiny. Web = feeling trapped or creating reality. Bite = creative block or toxic relationship.",
        "Bear": "Strength, introspection, protection. Bears represent primal power and motherhood. Hibernating = need for rest. Attacking = repressed anger. Cubs = nurturing needed.",
        "Lion": "Courage, leadership, pride. Lions symbolize personal power and authority. Roaring = speaking truth. Tamed = controlled power. Pride of lions = community strength.",
        "Fish": "Emotions, unconscious thoughts, spiritual insights. Fish swim in emotional waters. Many fish = abundance. Dead fish = emotional numbness. Catching = understanding feelings.",
        "Butterfly": "Transformation, rebirth, beauty. Butterflies signify metamorphosis and change. Cocoon = transformation in progress. Landing on you = blessing. Multiple = social connections.",
        "Wolf": "Instinct, intelligence, appetite for freedom. Wolves represent shadow self. Pack = family/community. Lone wolf = independence. Howling = calling for help.",
        "Elephant": "Memory, wisdom, strength. Elephants never forget. Trunk up = good luck. Herd = family bonds. Baby elephant = new beginning with wisdom.",
        "Owl": "Wisdom, intuition, transition. Owls see in darkness. Flying = expanding awareness. Dead = lost wisdom. Hooting = pay attention to signs.",
        "Rabbit": "Fear, fertility, luck. Rabbits multiply quickly. Running = anxiety. White rabbit = following curiosity. Multiple = creative abundance.",
        "Crow": "Magic, mystery, intelligence. Ravens are messengers. Flying = spiritual message. Dead = transformation. Group (murder) = major change coming."
    },

    "People": {
        "Mother": "Nurturing, comfort, origins. Your mother represents relationship with feminine energy and care. Angry mother = guilt. Deceased mother visiting = guidance.",
        "Father": "Authority, protection, structure. Father symbolizes rules and guidance. Angry father = authority issues. Absent father = need for guidance.",
        "Baby": "New beginnings, vulnerability, potential. Babies represent pure potential and new starts. Crying baby = neglected project. Sick baby = vulnerable new venture.",
        "Ex_Partner": "Unresolved feelings, past lessons, nostalgia. Ex represents unfinished emotional business. Not desire to reconnect - unresolved issues need attention.",
        "Stranger": "Unknown aspects of self, opportunities. Strangers are parts of yourself unrecognized. Friendly = new opportunities. Threatening = rejected aspects.",
        "Dead_Person": "Guidance, unresolved grief, change. Deceased visiting bring messages. Speaking = listen to advice. Alive again = unfinished business.",
        "Pregnant_Woman": "Creativity, potential, growth. Pregnancy symbolizes something developing. You pregnant = new ideas gestating. Someone else = witnessing creation.",
        "Child": "Inner child, innocence, potential. Children represent your younger self. Happy child = healed wounds. Hurt child = childhood trauma needs attention.",
        "Teacher": "Wisdom, guidance, lessons. Teachers bring knowledge. Strict teacher = harsh self-judgment. Kind teacher = accepting guidance.",
        "Doctor": "Healing, health concerns, care. Doctors symbolize need for healing. Surgery = deep change needed. Diagnosis = recognizing issue.",
        "Police": "Authority, rules, conscience. Police represent internal moral code. Arrest = feeling guilt. Chase = avoiding consequences.",
        "Celebrity": "Aspirations, projection, desired qualities. Celebrities embody traits you admire or want. Meeting = recognizing those qualities in self.",
        "Friend": "Aspects of self, support, relationship quality. Friends represent parts of you. Fighting = internal conflict. Lost friend = lost part of self.",
        "Enemy": "Shadow self, projection, internal conflict. Enemies are disowned parts of self. Fighting = integrating shadow. Befriending = accepting all parts.",
        "Crowd": "Social pressure, conformity, community. Crowds represent collective influence. Lost in crowd = identity crisis. Leading crowd = confidence."
    },

    "Places": {
        "House": "Self, psyche, comfort zone. Rooms = aspects of self. Basement = unconscious. Attic = higher consciousness. Kitchen = nourishment. Bedroom = intimacy. Bathroom = cleansing.",
        "School": "Lessons, testing, social anxiety. School represents life lessons. Lost in school = feeling unprepared. Naked at school = vulnerability. Passing test = overcoming challenge.",
        "Water": "Emotions, unconscious, spirituality. Calm water = peace. Turbulent = turmoil. Deep = unconscious. Drowning = overwhelmed. Swimming = navigating emotions.",
        "Flying": "Freedom, perspective, transcendence. Flying = liberation from limitations. High flying = success. Difficulty flying = obstacles. Falling while flying = loss of control.",
        "Falling": "Loss of control, anxiety, insecurity. Falling = feeling overwhelmed. May reflect work stress or relationship instability. Landing safely = resilience.",
        "Forest": "Unconscious, mystery, growth. Forests represent unknown territory. Dark forest = confusion. Light forest = exploration. Lost = searching for direction.",
        "Beach": "Boundary between conscious/unconscious, relaxation. Beaches where land meets sea (conscious meets unconscious). Walking = exploring feelings. Swimming = diving deep.",
        "Mountain": "Challenges, goals, achievement. Mountains = obstacles to overcome. Climbing = striving toward goals. Summit = success. Steep = difficult challenge.",
        "Desert": "Emptiness, spiritual quest, isolation. Deserts symbolize barren periods. May indicate feeling emotionally dry. Oasis = finding sustenance.",
        "City": "Community, complexity, opportunities. Cities represent social aspects. Lost in city = overwhelmed by options. Familiar city = comfort zone.",
        "Cave": "Unconscious, shelter, hidden knowledge. Caves are womb-like spaces. Dark cave = exploring unconscious. Treasure in cave = hidden talents.",
        "Bridge": "Transition, connection, passage. Bridges connect two states. Crossing = major transition. Collapsing bridge = transition anxiety. Under bridge = avoiding change.",
        "Road": "Life path, journey, direction. Roads = life direction. Straight = clear path. Winding = uncertainty. Dead end = feeling stuck. Fork = decision point.",
        "Hospital": "Healing, vulnerability, transformation. Hospitals = need for healing. Patient = receiving care. Doctor = helping others heal. Surgery = deep transformation.",
        "Church": "Spirituality, morality, seeking guidance. Churches = spiritual needs. Empty church = feeling disconnected. Full church = community support.",
        "Cemetery": "Endings, grief, letting go. Cemeteries = what has died. Visiting grave = processing loss. Peaceful cemetery = acceptance.",
        "Prison": "Restriction, guilt, punishment. Prisons = feeling trapped. Prisoner = self-imposed limits. Escaping = breaking free. Guard = harsh inner critic.",
        "Store": "Choices, resources, values. Stores = what's available to you. Shopping = seeking fulfillment. Empty shelves = feeling lack. Too many choices = overwhelmed.",
        "Restaurant": "Nourishment, social connection, choices. Restaurants = how you feed yourself emotionally. Menu = life choices. Alone = self-sufficiency. Group = community.",
        "Garden": "Growth, cultivation, potential. Gardens = what you're growing. Blooming = success. Weeds = neglect. Planting = new projects. Harvesting = reaping rewards."
    },

    "Actions": {
        "Dying": "Endings, transformation, rebirth. Death rarely means physical death. Represents end of phase and new beginning. Who dies matters.",
        "Being_Chased": "Avoidance, fear, running from issues. Being chased = running from something in waking life. Face the chaser to resolve. What chases you reveals what you avoid.",
        "Naked_in_Public": "Vulnerability, exposure, authenticity. Nakedness = feeling exposed. Fear of judgment. May indicate imposter syndrome or desire for authenticity.",
        "Teeth_Falling_Out": "Anxiety, powerlessness, communication issues. Teeth falling = loss of control or fear of aging. May indicate feeling powerless in situation.",
        "Unable_to_Scream": "Powerlessness, suppressed emotions, voicelessness. Can't speak = feeling unheard in waking life. Throat issues = communication blocks.",
        "Taking_Test": "Being judged, fear of failure, self-evaluation. Tests = feeling evaluated or unprepared. Unprepared = imposter syndrome. Passing = confidence.",
        "Missing_Transportation": "Missed opportunities, lack of control, timing issues. Missing bus/train = feeling life passing by. Running late = time pressure.",
        "Finding_Rooms": "Discovering potential, new aspects of self. New rooms in familiar house = discovering hidden talents or aspects of personality.",
        "Drowning": "Overwhelm, emotional flooding, loss of control. Drowning = being overwhelmed by emotions or situations. Rescue = help available.",
        "Running": "Escape, pursuit of goals, urgency. Running from = avoidance. Running toward = pursuing goals. Unable to run = feeling stuck.",
        "Swimming": "Navigating emotions, going with flow, immersion. Swimming = emotional navigation. Easy swimming = handling feelings well. Struggling = emotional difficulty.",
        "Eating": "Nourishment, consumption, taking in. What you eat matters. Junk food = empty experiences. Healthy food = good choices. Overeating = excess.",
        "Fighting": "Internal conflict, standing up for self, aggression. Fighting = inner or outer conflicts. Winning = overcoming. Losing = giving up power.",
        "Kissing": "Connection, intimacy, approval. Who you kiss matters. Stranger = accepting unknown. Wrong person = misplaced affection. Passionate = desire.",
        "Crying": "Release, grief, emotional cleansing. Crying in dreams = emotional release. Healthy processing. Someone else crying = empathy or projection.",
        "Laughing": "Joy, release, coping mechanism. Laughing = processing emotions. Uncontrollable laughter = hysteria or defense. Happy laughter = joy.",
        "Dancing": "Expression, freedom, celebration. Dancing = expressing yourself. Graceful = confidence. Awkward = self-consciousness. Partner dance = relationship harmony.",
        "Singing": "Expression, joy, communication. Singing = expressing feelings. Beautiful voice = confident expression. Can't sing = feeling voiceless.",
        "Hiding": "Avoidance, shame, protection. Hiding = avoiding confrontation. Can't find hiding spot = nowhere to run. Found = secrets revealed.",
        "Searching": "Seeking answers, lost aspects, quest. Searching = looking for meaning. Lost item = lost part of self. Finding = discovering truth."
    },

    "Objects": {
        "Car": "Life direction, control, personal drive. Driver's seat = control of life. Passenger = not in control. Accident = feeling out of control. Broken car = stalled progress.",
        "Phone": "Communication, connection, messages. Phone problems = communication issues. Can't dial = can't express self. Ringing = message coming. Dead phone = disconnection.",
        "Money": "Self-worth, power, resources. Finding money = discovering talents. Losing money = feeling undervalued. Stealing = taking what's not yours. Giving = generosity.",
        "Keys": "Access, solutions, opportunities. Keys unlock doors. Finding keys = discovering solution. Lost keys = feeling locked out. Many keys = many options.",
        "Mirror": "Self-reflection, truth, perception. How you appear = self-perception. Broken mirror = fractured self-image. Can't see reflection = identity crisis.",
        "Clock": "Time pressure, deadlines, mortality. Clocks = time concerns. Broken clock = timelessness. Running out of time = deadline pressure. Stopped clock = suspended moment.",
        "Book": "Knowledge, learning, records. Books = wisdom. Reading = learning. Writing = creating knowledge. Blank book = potential. Ancient book = old wisdom.",
        "Food": "Nourishment, abundance, satisfaction. Type of food matters. Spoiled = toxic situation. Abundant = prosperity. Hungry = unmet needs.",
        "Weapon": "Defense, aggression, power. Weapons = how you protect yourself. Gun = direct aggression. Knife = cutting away. Broken weapon = powerlessness.",
        "Door": "Opportunities, transitions, choices. Open door = opportunity. Closed door = blocked path. Locked door = obstacles. Many doors = overwhelming choices.",
        "Window": "Perspective, clarity, opportunities. Looking through = observing life. Broken window = shattered illusions. Closed window = closed off.",
        "Stairs": "Progress, ascension/descension, effort. Going up = advancement. Going down = regression or exploring unconscious. Spiral = cyclical progress.",
        "Bag": "Burdens, baggage, preparation. What you carry = emotional baggage or being prepared. Heavy bag = burdens. Empty bag = freedom.",
        "Jewelry": "Value, self-worth, gifts. Jewelry = what you treasure. Losing = forgetting worth. Receiving = recognizing value. Fake = false values.",
        "Clothing": "Identity, persona, protection. What you wear = how you present self. Changing clothes = transformation. Wrong clothes = not fitting in.",
        "Computer": "Mind, processing, connections. Computer = mental processes. Crash = mental overwhelm. Hacked = boundaries violated. Working well = clear thinking.",
        "Camera": "Perspective, memory, observation. Camera = capturing moments. Taking photo = wanting to remember. Looking at photos = reviewing past.",
        "Bed": "Rest, intimacy, vulnerability. Bed = personal space. Comfortable = self-care. Unmade = chaos. Someone in bed = intimacy or invasion.",
        "Chair": "Position, rest, status. Chair = where you sit in life. Throne = power. Empty chair = absence. Broken chair = unstable position.",
        "Knife": "Cutting away, division, precision. Knife = tool for change. Sharp = effective. Dull = ineffective. Wielding = power. Threatened = fear."
    },

    "Elements": {
        "Fire": "Passion, destruction, transformation. Fire purifies and destroys. Out of control = destructive anger. Controlled = useful passion. Fireplace = warmth and comfort.",
        "Rain": "Cleansing, emotions, renewal. Gentle rain = emotional release. Storm = turmoil. Drought ending = relief. Inside during rain = protected from emotions.",
        "Snow": "Purity, cold emotions, hibernation. Snow covers everything. Beautiful snow = fresh start. Blizzard = overwhelmed. Melting = warming up.",
        "Wind": "Change, spirit, invisible forces. Strong wind = major changes. Gentle breeze = subtle influences. Hurricane = life upheaval. Calm = peace.",
        "Sun": "Consciousness, vitality, clarity. Sun = awareness and life force. Rising = new beginning. Setting = ending. Bright = clarity. Hidden = confusion.",
        "Moon": "Feminine energy, emotions, cycles. Moon = unconscious and intuition. Full moon = completion. New moon = new beginning. Blood moon = intense emotions.",
        "Stars": "Hope, guidance, aspirations. Stars guide and inspire. Bright stars = clear goals. Falling star = wish. Constellation = pattern recognition.",
        "Thunder": "Sudden realization, power, awakening. Lightning strikes = sudden insight. Thunder = announcement. Storm = emotional intensity.",
        "Earthquake": "Foundation shaking, major change, instability. Earthquakes = life upheaval. Everything unstable. Aftershocks = continuing changes.",
        "Tornado": "Chaos, destructive emotions, upheaval. Tornadoes destroy to rebuild. Eye of storm = calm center. Path of destruction = clearing old.",
        "Flood": "Overwhelming emotions, cleansing, destruction. Flood = emotional overwhelm. Rising water = increasing pressure. Escape = need to process.",
        "Drought": "Emotional dryness, scarcity, barrenness. Drought = lack of emotional nourishment. Thirst = need for connection. Desert = isolation.",
        "Ice": "Frozen emotions, coldness, preservation. Ice = emotional shutdown. Melting = thawing. Slipping = unstable footing. Frozen in ice = stuck.",
        "Fog": "Confusion, uncertainty, mystery. Fog obscures vision. Can't see path = unclear direction. Lifting fog = clarity coming.",
        "Clouds": "Thoughts, emotions, obscured vision. Dark clouds = trouble. White fluffy clouds = pleasant thoughts. Storm clouds = brewing emotions."
    },

    "Colors": {
        "Red": "Passion, anger, vitality, danger. Red = intense energy. May indicate strong emotions, warning, or life force. Stop sign. Blood.",
        "Blue": "Calm, sadness, truth, spirituality. Blue = peaceful or melancholic. Clear blue = clarity. Dark blue = deep emotions. Sky and water.",
        "Green": "Growth, healing, jealousy, nature. Green = renewal or envy. Bright green = growth. Dark green = envy. Money. Nature.",
        "Yellow": "Joy, intellect, cowardice, optimism. Yellow = bright mental energy. Golden yellow = illumination. Dull yellow = fear. Sunshine.",
        "Black": "Unknown, shadow, mystery, potential. Black not always negative. Represents unconscious. Void. Darkness before dawn. Sophistication.",
        "White": "Purity, clarity, innocence, emptiness. White = clean slate. New beginning. Blank canvas. Sterility. Surrender. Peace.",
        "Purple": "Spirituality, royalty, luxury, intuition. Purple = mystical energy. May indicate spiritual growth. Crown chakra. Nobility.",
        "Orange": "Creativity, enthusiasm, warmth. Orange = creative energy. Playfulness. Vitality. Sunset. Citrus. Autumn.",
        "Brown": "Grounding, stability, earthiness. Brown = solid foundation. Comfort. Reliability. Earth. Wood. Natural.",
        "Gray": "Neutrality, depression, ambiguity. Gray = in-between state. Uncertainty. Blandness. Overcast. Elder wisdom.",
        "Pink": "Love, gentleness, romance. Pink = soft love. Compassion. Femininity. Tenderness. Innocence. Healing.",
        "Gold": "Value, achievement, divine. Gold = worth and success. Enlightenment. Highest achievement. Precious. Sun energy.",
        "Silver": "Reflection, moon energy, intuition. Silver = mirror quality. Feminine wisdom. Lunar. Precious but less than gold.",
        "Turquoise": "Healing, communication, protection. Turquoise = throat chakra. Speaking truth. Ocean energy. Healing waters."
    }
}

print("Creating dream symbol files...")
count = 0

# Create files for each symbol
for category, symbols in dream_symbols.items():
    category_dir = dream_dir / category.lower()
    category_dir.mkdir(exist_ok=True)

    for symbol, meaning in symbols.items():
        filename = symbol.lower() + ".txt"
        path = category_dir / filename

        content = f"{symbol.replace('_', ' ').upper()}\n\nCategory: {category}\n\n{meaning}\n\nRemember: Dream symbols are deeply personal. Your own associations and feelings about symbols matter most. Use this as a starting point, not absolute truth."

        with open(path, 'w', encoding='utf-8') as f:
            f.write(wrap_text(content))

        count += 1

print(f"  Created {count} symbol files")

# Create master index
print("Creating dream dictionary index...")
index_content = "DREAM DICTIONARY INDEX\n\nComplete Symbol List by Category\n\n"
total = 0

for category, symbols in dream_symbols.items():
    index_content += f"\n{category.upper()} ({len(symbols)} symbols):\n"
    for symbol in sorted(symbols.keys()):
        index_content += f"  {symbol.replace('_', ' ')}\n"
        total += 1

index_content += f"\n\nTotal Symbols: {total}\n\nAll symbols include detailed interpretations. Remember that dream interpretation is personal - trust your intuition."

index_path = dream_dir / "00_index.txt"
with open(index_path, 'w', encoding='utf-8') as f:
    f.write(wrap_text(index_content))

# Create interpretation guide
print("Creating dream interpretation guide...")
guide_content = """HOW TO INTERPRET YOUR DREAMS

COMPREHENSIVE GUIDE

WHY DREAMS MATTER:
Dreams are the language of the unconscious mind. They process emotions, solve problems, and reveal truths we hide from ourselves.

REMEMBERING YOUR DREAMS:

1. Keep journal and pen by bed
2. Set intention before sleep: "I will remember my dreams"
3. Don't move when you first wake up
4. Write immediately - memories fade fast
5. Record everything: symbols, colors, emotions, people, places
6. Date all entries
7. Note what was happening in waking life

INTERPRETATION PROCESS:

STEP 1: EMOTIONS
How did you FEEL in the dream? This is the most important element. The feeling tells you what your unconscious is processing.

STEP 2: SYMBOLS
What objects, people, places appeared? List everything you remember.

STEP 3: CONTEXT
What's happening in your waking life? Dreams often reflect current concerns, fears, hopes, conflicts.

STEP 4: PATTERNS
Do symbols repeat? Do you have recurring dreams? Repetition indicates unresolved issues.

STEP 5: PERSONAL ASSOCIATIONS
What does each symbol mean TO YOU? Your personal history with symbols matters most.

STEP 6: ASK QUESTIONS
- What part of my life does this represent?
- What am I avoiding or embracing?
- What needs my attention?
- What is changing or ending?
- What wants to be born?

COMMON DREAM TYPES:

PROCESSING DREAMS:
Most common. Your brain sorting daily experiences. Usually forgotten quickly. Not prophetic.

ANXIETY DREAMS:
Late for test, chased, naked in public. Reflect waking stress. Common during transitions.

PROPHETIC DREAMS:
Rare but real. Feel different from normal dreams. Very vivid and memorable. Often come true.

LUCID DREAMS:
You know you're dreaming. Can control dream. Practice increases ability. Gateway to astral projection.

NIGHTMARES:
Unresolved fears manifesting. Facing them in dreams reduces their power. Often childhood trauma.

RECURRING DREAMS:
Same dream repeats until you get the message. Indicates unresolved issue demanding attention.

HEALING DREAMS:
Process grief, trauma, illness. Often feature deceased loved ones bringing messages.

VISITATION DREAMS:
Deceased loved ones truly visit. Feel different from regular dreams. Comforting and real.

PRECOGNITIVE DREAMS:
See future events. Often mundane moments that later happen exactly as dreamed.

SHADOW WORK DREAMS:
Meet rejected parts of yourself. Usually appear as enemies or frightening figures.

IMPORTANT PRINCIPLES:

1. YOU ARE THE BEST INTERPRETER
No dream dictionary can replace your intuition. These are guides, not rules.

2. SYMBOLS ARE PERSONAL
Your associations matter most. Snake might mean danger to one person, wisdom to another.

3. DREAMS ARE NOT LITERAL
Death dreams rarely predict physical death. They indicate transformation.

4. EMOTIONS ARE KEY
The feeling matters more than the content. Fear in a dream about puppies means something different than joy.

5. CONTEXT MATTERS
Same symbol means different things in different life contexts.

6. TIMING IS RELEVANT
Dreams often reflect what happened that day or what's coming up.

7. RECURRING THEMES
Pay attention to patterns across many dreams. They reveal core issues.

ADVANCED TECHNIQUES:

DREAM INCUBATION:
Ask a question before sleep. Your dreams will answer it. Write question clearly. Read it before bed. Expect an answer.

LUCID DREAMING:
Reality checks during day transfer to dreams. Look at hands, check clocks. Practice makes perfect.

DREAM RE-ENTRY:
If dream was interrupted, you can return. Relax, visualize last scene, let dream continue.

ACTIVE IMAGINATION:
Jung's technique. Dialogue with dream figures while awake. Ask what they represent.

GESTALT TECHNIQUE:
Become each element. "I am the house. I feel solid but old..." Reveals hidden meanings.

KEEPING A DREAM JOURNAL:

Record:
- Date
- Title (name the dream)
- Full description
- Emotions
- Colors
- Recurring symbols
- Life context
- Initial interpretation
- Later insights

Review monthly. Patterns emerge over time.

TROUBLESHOOTING:

CAN'T REMEMBER DREAMS:
- Set strong intention
- Wake naturally (no alarm)
- Stay still when waking
- Practice daily
- Stop alcohol/marijuana (blocks dream recall)

ONLY NIGHTMARES:
- Usually means avoiding something
- Face fears in dreams
- Practice lucid dreaming
- Address waking life stress
- Therapy may help

SAME DREAM REPEATS:
- Contains urgent message
- Will repeat until resolved
- Face it rather than flee
- Wake up during dream and journal
- Resolve waking life parallel

NO DREAMS AT ALL:
Everyone dreams. You're not remembering.
- Improve sleep quality
- Set intention
- Keep journal anyway
- Be patient
- May take weeks of practice

CONFUSING DREAMS:
Not all dreams have clear messages. Some are just brain processing. If you can't figure it out, let it go. Important ones repeat.

RED FLAGS:
See a professional if dreams include:
- Persistent violent dreams
- Dreams that worsen anxiety
- Can't distinguish dream from reality
- Dreams that prevent functioning
- Traumatic nightmares (PTSD)

FINAL WISDOM:
Trust yourself. Your unconscious knows what you need to know. Dreams are gifts from your deeper self. Honor them.

The symbols in this dictionary are starting points. Your truth matters most.

Sweet dreams.
"""

guide_path = dream_dir / "00_how_to_interpret_dreams.txt"
with open(guide_path, 'w', encoding='utf-8') as f:
    f.write(wrap_text(guide_content))

# Create quick reference
print("Creating quick reference...")
quick_ref = """DREAM DICTIONARY QUICK REFERENCE

MOST COMMON DREAM SYMBOLS:

Being Chased: Avoiding something
Falling: Loss of control
Flying: Freedom, success
Teeth Falling Out: Anxiety, powerlessness
Naked in Public: Vulnerability, exposure
Water: Emotions (calm=peace, rough=turmoil)
Death: Transformation, not literal death
House: Yourself (rooms=aspects of self)
Car: Life direction and control
Test/Exam: Feeling judged or unprepared

REMEMBER:
- Emotions are most important
- Symbols are personal
- Dreams are not literal
- You are the best interpreter
- Trust your intuition

See full guide for detailed interpretations.
"""

quick_path = dream_dir / "00_quick_reference.txt"
with open(quick_path, 'w', encoding='utf-8') as f:
    f.write(wrap_text(quick_ref))

print()
print("=" * 70)
print("DREAM DICTIONARY COMPLETE!")
print("=" * 70)
print()
print(f"Location: {dream_dir.absolute()}")
print(f"Total symbol files: {count}")
print(f"Categories: {len(dream_symbols)}")
print()
print("Created:")
print("  - 100+ individual symbol files")
print("  - Master index")
print("  - Comprehensive interpretation guide")
print("  - Quick reference guide")
print()
print("All content formatted for M5Cardputer (39 chars wide)")
print("=" * 70)
