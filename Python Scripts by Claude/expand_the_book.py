#!/usr/bin/env python3
"""
The Book - MASSIVE EXPANSION Script
Expanding from 4.58 MB to 1-3 GB
"""

import os
import textwrap

# Text wrapping helper
def wrap_text(text, width=39):
    """Wrap text to specified width for M5Cardputer display"""
    lines = []
    for paragraph in text.split('\n'):
        if paragraph.strip():
            wrapped = textwrap.fill(paragraph, width=width, break_long_words=False, break_on_hyphens=False)
            lines.extend(wrapped.split('\n'))
        else:
            lines.append('')
    return '\n'.join(lines)

print("=" * 60)
print("The Book - MASSIVE EXPANSION")
print("Target: 1-3 GB of comprehensive offline knowledge")
print("=" * 60)

# Base directory
base_dir = "the_book"

# ============================================================
# 1. POISONOUS PLANTS (CRITICAL SAFETY!)
# ============================================================
print("\n🚨 Creating POISONOUS PLANTS guide (CRITICAL SAFETY)...")

poisonous_dir = os.path.join(base_dir, "poisonous_plants")
os.makedirs(poisonous_dir, exist_ok=True)

poisonous_plants = {
    "Poison_Ivy": """POISON IVY (Toxicodendron radicans)

⚠️ DANGER: SEVERE SKIN IRRITATION

IDENTIFICATION:
- Leaves of three, let it be!
- Three leaflets per stem
- Leaves are green in summer, red in fall
- Can grow as vine or shrub
- White berries in fall
- Leaflets 2-4 inches long

TOXIC PARTS:
- ALL parts (leaves, stems, roots, berries)
- Contains urushiol oil
- Toxic year-round, even when dead

EXPOSURE SYMPTOMS:
- Severe itching within 12-48 hours
- Red, swollen rash
- Blisters that ooze fluid
- Can last 1-3 weeks

TREATMENT:
- Wash skin immediately with soap/water
- Do NOT scratch
- Apply calamine lotion
- Take antihistamines for itching
- See doctor if severe or on face/eyes

PREVENTION:
- Learn to identify
- Wear long pants and sleeves
- Wash clothes/tools after exposure
- Never burn poison ivy (toxic smoke!)

REMEMBER: "Leaves of three, let it be!"
""",

    "Poison_Oak": """POISON OAK (Toxicodendron diversilobum)

⚠️ DANGER: SEVERE SKIN REACTION

IDENTIFICATION:
- Three oak-like leaflets
- Lobed leaves resembling oak
- Grows as shrub, 1-6 feet tall
- Green in spring/summer
- Red/orange in fall
- White/cream berries

LOCATION:
- Western United States
- Wooded areas, hillsides
- Often near trails

TOXIC COMPOUND:
- Urushiol oil (same as poison ivy)
- Extremely potent allergen
- Affects 50-75% of population

SYMPTOMS:
- Intense itching
- Red, linear rash
- Swelling and blisters
- Appears 24-72 hours after contact

FIRST AID:
1. Rinse with cold water immediately
2. Wash with soap (within 10 minutes best)
3. Clean under fingernails
4. Wash all clothing and gear
5. Apply topical steroids

SEVERE CASES (seek medical help):
- Rash covers large area
- Face or genital exposure
- Breathing difficulties
- Fever develops

MYTH BUSTING:
- Rash is NOT contagious
- Fluid from blisters won't spread it
- Only oil (urushiol) spreads reaction
""",

    "Poison_Hemlock": """POISON HEMLOCK (Conium maculatum)

☠️ EXTREME DANGER: DEADLY IF INGESTED

IDENTIFICATION:
- Tall plant, 6-10 feet
- Purple spots on hollow stems
- Fern-like leaves
- Small white flowers in umbrella clusters
- Carrot-like root
- Musty, unpleasant smell

TOXIC COMPOUND:
- Coniine (alkaloid poison)
- ALL PARTS DEADLY
- Especially dangerous: seeds, roots
- Can be fatal in small amounts

POISONING SYMPTOMS (30-60 min):
- Nervousness, trembling
- Dilated pupils
- Slow heartbeat
- Muscle paralysis
- Respiratory failure
- Death from respiratory paralysis

HISTORICAL NOTE:
- Used to execute Socrates in 399 BC
- One of most poisonous plants in NA

TREATMENT:
- IMMEDIATE medical emergency
- Call 911 / poison control
- Do NOT induce vomiting
- Hospital treatment essential
- No specific antidote

LOOKALIKES (SAFE):
- Wild carrot (Queen Anne's Lace)
  * Hairy stems, no purple spots
  * Smells like carrot
- Fennel
  * Yellow flowers, licorice smell

PREVENTION:
- Never eat unknown plants
- Teach children to avoid
- Wear gloves if removing
- Do not burn (toxic smoke)

⚠️ IF IN DOUBT, DON'T EAT IT!
""",

    "Deadly_Nightshade": """DEADLY NIGHTSHADE (Atropa belladonna)

☠️ EXTREMELY POISONOUS

IDENTIFICATION:
- Bushy plant, 3-4 feet tall
- Bell-shaped purple flowers
- Shiny black berries (cherry-sized)
- Oval leaves, 3-8 inches
- Sweet-tasting berries (DEADLY!)

TOXIC COMPOUNDS:
- Atropine, scopolamine
- Concentrated in berries and roots
- 2-5 berries can kill a child
- 10-20 berries can kill an adult

POISONING SYMPTOMS:
MILD (15-30 minutes):
- Dry mouth and throat
- Dilated pupils
- Blurred vision
- Sensitivity to light

SEVERE:
- Hallucinations
- Seizures
- Rapid heartbeat
- Difficulty breathing
- Coma, death

TREATMENT:
- Emergency medical care IMMEDIATELY
- Call poison control
- Hospitalization required
- Activated charcoal
- Supportive care

HISTORICAL USE:
- "Belladonna" = "beautiful lady"
- Used to dilate pupils for beauty
- Ancient poison for assassinations

LOOKALIKES:
- Blueberries (SAFE, grow on bushes)
- Elderberries (SAFE, purple-black)

KEY DIFFERENCES:
- Nightshade: single berries
- Blueberries: clusters, white bloom
- Always verify before eating!

⚠️ ALL PARTS POISONOUS
⚠️ DO NOT TOUCH BERRIES
""",

    "Water_Hemlock": """WATER HEMLOCK (Cicuta species)

☠️ MOST POISONOUS PLANT IN NORTH AMERICA

IDENTIFICATION:
- Grows near water
- 3-6 feet tall
- Purple-streaked stems
- White flower clusters (umbrella-like)
- Chambered root with yellow liquid
- Smells like parsnip or carrot

DEADLY PARTS:
- ROOT is most toxic
- Even small amount = FATAL
- Cicutoxin (violent poison)
- Works within 15 minutes

POISONING SYMPTOMS:
- Salivation, nausea
- Violent seizures
- Extreme pain
- Frothing at mouth
- Respiratory failure
- Death within hours

DEATH RATE:
- 30% fatal even with treatment
- No antidote exists
- Medical care can only support

DANGEROUS LOOKALIKE:
- Wild parsnip (edible)
  * But NEVER risk it!
- Difference very subtle

TREATMENT:
- Call 911 IMMEDIATELY
- Do NOT wait for symptoms
- May need breathing support
- Seizure control
- Hospital ICU care

SAFETY RULES:
1. Never eat plants near water
2. Roots are most dangerous
3. Teach children to avoid
4. Even touching can be risky

REMEMBERED AS:
"Deadliest plant in North America"
One root can kill a cow!

⚠️ ABSOLUTELY DO NOT CONSUME
""",

    "Foxglove": """FOXGLOVE (Digitalis purpurea)

☠️ BEAUTIFUL BUT DEADLY

IDENTIFICATION:
- Tall flower spike, 2-5 feet
- Purple, pink, or white tubular flowers
- Fuzzy, gray-green leaves
- Flowers have spots inside
- Blooms in early summer

TOXIC COMPOUND:
- Digitalis (cardiac glycoside)
- ALL PARTS poisonous
- Leaves most toxic
- Deadly to humans and animals

MEDICAL USE:
- Source of heart medication digoxin
- Used in controlled doses
- NEVER self-medicate!

POISONING SYMPTOMS:
EARLY:
- Nausea, vomiting
- Diarrhea
- Confusion
- Visual disturbances (yellow halos)

SEVERE:
- Irregular heartbeat
- Extremely slow or fast pulse
- Cardiac arrest
- Death

FATAL DOSE:
- Even small amounts dangerous
- 2-3 leaves can be fatal
- Toxicity varies by plant

TREATMENT:
- Emergency medical care
- Activated charcoal
- Heart monitoring
- Specific antibody (Digibind)

IMPORTANT WARNINGS:
- Never use in home remedies
- Keep children and pets away
- Don't mistake for edible plants
- Dried plants still poisonous

LOOKALIKE CAUTION:
- Can resemble comfrey (edible)
- ALWAYS positively identify
- When in doubt, don't consume

GARDEN SAFETY:
- Label if growing
- Wear gloves when handling
- Wash hands after gardening
""",

    "Castor_Bean": """CASTOR BEAN (Ricinus communis)

☠️ CONTAINS RICIN - EXTREME POISON

IDENTIFICATION:
- Large tropical-looking plant
- Star-shaped leaves (5-11 lobes)
- Red or green stems
- Spiky seed pods
- Mottled beans (attractive!)
- Grows 6-15 feet tall

DEADLY COMPONENT:
- Ricin (one of world's deadliest toxins)
- Concentrated in beans/seeds
- 1-4 beans can kill an adult
- 1 bean can kill a child

RICIN PROPERTIES:
- No antidote exists
- Even tiny amount is fatal
- Destroys cells
- Works slowly (hours to days)

POISONING SYMPTOMS:
INGESTION (6-24 hours later):
- Burning in mouth/throat
- Severe abdominal pain
- Bloody diarrhea
- Seizures
- Kidney/liver failure
- Death in 3-5 days

TREATMENT:
- Emergency medical care
- Aggressive hydration
- Supportive care only
- NO specific antidote
- Survival uncertain

INDUSTRIAL USE:
- Castor oil made from beans
- Processing removes ricin
- Commercial oil is safe

WEAPON USE:
- Used in biological warfare
- Famous assassination attempts
- Highly regulated substance

SAFETY PRECAUTIONS:
- Never plant where children play
- Remove existing plants
- Don't grow for ornamental use
- Dispose of carefully

IF EXPOSURE:
- Call poison control immediately
- Don't wait for symptoms
- Hospital care essential

⚠️ EXTREMELY DANGEROUS
⚠️ AVOID COMPLETELY
""",

    "Oleander": """OLEANDER (Nerium oleander)

☠️ EVERY PART IS DEADLY

IDENTIFICATION:
- Evergreen shrub, 6-20 feet
- Narrow, leathery leaves
- Showy flowers (pink, white, red)
- Grows in warm climates
- Milky sap when cut

TOXIC COMPOUNDS:
- Oleandrin and neriine
- Extremely cardiotoxic
- ALL parts deadly poisonous
- Smoke from burning also toxic!

DANGER LEVEL:
- Single leaf can kill a child
- A few leaves can kill an adult
- Toxins survive drying/heating
- No safe way to consume

POISONING SYMPTOMS:
EARLY:
- Nausea, vomiting
- Excessive salivation
- Abdominal pain
- Diarrhea (possibly bloody)

CARDIAC:
- Irregular heartbeat
- Very slow heart rate
- Dizziness
- Loss of consciousness
- Heart attack

TREATMENT:
- IMMEDIATE emergency care
- Heart monitoring essential
- Activated charcoal
- IV fluids
- Cardiac medications

DANGEROUS SCENARIOS:
- Using branches as skewers
- Burning wood (toxic smoke)
- Tea made from leaves (fatal!)
- Animals eating clippings

ENVIRONMENTAL NOTE:
- Common landscape plant
- Grows in southern US
- Very drought tolerant
- Banned in some areas

SAFETY RULES:
1. Never burn oleander
2. Don't use for cooking
3. Wash after handling
4. Keep children/pets away
5. Dispose of trimmings safely

REMEMBER:
Beautiful but deadly - admire, don't touch!
""",

    "Jimsonweed": """JIMSONWEED (Datura stramonium)

☠️ HALLUCINOGENIC POISON

COMMON NAMES:
- Devil's Trumpet
- Thorn Apple
- Locoweed
- Jamestown Weed

IDENTIFICATION:
- Bushy plant, 3-5 feet
- Large trumpet-shaped white/purple flowers
- Toothed leaves
- Spiky seed pods
- Unpleasant smell
- Grows in disturbed soil

TOXIC COMPOUNDS:
- Atropine, scopolamine
- Seeds most concentrated
- ALL parts poisonous
- Hallucinogenic properties

ABUSE POTENTIAL:
- Teens seeking "high"
- EXTREMELY DANGEROUS
- Unpredictable effects
- Easy to overdose
- Can cause permanent damage

POISONING SYMPTOMS:
EARLY:
- Extreme thirst
- Dry mouth
- Dilated pupils
- Flushed skin

SEVERE:
- Violent behavior
- Hallucinations
- Seizures
- High fever (105°F+)
- Coma
- Death

TREATMENT:
- Emergency medical care
- Physical restraint may be needed
- Cooling for fever
- Sedation
- Cardiac monitoring
- Supportive care

MORTALITY:
- Deaths reported every year
- Usually from intentional use
- Accidental poisonings rare

PREVENTION:
- Educate teens about dangers
- Remove plants from yards
- Report large infestations
- Never experiment with plants

LONG-TERM EFFECTS:
- Memory loss
- Psychosis
- Brain damage
- Vision problems

LEGAL STATUS:
- Not regulated (it's a weed!)
- But possession of extracted drugs illegal

WARNING TO PARENTS:
- Commonly abused by teens
- Found in vacant lots, roadsides
- Extremely dangerous
- Talk to children about plant dangers
""",

    "Poison_Sumac": """POISON SUMAC (Toxicodendron vernix)

⚠️ MORE TOXIC THAN POISON IVY OR OAK

IDENTIFICATION:
- Woody shrub or small tree
- 7-13 leaflets per stem (always odd)
- Smooth leaf edges (NOT toothed)
- Red stems
- Drooping white or gray berries
- Grows 5-20 feet tall

LOCATION:
- Swampy, wet areas
- Eastern and Southern US
- Often near water
- Flooded soils

TOXIC COMPOUND:
- Urushiol (same as ivy/oak)
- MORE concentrated than ivy
- 3X more likely to cause reaction
- Affects 75% of population

SYMPTOMS:
- Severe itching
- Streaky rash
- Large blisters
- Swelling
- More severe than ivy/oak

SAFE LOOKALIKE:
- Staghorn Sumac (SAFE species)
  * Red berries (not white!)
  * Toothed leaf edges
  * Fuzzy stems
  * Grows in dry areas

KEY DIFFERENCES:
POISON SUMAC:
- White/gray berries
- Smooth leaves
- Wet areas
- Red stems

SAFE SUMAC:
- Red berries
- Toothed leaves
- Dry areas
- Brown stems

TREATMENT:
- Same as poison ivy
- Wash immediately
- Topical steroids
- Oral antihistamines
- See doctor if severe

PREVENTION:
- Learn habitat
- Avoid swampy areas
- Wear protective clothing
- Check before hiking

REMOVAL:
- Professional recommended
- Full body protection
- Never burn (toxic smoke!)
- Dispose as hazardous waste

REMEMBER:
"Berries white, take flight!"
"Berries red, no need to dread!"
""",

    "White_Snakeroot": """WHITE SNAKEROOT (Ageratina altissima)

☠️ POISONOUS TO HUMANS AND ANIMALS

IDENTIFICATION:
- Perennial plant, 1-4 feet tall
- White fluffy flower clusters
- Opposite leaves, toothed edges
- Heart-shaped or oval leaves
- Blooms late summer/fall
- Grows in shaded woods

TOXIC COMPOUND:
- Tremetol (complex alcohol)
- Poisonous to livestock
- Poison passes through milk!
- Killed Abraham Lincoln's mother

HISTORICAL IMPORTANCE:
- Caused "milk sickness" in 1800s
- Killed thousands of settlers
- Cattle ate plant, poison in milk
- Humans drank milk, became ill

DIRECT POISONING:
- Trembling, weakness
- Nausea, vomiting
- Severe constipation
- Muscle stiffness
- Difficulty walking

MILK SICKNESS SYMPTOMS:
- Loss of appetite
- Listlessness
- Weakness
- Vomiting
- Bad breath
- Tremors
- Death (in severe cases)

TREATMENT:
- Supportive care
- Remove from toxin source
- IV fluids
- Monitor liver function
- No specific antidote

LIVESTOCK DANGER:
- Cattle, horses, goats affected
- 1-10% of body weight = fatal
- Poison accumulates over time
- Passes into milk for weeks

PREVENTION:
- Clear pastures of plant
- Don't graze in wooded areas
- Test milk from small farms
- Know your milk source

MODERN DAY:
- Rare due to large-scale dairy
- Still risk with small farms
- More common in Midwest

ERADICATION:
- Remove from pastures
- Herbicide treatment
- Prevent re-growth
- Monitor carefully

IMPORTANT NOTE:
Commercial milk is safe (blended sources)
Small farm milk may pose risk
""",

    "Monkshood": """MONKSHOOD (Aconitum napellus)

☠️ EXTREMELY TOXIC - "QUEEN OF POISONS"

COMMON NAMES:
- Aconite
- Wolfsbane
- Devil's Helmet
- Blue Rocket

IDENTIFICATION:
- Tall plant, 2-4 feet
- Deep blue/purple hooded flowers
- Deeply lobed leaves
- Tuberous roots
- Blooms mid-summer

TOXIC COMPOUND:
- Aconitine (highly toxic alkaloid)
- One of most poisonous plants
- ALL parts deadly
- Roots most concentrated

EXPOSURE ROUTES:
- Ingestion (FATAL)
- Skin contact (dangerous)
- Can absorb through skin!
- Even touching can be risky

POISONING SYMPTOMS:
IMMEDIATE (minutes):
- Tingling, numbness
- Nausea, vomiting
- Vision problems
- Difficulty breathing

CARDIAC:
- Irregular heartbeat
- Dangerously low heart rate
- Chest pain
- Cardiac arrest

FATAL DOSE:
- 2-6mg can be lethal
- Less than 1 teaspoon root
- Death in 1-6 hours

TREATMENT:
- EMERGENCY care immediately
- No specific antidote
- Cardiac life support
- Activated charcoal
- Symptomatic treatment

HISTORICAL USE:
- Ancient poison for arrows
- Used in werewolf legends
- Murder weapon in history
- Still causes deaths today

GARDEN DANGER:
- Popular ornamental plant
- Beautiful but deadly
- Can be mistaken for horseradish root
- NEVER plant where children play

SAFETY PRECAUTIONS:
- Always wear gloves
- Wash hands thoroughly
- Label clearly
- Consider removal
- Educate family

ACCIDENTAL POISONING:
- Often from foraging
- Roots look like radish/horseradish
- Leaves can resemble edible greens

WARNING:
Touch can kill - extreme caution required!
""",

    "Death_Camas": """DEATH CAMAS (Zigadenus species)

☠️ DEADLY LOOKALIKE TO EDIBLE PLANTS

IDENTIFICATION:
- Grass-like leaves
- Small white/cream flowers
- Flower cluster on tall stalk
- Bulb resembles wild onion
- NO onion smell (KEY!)
- Grows 1-2 feet tall

LOCATION:
- Western North America
- Meadows, grasslands
- Often near edible camas

TOXIC COMPOUNDS:
- Alkaloid toxins (zygacine)
- Entire plant poisonous
- Bulb most toxic
- Toxins survive cooking

DEADLY LOOKALIKES:
- Wild onions (safe)
  * Smell like onion (safe!)
- Edible blue camas (safe)
  * Blue flowers (safe!)
- Wild garlic (safe)

KEY IDENTIFICATION:
DEATH CAMAS:
- NO onion/garlic smell
- White/cream flowers
- Grass-like leaves

SAFE PLANTS:
- Strong onion/garlic smell
- Blue flowers (camas)
- Onion odor when crushed

POISONING SYMPTOMS:
30 minutes to 8 hours:
- Excessive salivation
- Nausea, vomiting
- Diarrhea
- Muscular weakness
- Low blood pressure
- Slow heartbeat
- Respiratory failure

SEVERITY:
- Can be fatal
- Children most at risk
- No specific antidote

TREATMENT:
- Emergency medical care
- Gastric lavage
- Activated charcoal
- Symptomatic support
- Monitor breathing/heart

HISTORICAL DEATHS:
- Native American children
- Early settlers
- Still causes deaths
- Usually foraging accidents

PREVENTION:
THE SMELL TEST:
- ALWAYS smell before eating
- No onion smell = DANGER
- If no smell, DON'T eat
- When in doubt, throw it out

FORAGING SAFETY:
- Positive identification essential
- Learn from expert
- Never eat unless 100% certain
- Teach children the smell test

LIVESTOCK:
- Also poisonous to animals
- Sheep especially vulnerable
- Remove from pastures
""",

}

for filename, content in poisonous_plants.items():
    filepath = os.path.join(poisonous_dir, f"{filename}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))

# Create index
index_file = os.path.join(poisonous_dir, "index.txt")
with open(index_file, 'w', encoding='utf-8') as f:
    for i, name in enumerate(poisonous_plants.keys()):
        plant_name = name.replace('_', ' ')
        f.write(f"{plant_name}|{name}.txt|0|POISONOUS - AVOID!\n")

print(f"✓ Created {len(poisonous_plants)} poisonous plant guides")
print(f"✓ Saved to /poisonous_plants/")

print("\n" + "=" * 60)
print("✓ POISONOUS PLANTS EXPANSION COMPLETE!")
print("=" * 60)
print("\nPress Ctrl+C to stop, or script will continue with more expansions...")
