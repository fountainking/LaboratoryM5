#!/usr/bin/env python3
"""
The Book - MEDICAL & HEALTH EXPANSION
Comprehensive survival medicine guide
"""

import os
import textwrap

def wrap_text(text, width=39):
    """Wrap text to specified width"""
    lines = []
    for paragraph in text.split('\n'):
        if paragraph.strip():
            wrapped = textwrap.fill(paragraph, width=width, break_long_words=False, break_on_hyphens=False)
            lines.extend(wrapped.split('\n'))
        else:
            lines.append('')
    return '\n'.join(lines)

print("=" * 60)
print("The Book - MEDICAL & HEALTH EXPANSION")
print("Survival medicine for extended isolation")
print("=" * 60)

base_dir = "the_book"
medical_dir = os.path.join(base_dir, "medical")
os.makedirs(medical_dir, exist_ok=True)

medical_guides = {
    "Diarrhea_Treatment": """DIARRHEA - TREATMENT & PREVENTION

⚠️ CAN BE LIFE-THREATENING IN SURVIVAL

CAUSES:
- Contaminated water
- Spoiled food
- Bacteria, parasites, viruses
- Stress
- Unfamiliar foods

DANGER SIGNS:
- Dehydration (dry mouth, no tears)
- Dark urine or no urination
- Dizziness when standing
- Rapid heartbeat
- Sunken eyes
- Severe cramping

IMMEDIATE TREATMENT:
1. HYDRATION (Most Important!)
   - Drink constantly
   - ORS (Oral Rehydration Solution)
   - Clear liquids

HOMEMADE ORS RECIPE:
- 1 liter clean water
- 6 teaspoons sugar
- 1/2 teaspoon salt
Mix well, drink throughout day

Alternative:
- Coconut water (natural ORS)
- Rice water (boil rice, drink water)
- Clear broth

FOOD GUIDANCE:
AVOID:
- Dairy products
- Fatty/greasy foods
- Spicy foods
- Raw fruits/vegetables
- Caffeine, alcohol

EAT (when tolerated):
- Plain white rice
- Bananas
- Dry toast or crackers
- Boiled potatoes
- Applesauce
- Plain chicken (boiled)

NATURAL REMEDIES:
- Activated charcoal (absorbs toxins)
- Black tea (tannins help)
- Ginger tea (anti-nausea)
- Cinnamon tea
- Blackberry root tea

PREVENTION:
- Boil all water (rolling boil, 1 min)
- Cook food thoroughly
- Wash hands frequently
- Avoid questionable food
- Proper food storage

WHEN TO WORRY:
- Blood in stool
- High fever (>101°F)
- Severe abdominal pain
- Lasts more than 3 days
- Signs of dehydration
- Diarrhea in infants/elderly

CHILDREN & INFANTS:
- More vulnerable to dehydration
- Give ORS every 5-10 minutes
- Continue breastfeeding
- Monitor wet diapers
- Seek help if worsening

SURVIVAL TIP:
Store oral rehydration salts packets
Life-saving in emergency
""",

    "Constipation_Relief": """CONSTIPATION - CAUSES & TREATMENT

COMMON IN SURVIVAL SITUATIONS

WHY IT HAPPENS:
- Dehydration (most common)
- Lack of fiber
- Reduced activity
- Stress
- Holding it in
- Change in diet
- Limited food variety

SYMPTOMS:
- No bowel movement 3+ days
- Hard, dry stools
- Straining
- Abdominal pain/bloating
- Feeling of incomplete evacuation

TREATMENT:

1. HYDRATION (Essential!)
   - Drink 8-10 glasses water daily
   - Warm water in morning
   - Prune juice if available

2. INCREASE FIBER:
   - Edible plants/greens
   - Berries
   - Nuts and seeds
   - Root vegetables
   - Whole grains

3. NATURAL LAXATIVES:
   - Prunes or prune juice (best)
   - Dandelion greens
   - Burdock root
   - Psyllium seeds
   - Flax seeds

4. EXERCISE:
   - Walk 15-30 minutes
   - Abdominal massage
   - Squatting position
   - Movement stimulates bowels

5. POSITIONING:
   - Squat position (natural)
   - Elevate feet when sitting
   - Lean forward slightly
   - Don't strain excessively

HERBAL REMEDIES:
- Dandelion tea (mild laxative)
- Chickweed (bulk laxative)
- Yellow dock root
- Cascara bark (strong!)

FOODS THAT HELP:
- Any berries
- Apples (with skin)
- Pears
- Sweet potatoes
- Beans/legumes
- Oats
- Seeds

FOODS TO AVOID:
- Processed foods
- Cheese
- Red meat
- Refined grains
- Bananas (worsens it)

ABDOMINAL MASSAGE:
- Lie on back
- Gentle circular motions
- Follow large intestine path
- Right side up, across, left side down
- 10-15 minutes, 2-3 times daily

PREVENTION:
- Stay hydrated
- Eat fiber-rich foods
- Regular physical activity
- Don't ignore urge to go
- Establish routine

WARNING SIGNS:
- Severe abdominal pain
- Vomiting
- Blood in stool
- No movement for 7+ days
- Fever
- Sudden weight loss

EMERGENCY MEASURES:
- Glycerin suppository (if available)
- Soapy water enema (last resort)
- Seek medical help if severe

CHILDREN:
- More susceptible
- Ensure adequate fluids
- Encourage movement/play
- Don't force straining

IMPORTANT:
Severe constipation can become
medical emergency (impaction)
""",

    "Wound_Care_Infection": """WOUND CARE & INFECTION PREVENTION

CRITICAL SURVIVAL SKILL

IMMEDIATE WOUND CARE:

1. STOP BLEEDING:
   - Direct pressure (5-10 min)
   - Elevate above heart
   - Pressure points if severe
   - Tourniquet only if life-threatening

2. CLEAN THE WOUND:
   - Wash hands first!
   - Flush with clean water
   - Remove debris gently
   - Don't use alcohol/peroxide on wound
     (damages tissue)
   - Soap around wound, not in it

3. DISINFECT:
   If available:
   - Diluted iodine solution
   - Hydrogen peroxide (briefly)
   - Honey (natural antibiotic!)
   - Salt water (1 tsp per cup)

4. COVER:
   - Clean, dry bandage
   - Change daily or if wet
   - Monitor for infection

INFECTION SIGNS:
⚠️ Watch carefully!
- Increased pain
- Redness spreading
- Swelling
- Heat around wound
- Pus (yellow/green discharge)
- Red streaks (blood poisoning!)
- Fever
- Bad smell

TREATING INFECTION:

MILD INFECTION:
- Clean 2-3 times daily
- Apply honey (proven antibiotic)
- Warm compresses
- Keep elevated
- Monitor closely

NATURAL ANTIBIOTICS:
- Honey (medicinal grade best)
- Garlic (crushed, applied)
- Tea tree oil (diluted)
- Turmeric paste
- Yarrow (stops bleeding, antiseptic)
- Plantain leaves (crushed)
- Pine resin (antiseptic)

SEVERE INFECTION:
⚠️ MEDICAL EMERGENCY
- Red streaks = blood poisoning
- Can be fatal
- Need antibiotics
- Seek help immediately

WOUND CLOSURE:

WHEN TO CLOSE:
- Clean wound
- Less than 6 hours old
- Not infected
- Edges align well

HOW TO CLOSE:
- Butterfly bandages (best)
- Steri-strips
- Super glue (in emergency)
- Sutures (if trained)

DON'T CLOSE IF:
- Infected
- Old wound (>6 hours)
- Dirty wound
- Animal bite
- Puncture wound

DRESSING CHANGES:
- Wash hands
- Remove old dressing gently
- Clean wound
- Check for infection
- Apply new dressing
- Secure but not too tight

SPECIAL WOUNDS:

PUNCTURE WOUNDS:
- High infection risk
- Don't close
- Clean deeply
- Monitor closely
- Tetanus risk

BURNS:
- Cool with clean water (10-20 min)
- Don't use ice
- Cover loosely
- Honey for minor burns
- Aloe vera

ANIMAL BITES:
- High infection risk
- Clean thoroughly
- Don't close
- Rabies risk (mammals)
- Monitor carefully

PREVENTION:
- Wear protective gear
- Be careful with tools
- Keep tools sharp (less likely to slip)
- Maintain hygiene
- Clean cuts immediately

SUPPLIES TO STOCKPILE:
- Bandages (various sizes)
- Gauze pads
- Medical tape
- Antibiotic ointment
- Honey
- Iodine
- Butterfly closures
- Gloves

HEALING TIMELINE:
- Small cuts: 3-7 days
- Deeper wounds: 2-3 weeks
- Major wounds: 4-6 weeks
- Keep clean throughout

SCARS:
- Vitamin E oil reduces scarring
- Aloe vera gel
- Keep wound moist
- Massage gently when healed
""",

    "Fever_Management": """FEVER - WHEN TO WORRY & TREATMENT

BODY'S DEFENSE MECHANISM

WHAT IS FEVER:
- Body temp above 100.4°F (38°C)
- Sign of infection
- Not always bad
- Helps fight illness

MEASURING WITHOUT THERMOMETER:
- Forehead feels hot
- Flushed, red face
- Sweating or chills
- Comparison to others
- Hot to touch on back/chest

FEVER LEVELS:
- 100-102°F: Low-grade (mild)
- 102-104°F: Moderate
- 104°F+: High (concerning)
- 105°F+: Very high (dangerous)

TREATMENT:

LOW FEVER (100-102°F):
- May not need treatment
- Rest
- Fluids (critical!)
- Light clothing
- Monitor

MODERATE FEVER (102-104°F):
- Cool compress on forehead
- Lukewarm sponge bath
- Remove excess clothing
- Drink constantly
- Rest
- Light, easy foods

NATURAL FEVER REDUCERS:
- Willow bark tea (natural aspirin)
- Elderberry
- Yarrow tea
- Catnip tea
- Boneset tea
- Basil tea

PHYSICAL COOLING:
- Cool, wet cloth on forehead
- Lukewarm bath (NOT cold!)
- Fan gently
- Remove blankets
- Drink cold water
- Stay in shade/cool area

DANGER SIGNS:
⚠️ SEEK HELP IF AVAILABLE:
- Fever above 104°F
- Lasts more than 3 days
- Severe headache
- Stiff neck
- Confusion/delirium
- Difficulty breathing
- Severe abdominal pain
- Rash with fever
- Seizures
- Vomiting repeatedly

CHILDREN & INFANTS:
MORE SERIOUS:
- Infants under 3 months: 100.4°F = emergency
- Listless, won't wake
- Not drinking
- Crying constantly
- Seizure
- Unusual rash

HYDRATION:
CRITICAL FOR FEVER:
- Sweating loses water
- Drink even if not thirsty
- Water, herbal tea, broth
- Small sips constantly
- Monitor urine color (pale = good)

WHAT NOT TO DO:
- Don't use ice bath (dangerous)
- Don't bundle up
- Don't stop drinking
- Don't ignore high fever
- Don't use rubbing alcohol bath

WHEN FEVER BREAKS:
- Sweating increases
- Temperature drops
- Feeling better
- Continue rest
- Keep hydrating
- Gradual recovery

FEVER WITH OTHER SYMPTOMS:

+ COUGH/CONGESTION:
- Likely respiratory infection
- Steam inhalation
- Honey for cough
- Rest upright

+ VOMITING/DIARRHEA:
- Likely gastroenteritis
- Critical hydration
- ORS solution
- Monitor closely

+ HEADACHE/STIFF NECK:
⚠️ Possible meningitis
- Very serious
- Seek help urgently

+ RASH:
- Could indicate serious infection
- Note pattern/spread
- Don't scratch
- Monitor closely

FEVER IN SURVIVAL:
- Common with infections
- Stress lowers immunity
- Poor nutrition worsens
- Keep clean to prevent
- Conserve energy
- Stay hydrated

PREVENTION:
- Hygiene (handwashing)
- Cook food properly
- Boil water
- Avoid sick people
- Stay dry and warm
- Adequate rest
- Good nutrition

FEVER MYTHS:
✗ Feed a cold, starve a fever (FALSE)
  - Eat if hungry, both conditions
✗ Fever causes brain damage (FALSE)
  - Only if extremely high (107°F+)
✗ Always needs medicine (FALSE)
  - Low fever helpful

MONITORING:
- Check temp regularly
- Note pattern (rising/falling)
- Record symptoms
- Track fluids consumed
- Watch for complications

RECOVERY:
- Rest even after fever breaks
- Continue fluids
- Gradual return to activities
- Eat nutritious foods
- Rebuild strength slowly
""",

    "Dehydration": """DEHYDRATION - RECOGNITION & TREATMENT

LEADING CAUSE OF DEATH IN SURVIVAL

WHAT IT IS:
- Body losing more water than taking in
- Can be rapid and deadly
- Affects all body functions

CAUSES:
- Insufficient drinking
- Diarrhea/vomiting
- Sweating (heat, exertion)
- Fever
- Urinating frequently
- Bleeding

SYMPTOMS BY SEVERITY:

MILD DEHYDRATION:
- Thirst
- Dry mouth
- Darker urine
- Less urination
- Fatigue
- Dizziness

MODERATE DEHYDRATION:
- Very thirsty
- Dry mouth, sticky
- Little to no urine
- Dark yellow urine
- Sunken eyes
- Headache
- Rapid heartbeat
- Low blood pressure

SEVERE DEHYDRATION:
⚠️ MEDICAL EMERGENCY:
- Extreme thirst or no thirst
- Very dry mouth/skin
- No urination
- Very dark urine or none
- Sunken eyes
- Rapid breathing
- Rapid, weak pulse
- Cold hands/feet
- Confusion/delirium
- Unconsciousness

SKIN PINCH TEST:
- Pinch skin on back of hand
- Release
- If skin stays "tented" = dehydrated
- Should snap back immediately

TREATMENT:

MILD DEHYDRATION:
- Drink water slowly
- Small sips frequently
- Don't chug
- Continue until urine pale

MODERATE-SEVERE:
- ORS (Oral Rehydration Solution)
- If unconscious = medical emergency

HOMEMADE ORS:
Recipe 1:
- 1 liter clean water
- 6 tsp sugar
- 1/2 tsp salt

Recipe 2:
- 1 liter clean water
- 8 tsp sugar
- 1 tsp salt
- 1 cup orange juice (if available)

HOW TO GIVE:
- Small sips every 5 minutes
- 1-2 ounces at a time
- Gradually increase
- Continue for several hours
- Goal: pale urine

NATURAL SOURCES:
- Coconut water (excellent!)
- Watermelon
- Cucumbers
- Oranges
- Berries
- Clear broth

PREVENTION:
- Drink before thirsty
- Clear/pale urine = good
- Hot weather = drink more
- Exertion = drink more
- With meals
- Morning and night

DAILY WATER NEEDS:
Normal conditions:
- Adults: 2-3 liters (8-12 cups)
- Children: 1-2 liters

Hot weather/exertion:
- Adults: 4-6 liters or more
- Drink constantly

URINE COLOR GUIDE:
- Clear/pale yellow: Good!
- Dark yellow: Need water
- Orange/brown: Severely dehydrated
- No urine: Emergency

SPECIAL SITUATIONS:

DIARRHEA/VOMITING:
- Lose water rapidly
- ORS essential
- Replace what's lost
- Continuous small sips

HEAT EXHAUSTION:
- Dehydration + overheating
- Move to shade
- Cool body
- Sips of ORS
- Rest

CHILDREN/INFANTS:
- Dehydrate faster
- More vulnerable
- Watch carefully
- Wet diapers = good
- Sunken fontanel = dehydrated

ELDERLY:
- Less thirst sensation
- Remind to drink
- Monitor closely
- More vulnerable

SIGNS OF IMPROVEMENT:
- Urinating more
- Urine getting lighter
- Feeling better
- Moist mouth
- More energy
- Normal skin turgor

CRITICAL POINTS:
- Prevent rather than treat
- Drink constantly
- Monitor urine color
- ORS for illness
- Children need extra attention

SURVIVAL TIP:
- Water more important than food
- Can survive weeks without food
- Days without water
- NEVER ration water
- Drink what you have
- Find more water

WATER SOURCES:
- Streams, rivers (boil first!)
- Rain catchment
- Dew collection
- Plant transpiration
- Dig near water
- Follow animals to water

CONSERVATION:
- Stay cool
- Rest during heat
- Work in shade
- Avoid alcohol
- Avoid caffeine
- Breathe through nose

STORING WATER:
- Clean containers
- Covered
- Out of sun
- Rotate stock
- Purify before drinking
""",

    "Food_Poisoning": """FOOD POISONING - RECOGNITION & CARE

COMMON SURVIVAL THREAT

CAUSES:
- Spoiled food
- Undercooked meat
- Contaminated water
- Unwashed produce
- Cross-contamination
- Improperly stored food

SYMPTOMS:
Usually 1-6 hours after eating:
- Nausea
- Vomiting
- Diarrhea
- Abdominal cramps
- Fever
- Weakness

MILD FOOD POISONING:

TREATMENT:
- Stop eating solid food
- Small sips of water
- ORS solution
- Rest
- Let it pass naturally

WHEN TO VOMIT:
- Body's defense mechanism
- Don't fight it
- Stay hydrated after
- If poison suspected, vomit helps

RECOVERY:
- Usually 24-48 hours
- Gradually reintroduce food
- Start with bland foods

SEVERE POISONING:

DANGER SIGNS:
⚠️ SEEK HELP:
- Blood in vomit/stool
- Severe abdominal pain
- High fever (103°F+)
- Signs of dehydration
- Blurred vision
- Muscle weakness
- Difficulty breathing
- Lasts more than 3 days

REHYDRATION:
MOST IMPORTANT:
- Vomiting + diarrhea = rapid fluid loss
- Dangerous dehydration
- ORS every 15 minutes
- Small sips
- Monitor urine

FOOD REINTRODUCTION:

DAY 1:
- Clear liquids only
- Water, broth, ORS
- No solid food

DAY 2 (if improving):
- Bland foods
- Crackers
- Rice
- Toast
- Bananas
- Applesauce

DAY 3+:
- Gradually normal diet
- Avoid dairy for few days
- Avoid spicy/fatty foods
- Small portions

NATURAL REMEDIES:

ANTI-NAUSEA:
- Ginger tea (very effective)
- Peppermint tea
- Chamomile tea
- Cinnamon tea

STOMACH SETTLING:
- Activated charcoal (absorbs toxins)
- Apple cider vinegar (diluted)
- Lemon water
- Bone broth

ANTI-DIARRHEA:
- Black tea (tannins)
- Blackberry root tea
- Rice water
- Chamomile

PREVENTION:

FOOD SAFETY:
- Cook thoroughly
- Check for spoilage
- Proper storage
- Keep hot food hot
- Keep cold food cold

SIGNS OF SPOILAGE:
- Bad smell
- Off color
- Slimy texture
- Mold
- Bulging cans
- Fizzing/bubbling
- When in doubt, throw out!

COOKING TEMPS:
- Pork: 145°F
- Chicken: 165°F
- Beef: 160°F (ground)
- Fish: 145°F
- All meat well-done in survival

WATER SAFETY:
- Boil 1 minute (rolling boil)
- Filter if available
- Never drink untreated
- Assume all wild water contaminated

CROSS-CONTAMINATION:
- Separate raw and cooked
- Clean cutting boards
- Wash hands between tasks
- Don't reuse marinades
- Clean utensils

STORAGE:

WITHOUT REFRIGERATION:
- Eat fresh food first
- Dry/smoke meat
- Salt for preservation
- Avoid mayo, dairy
- Cook daily portions only

SPOILAGE TIMELINE:
(without refrigeration):
- Meat: hours to 1 day
- Fish: even faster
- Dairy: very fast
- Eggs: weeks (if uncracked)
- Hard cheese: days
- Vegetables: varies

SAFE FOODS:
- Canned goods (unopened)
- Dried foods
- Properly smoked meat
- Salted fish/meat
- Hard grains
- Honey (never spoils!)
- Dried beans

MUSHROOM POISONING:
⚠️ VERY SERIOUS:
- Can be deadly
- Symptoms delayed
- May seem better then worsen
- Liver/kidney failure
- Expert ID essential
- When in doubt, don't eat

BOTULISM:
⚠️ DEADLY:
- From improper canning
- Bulging cans
- Paralysis symptoms
- Double vision
- Can't swallow
- Medical emergency
- Avoid home-canned foods in survival

TEACHING OTHERS:
- Smell test
- Visual inspection
- When in doubt rule
- Proper cooking
- Hygiene importance

CHILDREN:
- More vulnerable
- Smaller portions = less exposure
- Watch carefully
- Rehydrate aggressively
- Seek help sooner
""",

    "Burns_Treatment": """BURNS - FIRST AID & CARE

COMMON CAMP/SURVIVAL INJURY

BURN DEGREES:

FIRST-DEGREE (Superficial):
- Red skin
- Painful
- No blisters
- Dry
- Example: mild sunburn

SECOND-DEGREE (Partial Thickness):
- Very painful
- Red, blotchy
- Blisters
- Swelling
- Moist appearance

THIRD-DEGREE (Full Thickness):
- May not hurt (nerves destroyed)
- White or charred
- Dry, leathery
- Needs medical care
- High infection risk

IMMEDIATE CARE:

1. STOP THE BURNING:
   - Remove from heat source
   - If clothing on fire: Stop, Drop, Roll
   - Remove jewelry/tight items
   - Chemical burns: flush 20 minutes

2. COOL THE BURN:
   - Cool (not ice cold) water
   - 10-20 minutes
   - Don't use ice
   - Reduces depth of burn
   - Decreases pain

3. ASSESS SEVERITY:
   - Size
   - Depth
   - Location
   - Victim's age

WHEN TO WORRY:

SEEK MEDICAL HELP:
- Third-degree burn
- Second-degree larger than 2-3 inches
- On face, hands, feet, genitals
- Over joints
- Electrical burn
- Chemical burn
- Infant or elderly victim
- Signs of infection

TREATMENT BY DEGREE:

FIRST-DEGREE:
- Cool with water
- Aloe vera gel
- Moisturize
- Cover if needed
- Heals in 3-7 days

SECOND-DEGREE:
- Cool with water
- Don't pop blisters!
- Gently clean
- Apply honey or aloe
- Cover with gauze
- Change dressing daily
- Monitor for infection
- Heals in 2-3 weeks

THIRD-DEGREE:
- Cool gently
- Cover with clean cloth
- Don't remove stuck clothing
- Elevate if possible
- Treat for shock
- Medical care essential

NATURAL REMEDIES:

HONEY:
- Proven burn treatment
- Antibacterial
- Promotes healing
- Apply thin layer
- Cover with gauze

ALOE VERA:
- Excellent for burns
- Cooling effect
- Anti-inflammatory
- Apply gel directly
- Reapply as needed

CALENDULA:
- Healing properties
- Reduces scarring
- Make into salve

PLANTAIN LEAVES:
- Crushed and applied
- Drawing properties
- Reduces pain

LAVENDER OIL:
- After initial healing
- Prevents scarring
- Dilute in carrier oil

BANDAGING:

MATERIALS:
- Clean gauze
- Non-stick pads
- Medical tape
- Loose wrapping

PROCESS:
- Clean hands
- Apply ointment/honey
- Cover with non-stick pad
- Wrap loosely with gauze
- Don't wrap too tight
- Elevate if possible

DRESSING CHANGES:
- Daily or if wet/dirty
- Gently remove old bandage
- Clean burn
- Check for infection
- Apply fresh ointment
- New bandage

INFECTION SIGNS:

WATCH FOR:
- Increased redness
- Spreading warmth
- Swelling
- Pus
- Increased pain
- Red streaks
- Fever
- Bad odor

IF INFECTED:
- Clean more frequently
- Honey application
- Monitor closely
- May need antibiotics
- Serious if unchecked

PAIN MANAGEMENT:

NATURAL OPTIONS:
- Cool compresses
- Aloe vera
- Willow bark tea (natural aspirin)
- Keep covered (prevents air contact)
- Elevation reduces throbbing
- Distraction

BLISTERS:

DO:
- Leave intact if possible
- Natural protection
- Cover to prevent rupture
- Clean if broken

DON'T:
- Pop intentionally
- Peel off skin
- Leave open to air initially

IF BLISTER POPS:
- Clean gently
- Apply antibiotic/honey
- Cover
- Watch for infection

SPECIAL BURNS:

SUNBURN:
- Cool baths
- Aloe vera
- Stay hydrated
- Loose clothing
- Prevent with coverage

CHEMICAL BURNS:
- Flush 20+ minutes
- Remove contaminated clothing
- Don't neutralize (can worsen)
- Continue flushing
- Seek medical care

ELECTRICAL BURNS:
⚠️ SERIOUS:
- External burn may be small
- Internal damage possible
- Heart rhythm issues
- Always seek medical care

HOT OIL/GREASE:
- Cool with water
- Don't use butter/oils!
- Can be deep
- High infection risk
- Clean carefully

PREVENTION:

FIRE SAFETY:
- Control flame size
- Keep water nearby
- Fire-resistant clothing
- Long sleeves near fire
- Secure pots
- Teach children

COOKING:
- Pot handles in
- No loose sleeves
- Careful with grease
- Keep children away
- Oven mitts

LONG-TERM CARE:

SCARRING:
- Vitamin E oil
- Massage gently
- Keep moisturized
- Protect from sun
- May fade over time

HEALING TIMELINE:
- First-degree: 3-7 days
- Second-degree: 2-3 weeks
- Third-degree: months

NUTRITION FOR HEALING:
- Protein (meat, eggs, beans)
- Vitamin C (citrus)
- Vitamin E
- Zinc
- Stay hydrated
- Adequate calories
""",

    "Hypothermia_Treatment": """HYPOTHERMIA - RECOGNITION & TREATMENT

DEADLY COLD WEATHER THREAT

WHAT IT IS:
- Body temperature drops below 95°F
- Normal is 98.6°F
- Can be fatal
- Even in mild weather (40-50°F)

RISK FACTORS:
- Wet clothing
- Wind (wind chill)
- Exhaustion
- Lack of food
- Inadequate clothing
- Elderly or young
- Alcohol consumption

STAGES:

MILD (95-97°F):
- Shivering (intense)
- Cold hands/feet
- Numbness
- Clumsiness
- Difficulty with tasks
- Increased breathing/heart rate

MODERATE (90-95°F):
- Violent shivering or stops
- Confusion
- Drowsiness
- Slurred speech
- Stumbling
- Irrational behavior
- Poor judgment

SEVERE (Below 90°F):
- No shivering
- Blue skin
- Dilated pupils
- Very slow breathing
- Weak pulse
- Unconsciousness
- Appears dead
- Can still be saved!

TREATMENT:

IMMEDIATE ACTION:
1. Get to shelter
2. Remove wet clothing
3. Insulate from ground
4. Add dry layers
5. Cover head (major heat loss)
6. Warm gradually

MILD HYPOTHERMIA:
- Active rewarming okay
- Warm drinks (not alcohol!)
- Light exercise if able
- Huddle with others
- Warm, sweet drinks
- High-calorie food

MODERATE HYPOTHERMIA:
- Passive rewarming
- Insulation only
- Warm drinks if conscious
- No vigorous activity
- Handle gently
- Monitor closely

SEVERE HYPOTHERMIA:
⚠️ CRITICAL:
- Handle extremely gently
- Horizontal position
- Core warming only
- Warm drinks if conscious/able
- Don't rub extremities
- Medical emergency
- May seem dead but savable

WARMING METHODS:

BODY HEAT:
- Skin-to-skin contact
- In sleeping bag together
- Focus on core (chest/neck)
- Very effective
- Two warm people better

WARM LIQUIDS:
- Hot tea/coffee (not alcohol!)
- Hot cocoa
- Warm soup/broth
- High sugar content helps
- Sip slowly
- Only if conscious

FIRE/HEAT:
- Gradual warming
- Don't get too close
- Risk of burns (numbness)
- Warm core first
- Rotate to warm evenly

INSULATION:
- Sleeping bag
- Blankets
- Emergency blanket
- Dry clothing
- Cover head
- Insulate from ground

WHAT NOT TO DO:

NEVER:
- Give alcohol (worsens)
- Warm too quickly
- Apply heat to extremities first
- Rub or massage
- Give unconscious person liquids
- Assume they're dead

WHY NOT EXTREMITIES FIRST:
- Cold blood rushes to core
- Can cause heart attack
- "After-drop" phenomenon
- Core first, always

PREVENTION:

CLOTHING:
- Layer system
- Base: moisture-wicking
- Mid: insulation
- Outer: windproof/waterproof
- Cotton kills (stays wet)
- Wool or synthetic

STAY DRY:
- Waterproof outer layer
- Change wet clothes immediately
- Avoid sweating
- Ventilate when active
- Dry clothes by fire

HEAD COVERAGE:
- Lose 40% heat through head
- Hat essential
- Balaclava in severe cold
- Keep ears covered

FOOD & WATER:
- Eat regularly
- High-calorie foods
- Fats and proteins
- Stay hydrated
- Body burns more calories in cold

RECOGNITION:
- "Umbles": Mumbles, Stumbles, Grumbles
- Confusion
- Poor coordination
- Stop and warm up

SHELTERS:

EMERGENCY:
- Snow cave (if snow deep)
- Debris hut
- Lean-to with reflector
- Get out of wind
- Insulate from ground

SLEEPING:
- Insulated pad essential
- Fill gaps in sleeping bag
- Multiple people share warmth
- Fire if safe
- Ventilation important

CHILDREN:
- Cool faster
- More vulnerable
- May not complain
- Check frequently
- Extra layers

ELDERLY:
- Less able to shiver
- Medications may affect
- Monitor closely
- More layers needed

RECOVERY:
- Continue warming
- Monitor for hours
- Give food
- Keep dry
- Rest
- Gradual return to activity

COLD WATER:
⚠️ VERY DANGEROUS:
- Hypothermia in minutes
- Even 50°F water deadly
- Get out immediately
- Remove wet clothes
- Dry and warm quickly

SURVIVAL TIP:
If someone appears dead from cold,
continue warming - may revive!
"Not dead until warm and dead"

WINTER SURVIVAL PRIORITIES:
1. Shelter from wind
2. Insulation
3. Dry clothing
4. Caloric intake
5. Stay hydrated
""",

    "Hypothermia_Treatment": """HYPOTHERMIA - RECOGNITION & TREATMENT

DEADLY COLD WEATHER THREAT

WHAT IT IS:
- Body temperature drops below 95°F
- Normal is 98.6°F
- Can be fatal
- Even in mild weather (40-50°F)

RISK FACTORS:
- Wet clothing
- Wind (wind chill)
- Exhaustion
- Lack of food
- Inadequate clothing
- Elderly or young
- Alcohol consumption

STAGES:

MILD (95-97°F):
- Shivering (intense)
- Cold hands/feet
- Numbness
- Clumsiness
- Difficulty with tasks
- Increased breathing/heart rate

MODERATE (90-95°F):
- Violent shivering or stops
- Confusion
- Drowsiness
- Slurred speech
- Stumbling
- Irrational behavior
- Poor judgment

SEVERE (Below 90°F):
- No shivering
- Blue skin
- Dilated pupils
- Very slow breathing
- Weak pulse
- Unconsciousness
- Appears dead
- Can still be saved!

TREATMENT:

IMMEDIATE ACTION:
1. Get to shelter
2. Remove wet clothing
3. Insulate from ground
4. Add dry layers
5. Cover head (major heat loss)
6. Warm gradually

MILD HYPOTHERMIA:
- Active rewarming okay
- Warm drinks (not alcohol!)
- Light exercise if able
- Huddle with others
- Warm, sweet drinks
- High-calorie food

MODERATE HYPOTHERMIA:
- Passive rewarming
- Insulation only
- Warm drinks if conscious
- No vigorous activity
- Handle gently
- Monitor closely

SEVERE HYPOTHERMIA:
⚠️ CRITICAL:
- Handle extremely gently
- Horizontal position
- Core warming only
- Warm drinks if conscious/able
- Don't rub extremities
- Medical emergency
- May seem dead but savable

WARMING METHODS:

BODY HEAT:
- Skin-to-skin contact
- In sleeping bag together
- Focus on core (chest/neck)
- Very effective
- Two warm people better

WARM LIQUIDS:
- Hot tea/coffee (not alcohol!)
- Hot cocoa
- Warm soup/broth
- High sugar content helps
- Sip slowly
- Only if conscious

FIRE/HEAT:
- Gradual warming
- Don't get too close
- Risk of burns (numbness)
- Warm core first
- Rotate to warm evenly

INSULATION:
- Sleeping bag
- Blankets
- Emergency blanket
- Dry clothing
- Cover head
- Insulate from ground

WHAT NOT TO DO:

NEVER:
- Give alcohol (worsens)
- Warm too quickly
- Apply heat to extremities first
- Rub or massage
- Give unconscious person liquids
- Assume they're dead

WHY NOT EXTREMITIES FIRST:
- Cold blood rushes to core
- Can cause heart attack
- "After-drop" phenomenon
- Core first, always

PREVENTION:

CLOTHING:
- Layer system
- Base: moisture-wicking
- Mid: insulation
- Outer: windproof/waterproof
- Cotton kills (stays wet)
- Wool or synthetic

STAY DRY:
- Waterproof outer layer
- Change wet clothes immediately
- Avoid sweating
- Ventilate when active
- Dry clothes by fire

HEAD COVERAGE:
- Lose 40% heat through head
- Hat essential
- Balaclava in severe cold
- Keep ears covered

FOOD & WATER:
- Eat regularly
- High-calorie foods
- Fats and proteins
- Stay hydrated
- Body burns more calories in cold

RECOGNITION:
- "Umbles": Mumbles, Stumbles, Grumbles
- Confusion
- Poor coordination
- Stop and warm up

SHELTERS:

EMERGENCY:
- Snow cave (if snow deep)
- Debris hut
- Lean-to with reflector
- Get out of wind
- Insulate from ground

SLEEPING:
- Insulated pad essential
- Fill gaps in sleeping bag
- Multiple people share warmth
- Fire if safe
- Ventilation important

CHILDREN:
- Cool faster
- More vulnerable
- May not complain
- Check frequently
- Extra layers

ELDERLY:
- Less able to shiver
- Medications may affect
- Monitor closely
- More layers needed

RECOVERY:
- Continue warming
- Monitor for hours
- Give food
- Keep dry
- Rest
- Gradual return to activity

COLD WATER:
⚠️ VERY DANGEROUS:
- Hypothermia in minutes
- Even 50°F water deadly
- Get out immediately
- Remove wet clothes
- Dry and warm quickly

SURVIVAL TIP:
If someone appears dead from cold,
continue warming - may revive!
"Not dead until warm and dead"

WINTER SURVIVAL PRIORITIES:
1. Shelter from wind
2. Insulation
3. Dry clothing
4. Caloric intake
5. Stay hydrated
""",

}

for filename, content in medical_guides.items():
    filepath = os.path.join(medical_dir, f"{filename}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))

# Create index
index_file = os.path.join(medical_dir, "index.txt")
with open(index_file, 'w', encoding='utf-8') as f:
    for i, name in enumerate(medical_guides.keys()):
        guide_name = name.replace('_', ' ')
        f.write(f"{guide_name}|{name}.txt|0|Medical survival guide\n")

print(f"✓ Created {len(medical_guides)} medical survival guides")
print(f"✓ Saved to /medical/")

print("\n" + "=" * 60)
print("✓ MEDICAL EXPANSION COMPLETE!")
print("=" * 60)
