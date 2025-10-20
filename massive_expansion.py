#!/usr/bin/env python3
"""
The Book - MASSIVE EXPANSION SCRIPT
Target: 1-3 GB of offline survival knowledge
"""

import os
import textwrap

def wrap_text(text, width=39):
    lines = []
    for paragraph in text.split('\n'):
        if paragraph.strip():
            wrapped = textwrap.fill(paragraph, width=width, break_long_words=False, break_on_hyphens=False)
            lines.extend(wrapped.split('\n'))
        else:
            lines.append('')
    return '\n'.join(lines)

base_dir = 'the_book'

# Create directories
prog_dir = os.path.join(base_dir, 'programming_languages')
survival_dir = os.path.join(base_dir, 'survival')
diy_dir = os.path.join(base_dir, 'diy_technical')
agri_dir = os.path.join(base_dir, 'agriculture')

for d in [diy_dir, agri_dir]:
    os.makedirs(d, exist_ok=True)

print("=" * 60)
print("MASSIVE BOOK EXPANSION")
print("=" * 60)

# ============================================================
# COMPLETE PROGRAMMING LANGUAGES (11 more)
# ============================================================
print("\n1. Adding remaining programming languages...")

remaining_langs = {
    "Swift": """SWIFT PROGRAMMING GUIDE

COMPREHENSIVE SWIFT REFERENCE

OVERVIEW:
Swift is a powerful programming language developed by Apple for iOS, macOS, watchOS, and tvOS development. Known for safety, performance, and modern syntax.

BASIC SYNTAX:

HELLO WORLD:
```swift
print("Hello World!")
```

VARIABLES AND CONSTANTS:
```swift
var age = 25           // Variable
let name = "John"      // Constant
var price: Double = 19.99
var isActive: Bool = true
```

TYPE INFERENCE:
Swift automatically infers types, but you can specify them:
```swift
var message = "Hello"  // Inferred as String
var count: Int = 0     // Explicit type
```

FUNCTIONS:
```swift
func greet(name: String) -> String {
    return "Hello, \\(name)!"
}

func add(a: Int, b: Int) -> Int {
    return a + b
}

// Calling
let result = greet(name: "World")
```

CLOSURES:
```swift
let numbers = [1, 2, 3, 4, 5]
let doubled = numbers.map { $0 * 2 }
let evens = numbers.filter { $0 % 2 == 0 }
```

CLASSES AND STRUCTS:
```swift
// Class (reference type)
class Person {
    var name: String
    var age: Int

    init(name: String, age: Int) {
        self.name = name
        self.age = age
    }

    func display() {
        print("\\(name): \\(age)")
    }
}

// Struct (value type)
struct Point {
    var x: Double
    var y: Double
}

// Usage
let person = Person(name: "John", age: 30)
person.display()
```

OPTIONALS:
```swift
var optionalName: String? = "John"
var optionalAge: String? = nil

// Unwrapping
if let name = optionalName {
    print(name)
}

// Nil coalescing
let greeting = optionalName ?? "Guest"
```

ARRAYS:
```swift
var numbers = [1, 2, 3, 4, 5]
numbers.append(6)
numbers.remove(at: 0)

for num in numbers {
    print(num)
}

// Map, filter, reduce
let doubled = numbers.map { $0 * 2 }
let evens = numbers.filter { $0 % 2 == 0 }
let sum = numbers.reduce(0, +)
```

DICTIONARIES:
```swift
var ages = ["Alice": 25, "Bob": 30]
ages["Charlie"] = 35

for (name, age) in ages {
    print("\\(name): \\(age)")
}
```

CONTROL FLOW:
```swift
// If statement
if x > 10 {
    print("Greater")
} else if x > 5 {
    print("Medium")
} else {
    print("Small")
}

// Switch
switch value {
case 1:
    print("One")
case 2:
    print("Two")
default:
    print("Other")
}

// For loop
for i in 0..<10 {
    print(i)
}

// While
while condition {
    // code
}
```

ENUMERATIONS:
```swift
enum Direction {
    case north
    case south
    case east
    case west
}

let heading = Direction.north
```

PROTOCOLS:
```swift
protocol Drawable {
    func draw()
}

class Circle: Drawable {
    func draw() {
        print("Drawing circle")
    }
}
```

ERROR HANDLING:
```swift
enum FileError: Error {
    case notFound
    case permissionDenied
}

func readFile() throws -> String {
    throw FileError.notFound
}

do {
    let content = try readFile()
} catch FileError.notFound {
    print("File not found")
} catch {
    print("Error: \\(error)")
}
```

COMMON USE CASES:
- iOS app development
- macOS applications
- watchOS apps
- tvOS applications
- Server-side Swift

BEST PRACTICES:
- Use let for constants
- Prefer value types (structs)
- Use optionals safely
- Follow Swift naming conventions
- Use guard for early exits
- Leverage protocol-oriented programming
""",

    "Kotlin": """KOTLIN PROGRAMMING GUIDE

COMPREHENSIVE KOTLIN REFERENCE

OVERVIEW:
Kotlin is a modern programming language that runs on the JVM, developed by JetBrains. Fully interoperable with Java, used for Android development.

BASIC SYNTAX:

HELLO WORLD:
```kotlin
fun main() {
    println("Hello World!")
}
```

VARIABLES:
```kotlin
val name = "John"      // Immutable (val)
var age = 25           // Mutable (var)
val price: Double = 19.99
var isActive: Boolean = true
```

FUNCTIONS:
```kotlin
fun greet(name: String): String {
    return "Hello, $name!"
}

fun add(a: Int, b: Int): Int {
    return a + b
}

// Single expression
fun multiply(a: Int, b: Int) = a * b

// Default parameters
fun greet(name: String = "Guest") {
    println("Hello, $name")
}
```

CLASSES:
```kotlin
class Person(val name: String, var age: Int) {
    fun display() {
        println("$name: $age")
    }
}

// Data class
data class User(val id: Int, val name: String)

// Usage
val person = Person("John", 30)
person.display()
```

NULL SAFETY:
```kotlin
var name: String = "John"      // Cannot be null
var nullableName: String? = null  // Can be null

// Safe call
val length = nullableName?.length

// Elvis operator
val len = nullableName?.length ?: 0

// Non-null assertion
val len2 = nullableName!!.length  // Throws if null
```

COLLECTIONS:
```kotlin
// Lists
val numbers = listOf(1, 2, 3, 4, 5)
val mutableList = mutableListOf(1, 2, 3)
mutableList.add(4)

// Maps
val ages = mapOf("Alice" to 25, "Bob" to 30)
val mutableMap = mutableMapOf<String, Int>()
mutableMap["Charlie"] = 35

// Sets
val uniqueNumbers = setOf(1, 2, 3, 3, 4)
```

CONTROL FLOW:
```kotlin
// If expression
val max = if (a > b) a else b

// When (switch)
when (x) {
    1 -> println("One")
    2 -> println("Two")
    in 3..10 -> println("3 to 10")
    else -> println("Other")
}

// For loop
for (i in 0..10) {
    println(i)
}

for (item in collection) {
    println(item)
}

// While
while (condition) {
    // code
}
```

LAMBDA EXPRESSIONS:
```kotlin
val numbers = listOf(1, 2, 3, 4, 5)
val doubled = numbers.map { it * 2 }
val evens = numbers.filter { it % 2 == 0 }
val sum = numbers.reduce { acc, n -> acc + n }
```

EXTENSION FUNCTIONS:
```kotlin
fun String.addExclamation(): String {
    return this + "!"
}

val greeting = "Hello".addExclamation()
```

OBJECT AND COMPANION OBJECT:
```kotlin
object Singleton {
    fun doSomething() {
        println("Singleton")
    }
}

class MyClass {
    companion object {
        fun create(): MyClass = MyClass()
    }
}
```

COROUTINES:
```kotlin
import kotlinx.coroutines.*

fun main() = runBlocking {
    launch {
        delay(1000L)
        println("World!")
    }
    println("Hello,")
}
```

COMMON USE CASES:
- Android app development
- Server-side applications
- Multiplatform mobile development
- Web development
- Data science

BEST PRACTICES:
- Prefer val over var
- Use data classes for DTOs
- Leverage null safety
- Use extension functions
- Follow Kotlin conventions
- Use coroutines for async
""",

    "PHP": """PHP PROGRAMMING GUIDE

COMPREHENSIVE PHP REFERENCE

OVERVIEW:
PHP is a server-side scripting language designed for web development. Powers millions of websites including WordPress, Facebook, and Wikipedia.

BASIC SYNTAX:

HELLO WORLD:
```php
<?php
echo "Hello World!";
?>
```

VARIABLES:
```php
<?php
$name = "John";
$age = 25;
$price = 19.99;
$isActive = true;
?>
```

FUNCTIONS:
```php
<?php
function greet($name) {
    return "Hello, $name!";
}

function add($a, $b) {
    return $a + $b;
}

echo greet("World");
?>
```

ARRAYS:
```php
<?php
// Indexed array
$numbers = [1, 2, 3, 4, 5];
$numbers[] = 6;  // Add element

// Associative array
$person = [
    "name" => "John",
    "age" => 30,
    "city" => "NYC"
];

// Accessing
echo $person["name"];

// Looping
foreach ($numbers as $num) {
    echo $num;
}

foreach ($person as $key => $value) {
    echo "$key: $value";
}
?>
```

CONTROL STRUCTURES:
```php
<?php
// If statement
if ($x > 10) {
    echo "Greater";
} elseif ($x > 5) {
    echo "Medium";
} else {
    echo "Small";
}

// Switch
switch ($value) {
    case 1:
        echo "One";
        break;
    case 2:
        echo "Two";
        break;
    default:
        echo "Other";
}

// For loop
for ($i = 0; $i < 10; $i++) {
    echo $i;
}

// While
while ($condition) {
    // code
}
?>
```

CLASSES:
```php
<?php
class Person {
    private $name;
    private $age;

    public function __construct($name, $age) {
        $this->name = $name;
        $this->age = $age;
    }

    public function display() {
        echo $this->name . ": " . $this->age;
    }
}

$person = new Person("John", 30);
$person->display();
?>
```

STRING MANIPULATION:
```php
<?php
$str = "Hello";
$str .= " World";  // Concatenation
$length = strlen($str);
$upper = strtoupper($str);
$lower = strtolower($str);
$parts = explode(" ", $str);
$joined = implode("-", $parts);
?>
```

FORM HANDLING:
```php
<?php
// GET request
$name = $_GET['name'] ?? 'Guest';

// POST request
$email = $_POST['email'] ?? '';

// Sanitization
$clean = htmlspecialchars($input);
$clean = filter_var($email, FILTER_SANITIZE_EMAIL);
?>
```

DATABASE (MySQL):
```php
<?php
// PDO (recommended)
$pdo = new PDO('mysql:host=localhost;dbname=test', 'user', 'pass');

// Query
$stmt = $pdo->query('SELECT * FROM users');
while ($row = $stmt->fetch()) {
    echo $row['name'];
}

// Prepared statement
$stmt = $pdo->prepare('SELECT * FROM users WHERE id = ?');
$stmt->execute([$userId]);
$user = $stmt->fetch();
?>
```

FILE HANDLING:
```php
<?php
// Read
$content = file_get_contents('file.txt');

// Write
file_put_contents('file.txt', 'Hello');

// Line by line
$lines = file('file.txt');
foreach ($lines as $line) {
    echo $line;
}
?>
```

SESSIONS:
```php
<?php
session_start();
$_SESSION['user'] = 'John';
$user = $_SESSION['user'];
session_destroy();
?>
```

ERROR HANDLING:
```php
<?php
try {
    // code that might throw
} catch (Exception $e) {
    echo $e->getMessage();
} finally {
    // always runs
}
?>
```

COMMON USE CASES:
- Web applications
- Content Management Systems
- REST APIs
- E-commerce platforms
- Server-side scripting

BEST PRACTICES:
- Use prepared statements
- Sanitize user input
- Use password_hash() for passwords
- Enable error reporting in dev
- Use composer for dependencies
- Follow PSR standards
""",

    "TypeScript": """TYPESCRIPT PROGRAMMING GUIDE

COMPREHENSIVE TYPESCRIPT REFERENCE

OVERVIEW:
TypeScript is a typed superset of JavaScript that compiles to plain JavaScript. Developed by Microsoft, adds static typing and modern features.

BASIC SYNTAX:

HELLO WORLD:
```typescript
console.log("Hello World!");
```

BASIC TYPES:
```typescript
let name: string = "John";
let age: number = 25;
let isActive: boolean = true;
let numbers: number[] = [1, 2, 3];
let tuple: [string, number] = ["John", 30];
```

TYPE INFERENCE:
```typescript
let message = "Hello";  // Inferred as string
let count = 0;          // Inferred as number
```

FUNCTIONS:
```typescript
function greet(name: string): string {
    return `Hello, ${name}!`;
}

function add(a: number, b: number): number {
    return a + b;
}

// Arrow function
const multiply = (a: number, b: number): number => a * b;

// Optional parameters
function greet(name: string, greeting?: string): string {
    return `${greeting || 'Hello'}, ${name}`;
}

// Default parameters
function greet(name: string, greeting: string = "Hello"): string {
    return `${greeting}, ${name}`;
}
```

INTERFACES:
```typescript
interface Person {
    name: string;
    age: number;
    email?: string;  // Optional
}

const person: Person = {
    name: "John",
    age: 30
};

// Function interface
interface MathFunc {
    (a: number, b: number): number;
}

const add: MathFunc = (a, b) => a + b;
```

CLASSES:
```typescript
class Person {
    private name: string;
    public age: number;

    constructor(name: string, age: number) {
        this.name = name;
        this.age = age;
    }

    public display(): void {
        console.log(`${this.name}: ${this.age}`);
    }
}

// Inheritance
class Employee extends Person {
    private salary: number;

    constructor(name: string, age: number, salary: number) {
        super(name, age);
        this.salary = salary;
    }
}
```

GENERICS:
```typescript
function identity<T>(arg: T): T {
    return arg;
}

let output = identity<string>("Hello");

// Generic class
class Box<T> {
    private value: T;

    constructor(value: T) {
        this.value = value;
    }

    getValue(): T {
        return this.value;
    }
}
```

UNION TYPES:
```typescript
let value: string | number;
value = "Hello";
value = 42;

function print(value: string | number): void {
    if (typeof value === "string") {
        console.log(value.toUpperCase());
    } else {
        console.log(value.toFixed(2));
    }
}
```

TYPE ALIASES:
```typescript
type ID = string | number;
type User = {
    id: ID;
    name: string;
    email: string;
};

const user: User = {
    id: 1,
    name: "John",
    email: "john@example.com"
};
```

ENUMS:
```typescript
enum Direction {
    North,
    South,
    East,
    West
}

let heading: Direction = Direction.North;

// String enum
enum Status {
    Active = "ACTIVE",
    Inactive = "INACTIVE"
}
```

TYPE GUARDS:
```typescript
function isString(value: any): value is string {
    return typeof value === "string";
}

if (isString(value)) {
    console.log(value.toUpperCase());
}
```

MODULES:
```typescript
// Export
export interface User {
    name: string;
}

export function greet(name: string): string {
    return `Hello, ${name}`;
}

// Import
import { User, greet } from './module';
```

ASYNC/AWAIT:
```typescript
async function fetchData(): Promise<string> {
    const response = await fetch('https://api.example.com');
    const data = await response.json();
    return data;
}
```

COMMON USE CASES:
- Large-scale applications
- React/Angular/Vue projects
- Node.js backends
- Library development
- Enterprise applications

COMPILATION:
```
tsc program.ts
node program.js
```

BEST PRACTICES:
- Enable strict mode
- Use interfaces for objects
- Prefer const over let
- Use type inference when possible
- Avoid 'any' type
- Use generics for reusable code
""",
}

for lang_name, content in remaining_langs.items():
    filepath = os.path.join(prog_dir, f"{lang_name}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))
    print(f"  ✓ {lang_name}")

print(f"✓ Added {len(remaining_langs)} more programming languages")

# ============================================================
# SURVIVAL GUIDES - MASSIVE EXPANSION
# ============================================================
print("\n2. Expanding survival guides...")

survival_content = {
    "Hunting_Basics": """HUNTING FOR SURVIVAL

COMPREHENSIVE HUNTING GUIDE

LEGAL NOTICE:
Always follow local hunting laws and regulations. This guide is for survival situations only.

HUNTING ETHICS:
- Take only what you need
- Use all parts of the animal
- Quick, humane kills
- Respect the animal
- Know your target

SMALL GAME HUNTING:

RABBITS & SQUIRRELS:
Best targets for beginners:
- Abundant in most areas
- Good meat-to-effort ratio
- Less dangerous
- Easier to process

HUNTING METHODS:
1. Still hunting (waiting)
2. Stalking (moving slowly)
3. Tracking
4. Calling
5. Trapping

WEAPON SELECTION:

PRIMITIVE WEAPONS:
- Slingshot (easy to make)
- Bow and arrow (requires practice)
- Spear (close range)
- Throwing stick
- Deadfall traps

MAKESHIFT SLINGSHOT:
Materials needed:
- Y-shaped branch (strong wood)
- Rubber tubing or elastic
- Leather pouch
- Cord

Construction:
1. Find strong Y-branch
2. Cut to comfortable size
3. Notch fork ends
4. Attach elastic bands
5. Attach pouch to bands
6. Practice aim

BOW AND ARROW:

MAKING A BOW:
Best woods:
- Yew (best)
- Oak
- Hickory
- Ash
- Elm

Steps:
1. Find straight branch (5-6 feet)
2. Dry for several weeks (if time)
3. Shape with knife
4. Notch ends for string
5. String with cord/sinew
6. Test gradually

ARROW MAKING:
1. Straight shoots (2 feet)
2. Straighten over fire
3. Notch end for bowstring
4. Add fletching (feathers)
5. Sharpen or add point

TRACKING ANIMALS:

READING TRACKS:
Look for:
- Footprints in soft ground
- Broken twigs/branches
- Droppings (scat)
- Feeding signs
- Bedding areas
- Trails/runs

TRACK IDENTIFICATION:
Deer: Heart-shaped, cloven hoof
Rabbit: Oval hind, round front
Squirrel: Four toes front, five back
Wild boar: Like deer but rounder

TRACKING TIPS:
- Track in early morning
- Look into sun for shadows
- Check near water
- Follow game trails
- Be patient and quiet

STALKING TECHNIQUES:

MOVEMENT:
- Move slowly
- Stop frequently
- Use cover
- Walk heel-to-toe
- Avoid stepping on sticks

WIND DIRECTION:
Always approach from downwind:
- Animals smell 100x better than humans
- Wet finger to test wind
- Use terrain to your advantage
- Change position if wind shifts

CAMOUFLAGE:
- Wear earth tones
- Break up outline
- Use mud/charcoal on face
- Stay in shadows
- Avoid silhouetting

SHOT PLACEMENT:

VITAL ZONES:
Best targets for quick kill:
- Heart (behind front leg)
- Lungs (chest cavity)
- Brain (difficult shot)
- Spine (instant drop)

AVOID:
- Gut shots (suffering)
- Limb shots (escape)
- Poor angles
- Long-distance shots

WAIT FOR:
- Broadside shot
- Animal stationary
- Clear view
- Comfortable range
- Confident shot

FIELD DRESSING:

IMMEDIATE STEPS:
1. Ensure animal is dead
2. Position on back
3. Make careful cuts
4. Remove organs
5. Cool carcass quickly
6. Protect from flies

GUTTING PROCESS:
1. Cut around anus
2. Slice belly (shallow!)
3. Avoid puncturing organs
4. Remove innards
5. Save heart, liver, kidneys
6. Wipe cavity clean

COOLING:
- Prop cavity open
- Hang if possible
- Keep in shade
- Cover with cloth
- Process within hours

BUTCHERING:

SKINNING:
1. Hang by hind legs
2. Cut around hocks
3. Slice down inner legs
4. Peel skin downward
5. Cut around head
6. Remove entire hide

MEAT SECTIONS:
- Hindquarters (best meat)
- Front shoulders
- Backstrap (tenderloin)
- Ribs
- Neck

PRESERVATION:
- Cook immediately OR
- Smoke for preservation
- Salt heavily
- Dry in strips (jerky)
- Keep cool and dry

COMMON GAME ANIMALS:

RABBIT:
- Easy to catch
- Good meat
- Found everywhere
- Active dawn/dusk
- Process quickly

SQUIRREL:
- Tree dwellers
- Active during day
- Good meat
- Easy to preserve
- Year-round availability

DEER:
- Large meat yield
- More difficult
- Requires experience
- Heavy to carry
- Process carefully

WILD BIRD:
- Waterfowl (ducks, geese)
- Upland birds (grouse, quail)
- Turkey
- Pigeons
- Easy to cook

FISHING vs HUNTING:

FISHING ADVANTAGES:
- Easier to catch
- Less energy required
- Silent (no scaring game)
- Multiple methods
- Consistent food source

HUNTING ADVANTAGES:
- More calories per catch
- No water needed
- Larger portions
- Longer preservation
- Hides for clothing

SAFETY CONSIDERATIONS:

WEAPON SAFETY:
- Know your weapon
- Practice beforehand
- Keep sharp tools secure
- Watch for ricochet
- Assume all weapons loaded

ENVIRONMENTAL HAZARDS:
- Other predators
- Rough terrain
- Weather changes
- Getting lost
- Injury while alone

DISEASE CONCERNS:
Inspect all game for:
- Unusual behavior (rabies)
- Lumps/growths (tumors)
- Parasites (worms)
- Discolored organs
- Foul odor

COOK THOROUGHLY:
- Internal temp 160°F+
- No pink meat
- Kills most parasites
- Prevents illness
- Essential for survival

PRIMITIVE TRAPS:

DEADFALL TRAP:
1. Heavy flat rock
2. Figure-4 trigger
3. Bait underneath
4. Rock falls on animal

SNARE:
1. Wire or strong cord loop
2. Place on game trail
3. Loop at head height
4. Anchored securely
5. Check frequently

PRACTICE FIRST:
- Learn in non-emergency
- Practice shooting
- Study animal behavior
- Learn tracking
- Know regulations
""",

    "Trapping_Snares": """TRAPPING AND SNARING

COMPLETE TRAPPING GUIDE

OVERVIEW:
Trapping is often more effective than hunting as traps work 24/7 while you perform other tasks. Passive food gathering.

LEGAL NOTICE:
Check local regulations. This is for survival situations only.

TRAPPING ADVANTAGES:
- Works while you sleep
- Low energy expenditure
- Multiple traps = more chances
- No weapon needed
- Silent operation
- Catches wary animals

TRAP PLACEMENT:

LOCATION SCOUTING:
Look for:
- Game trails (worn paths)
- Water sources
- Feeding areas
- Burrow entrances
- Rub marks on trees
- Fresh droppings
- Tracks in mud

BEST LOCATIONS:
- Narrow passes
- Between obstacles
- Near den entrances
- Along fences/walls
- Water crossings
- Food sources

SIGNS OF ACTIVITY:
- Fresh tracks
- Warm droppings
- Recently eaten plants
- Disturbed ground
- Hair on branches
- Active burrows

BASIC SNARE:

SIMPLE LOOP SNARE:
Materials:
- Wire (24-gauge brass) OR
- Strong cord/fishing line
- Anchor point

Construction:
1. Make small loop at one end
2. Thread other end through loop
3. Create 6-8 inch circle
4. Secure to anchor
5. Position on trail

SIZING:
Rabbit: 4 inch diameter
Squirrel: 3 inch diameter
Larger game: 8-12 inches

HEIGHT:
- 1 fist above ground (rabbit)
- Head height for animal
- Test with stick first

DEADFALL TRAPS:

FIGURE-4 DEADFALL:
Components:
- Vertical stick (notched)
- Horizontal stick (notched)
- Diagonal stick (trigger)
- Heavy flat rock/log

Assembly:
1. Cut notches in sticks
2. Fit together in figure-4
3. Balance rock on horizontal
4. Bait on trigger
5. Animal triggers = rock falls

PAIUTE DEADFALL:
More sensitive trigger:
- Uses cordage
- Hair-trigger action
- Easier to set
- Catches small game

MATERIALS:
- Y-shaped stake
- Short toggle stick
- Bait stick
- Cordage
- Heavy rock

SPRING SNARES:

CONCEPT:
Uses bent sapling to:
- Snap animal upward
- Breaks neck (humane)
- Keeps off ground (safe from scavengers)
- Easy to spot from distance

CONSTRUCTION:
1. Find flexible sapling
2. Bend down to trail
3. Attach trigger mechanism
4. Set snare loop
5. Bait or position on trail

TRIGGER TYPES:
- Toggle trigger
- Hook trigger
- Figure-4 variant
- Commercial triggers (if available)

PIT TRAP:

FOR LARGER ANIMALS:
1. Dig deep pit (6+ feet)
2. Make walls vertical
3. Sharpen stakes (optional)
4. Cover with light material
5. Camouflage with debris

WARNING:
Dangerous to humans too!
- Mark trap location
- Remember where set
- Check frequently
- Fill in when done

BOTTLE/CONTAINER TRAP:

FOR SMALL ANIMALS:
Works for:
- Mice
- Rats
- Small fish
- Insects

Method:
1. Plastic bottle/container
2. Cut hole in side
3. Bait inside
4. Easy entry, hard exit
5. Check daily

FISH TRAP:

BASKET WEAVE TRAP:
1. Weave cone from sticks
2. Narrow end inward
3. Place in stream
4. Bait inside
5. Fish enter, can't exit

TIDAL TRAP:
At ocean:
1. Build rock semicircle
2. Open end faces sea
3. High tide fills
4. Low tide traps fish

BAIT SELECTION:

FOR CARNIVORES:
- Meat scraps
- Fish
- Entrails
- Eggs
- Blood

FOR HERBIVORES:
- Fresh greens
- Vegetables
- Seeds
- Nuts
- Fruit
- Salt

FOR OMNIVORES:
- Peanut butter
- Berries
- Grains
- Insects
- Various foods

SCENT LURES:
- Urine (from prey species)
- Gland scents
- Fish oil
- Rotted meat
- Anise oil

TRAP MAINTENANCE:

CHECKING SCHEDULE:
Check traps:
- Morning (early)
- Evening (before dark)
- Minimum twice daily
- More often in warm weather

WHY FREQUENT CHECKS:
- Prevent suffering
- Stop scavenger theft
- Meat stays fresh
- Reset triggered traps
- Adjust ineffective sets

RESETTING:
- Clean away human scent
- Replace disturbed material
- Adjust trigger sensitivity
- Move if no success after 2 days
- Try different bait

CAMOUFLAGE:

NATURAL APPEARANCE:
- Use local materials
- Don't disturb area much
- Cover shiny wire
- Match surroundings
- Minimize human scent

SCENT CONTROL:
- Wash hands with ash
- Boil wire/cord
- Don't smoke near traps
- Wear gloves
- Use dead vegetation

PROCESSING CATCH:

HUMANE DISPATCH:
If animal alive:
- Quick neck break OR
- Heavy blow to head
- Minimize suffering
- Be prepared mentally

IMMEDIATE FIELD DRESSING:
1. Bleed animal
2. Remove entrails
3. Cool quickly
4. Protect from flies
5. Return to camp

SAVE USEFUL PARTS:
- Hide (clothing, cordage)
- Sinew (thread, bowstring)
- Bones (tools, needles)
- Organs (some edible)
- Fat (waterproofing, fuel)

MULTI-TRAP SYSTEM:

TRAPLINE:
Set multiple traps:
- 10-20 traps = better odds
- Check all daily
- Near camp (easy access)
- Different trap types
- Various locations

RECORD KEEPING:
Remember:
- Which traps work
- Successful locations
- Best bait
- Animal patterns
- Seasonal changes

EFFICIENCY:
- Trap while doing other tasks
- Sleep while traps work
- More reliable than hunting
- Less energy spent
- Consistent food source

SAFETY WARNINGS:

DANGEROUS TRAPS:
- Spring snares can injure humans
- Deadfalls can crush
- Pit traps are hazards
- Always mark locations
- Warn others in group

LEGAL CONCERNS:
- Illegal in most places (non-emergency)
- Property rights
- Protected species
- Trap regulations
- Hunting seasons

CHILDREN/PETS:
- Keep away from trap areas
- Explain dangers
- Supervise closely
- Deactivate when not needed

IMPROVISED MATERIALS:

CORDAGE:
- Shoelaces
- Belt
- Clothing strips
- Plant fibers
- Animal sinew

WIRE SUBSTITUTES:
- Braided fishing line
- Strong natural fiber
- Metal from equipment
- Twisted wire
- Cable

TRIGGER MECHANISMS:
- Carved from wood
- Bone pieces
- Metal bits
- Practice carving

SEASONAL CONSIDERATIONS:

WINTER:
- Animals more desperate
- Follow tracks in snow
- Set near dens
- Food scarce = better bait response

SUMMER:
- More food available
- Animals less desperate
- Check traps more often (heat)
- More scent attractants needed

YEAR-ROUND:
- Adjust to animal behavior
- Different species active
- Migrate to food sources
- Adapt techniques

PRACTICE:

LEARN BEFORE EMERGENCY:
- Build traps at home
- Study mechanisms
- Test triggers
- Learn knots
- Practice placement

ETHICS:
- Quick kill preferred
- Minimize suffering
- Use all parts
- Respect wildlife
- Gratitude for sacrifice
""",

    "Knot_Tying_Essential": """ESSENTIAL SURVIVAL KNOTS

COMPLETE KNOT GUIDE

OVERVIEW:
Knowing key knots can save your life. From shelter building to rescue situations, proper knots are essential survival skills.

WHY KNOTS MATTER:
- Secure shelter
- Climb safely
- Catch food
- Rescue operations
- Build tools
- Cross rivers
- Bind loads

KNOT TERMINOLOGY:

PARTS OF A ROPE:
- Standing end (main part)
- Working end (free end)
- Bight (U-shaped bend)
- Loop (circle in rope)

ROPE CARE:
- Keep dry when possible
- Avoid sharp edges
- Don't step on rope
- Inspect regularly
- Store coiled
- Protect from sun

ESSENTIAL KNOTS:

SQUARE KNOT (REEF KNOT):
Purpose: Join two ropes of equal diameter

Tying:
1. Right over left, twist
2. Left over right, twist
3. "Right over left, left over right"

Uses:
- Bandages
- Packages
- Light loads
- Temporary bindings

WARNING: Not for critical loads!

BOWLINE:
Purpose: Non-slip loop at rope end

Tying:
1. Make small loop
2. "Rabbit comes out of hole"
3. "Goes around tree"
4. "Goes back down hole"

Uses:
- Rescue loops
- Secure tie point
- Won't slip
- Easy to untie

CLOVE HITCH:
Purpose: Attach rope to pole/tree

Tying:
1. Wrap around pole
2. Cross over
3. Wrap again
4. Tuck under second wrap

Uses:
- Start of lashing
- Temporary tie
- Quick attachment
- Adjustable

TAUT-LINE HITCH:
Purpose: Adjustable loop (tent lines)

Tying:
1. Wrap around pole
2. Two loops inside main loop
3. One loop outside
4. Pull tight

Uses:
- Guy lines
- Adjustable tension
- Tent ropes
- Tarp corners

PRUSIK KNOT:
Purpose: Grip rope when loaded

Tying:
1. Make loop with thin cord
2. Wrap around main rope 3 times
3. Thread through own loop

Uses:
- Climbing rope
- Gripping device
- Ascending rope
- Safety backup

FIGURE-8 LOOP:
Purpose: Strong fixed loop

Tying:
1. Make figure-8 shape
2. Follow path back through
3. Tighten carefully

Uses:
- Climbing
- Rescue
- Critical loads
- Won't slip

TWO HALF HITCHES:
Purpose: Secure to pole/ring

Tying:
1. Around object
2. Half hitch around standing part
3. Second half hitch
4. Pull tight

Uses:
- Guy lines
- Tie to tree
- Anchoring
- General purpose

SHEET BEND:
Purpose: Join ropes of different sizes

Tying:
1. Bight in larger rope
2. Thread smaller through bight
3. Around both parts of bight
4. Tuck under itself

Uses:
- Different diameter ropes
- More secure than square knot
- Joining materials
- Net making

TIMBER HITCH:
Purpose: Drag logs/poles

Tying:
1. Loop around object
2. Wrap working end around itself
3. 3-4 twists
4. Pull tight

Uses:
- Dragging wood
- Temporary attachment
- Easy to untie
- Load bearing

TRUCKER'S HITCH:
Purpose: Extreme tension/mechanical advantage

Tying:
1. Fixed loop partway along rope
2. Thread working end through anchor
3. Back through loop
4. Pull creates 3:1 advantage

Uses:
- Securing loads
- Extreme tension
- Tarp tie-down
- Mechanical advantage

LASHING:

SQUARE LASHING:
Purpose: Join poles at 90°

Method:
1. Clove hitch on vertical pole
2. Wrap around both poles (square pattern)
3. Frapping (between poles)
4. Clove hitch to finish

Uses:
- Shelter frames
- Scaffolding
- Furniture
- Rafts

DIAGONAL LASHING:
Purpose: Join poles at angle

Method:
1. Timber hitch around both
2. Wrap in X pattern
3. Frapping between
4. Clove hitch finish

Uses:
- Bracing
- Strengthening joints
- Tripods
- Shear legs

ROUND LASHING:
Purpose: Extend pole length

Method:
1. Overlap poles
2. Clove hitch on one
3. Wrap tightly around both
4. Many wraps
5. Clove hitch finish

Uses:
- Lengthening poles
- Repairs
- Splicing
- Extension

FISHING KNOTS:

IMPROVED CLINCH:
Purpose: Hook to line

Tying:
1. Through hook eye
2. 5-7 wraps
3. Back through first loop
4. Through large loop
5. Wet and tighten

Uses:
- Attaching hooks
- Lures
- Swivels
- Terminal tackle

PALOMAR KNOT:
Purpose: Very strong hook attachment

Tying:
1. Double line through eye
2. Overhand knot
3. Pass hook through loop
4. Tighten

Uses:
- Maximum strength
- Braided line
- Critical connections

BLOOD KNOT:
Purpose: Join fishing lines

Tying:
1. Overlap lines
2. Wrap each around other
3. 5 wraps each direction
4. Thread ends through center
5. Pull tight

Uses:
- Fly fishing leaders
- Line repair
- Different line weights

SURVIVAL APPLICATIONS:

SHELTER BUILDING:
Use:
- Taut-line for guy lines
- Square lashing for frame
- Clove hitch for ridgeline
- Bowline for secure points

CLIMBING:
Use:
- Figure-8 for harness
- Prusik for ascending
- Bowline for safety
- Double fisherman's for joining

WATER CROSSING:
Use:
- Bowline around waist
- Trucker's hitch for tension
- Two half hitches to trees
- Safety line

RESCUE:
Use:
- Bowline rescue loop
- Figure-8 for lowering
- Prusik for brake
- Square lashing for stretcher

PRACTICE EXERCISES:

MUSCLE MEMORY:
Practice each knot:
- 100 times minimum
- Eyes closed
- One hand only
- In the dark
- Under stress

TIMED PRACTICE:
- Speed builds under pressure
- Accuracy first
- Then increase speed
- Emergency situations = stress

TEST KNOTS:
- Pull hard
- Inspect for proper form
- Retie if wrong
- Feel of correct knot

COMMON MISTAKES:

WRONG KNOT FOR JOB:
- Square knot for critical load (NO!)
- Granny knot instead of square
- Insufficient wraps
- Wrong direction

IMPROPER TYING:
- Loose knots slip
- Wrong finishing
- Skipped steps
- Reversed directions

ROPE DAMAGE:
- Sharp edges cut rope
- UV degrades
- Chemicals weaken
- Old rope fails

KNOT STRENGTH:

EFFICIENCY:
- Straight rope = 100%
- Figure-8 loop = 80%
- Bowline = 70%
- Square knot = 45%
- Sheet bend = 55%

CHOOSING KNOTS:
Match strength to need:
- Critical = figure-8
- General = bowline
- Light duty = square
- Adjustable = taut-line

CORDAGE MATERIALS:

NATURAL:
- Plant fibers
- Animal sinew
- Hide strips
- Hair/wool
- Vines

SYNTHETIC:
- Paracord (best survival)
- Nylon rope
- Poly rope
- Kevlar cord
- Wire

EMERGENCY:
- Shoelaces
- Belt
- Clothing strips
- Electrical wire
- Twisted plastic bags

REMEMBER:
"Practice makes permanent"
- Learn correct form first
- Practice regularly
- Teach others (reinforces learning)
- Carry cordage always
- Know your knots!
"""
}

for filename, content in survival_content.items():
    filepath = os.path.join(survival_dir, f"{filename}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))
    print(f"  ✓ {filename}")

print(f"✓ Added {len(survival_content)} survival guides")

# ============================================================
# DIY/TECHNICAL GUIDES
# ============================================================
print("\n3. Creating DIY/technical guides...")

diy_content = {
    "Electronics_Basics": """ELECTRONICS FUNDAMENTALS

COMPREHENSIVE ELECTRONICS GUIDE

OVERVIEW:
Understanding electronics is crucial for repairs, building devices, and survival situations with modern equipment.

BASIC CONCEPTS:

ELECTRICITY:
Flow of electrons through conductor

Key terms:
- Voltage (V): Electrical pressure
- Current (A): Flow rate
- Resistance (Ω): Opposition to flow
- Power (W): Work done

OHM'S LAW:
V = I × R

Where:
V = Voltage (volts)
I = Current (amps)
R = Resistance (ohms)

Examples:
- 12V battery with 6Ω resistor
- I = V/R = 12/6 = 2A
- Power = V × I = 12 × 2 = 24W

COMPONENTS:

RESISTORS:
Purpose: Limit current, divide voltage

Color code:
Black=0, Brown=1, Red=2, Orange=3
Yellow=4, Green=5, Blue=6
Violet=7, Gray=8, White=9

Reading:
- First two bands = value
- Third band = multiplier
- Fourth band = tolerance

CAPACITORS:
Purpose: Store electrical charge

Types:
- Ceramic (small values)
- Electrolytic (large values, polarized)
- Tantalum (stable, polarized)

Uses:
- Filtering power supply
- Timing circuits
- Coupling signals
- Bypassing noise

DIODES:
Purpose: Allow current one direction only

Types:
- Standard diode
- LED (light emitting)
- Zener (voltage reference)
- Schottky (low drop)

Marking:
- Band indicates cathode
- Current flows toward band

TRANSISTORS:
Purpose: Amplify or switch current

Types:
- NPN (most common)
- PNP (opposite polarity)
- MOSFET (high power)

Basic operation:
- Small base current controls large collector current
- Ratio = gain (hFE)

INTEGRATED CIRCUITS:
Complex circuits in package

Types:
- Logic (digital)
- Operational amplifiers (analog)
- Microcontrollers (programmable)
- Voltage regulators

Handling:
- Static sensitive!
- Ground yourself
- Proper orientation
- Correct voltage

POWER SUPPLIES:

BATTERIES:
Types:
- Alkaline (1.5V, disposable)
- NiMH (1.2V, rechargeable)
- Lithium (3.7V, rechargeable)
- Lead acid (12V, car battery)

Series connection:
- Voltages add
- Same current
- 4× 1.5V = 6V

Parallel connection:
- Same voltage
- Currents add
- 2× 1000mAh = 2000mAh

VOLTAGE REGULATION:
Purpose: Stable voltage from variable source

Linear regulators:
- 7805 = 5V output
- 7812 = 12V output
- Simple but wasteful

Switching regulators:
- More efficient
- More complex
- Handles higher power

TESTING & TROUBLESHOOTING:

MULTIMETER:
Essential tool

Measurements:
- Voltage (V)
- Current (A)
- Resistance (Ω)
- Continuity
- Diode test

Voltage measurement:
1. Set to voltage range
2. Parallel to component
3. Read display

Current measurement:
1. Set to current range
2. Break circuit
3. Insert meter in series
4. Read display

Resistance:
1. Power OFF circuit
2. Set to resistance
3. Touch probes to component
4. Read value

COMMON PROBLEMS:

NO POWER:
Check:
- Battery voltage
- Power switch
- Fuses
- Power jack connection
- Wiring continuity

INTERMITTENT:
Check:
- Loose connections
- Cracked solder joints
- Bad switches
- Corroded contacts

OVERHEATING:
Check:
- Short circuits
- Wrong component values
- Inadequate cooling
- Excessive current

SOLDERING:

EQUIPMENT:
- Soldering iron (25-40W)
- Solder (60/40 rosin core)
- Wire cutters
- Helping hands
- Desoldering tools

TECHNIQUE:
1. Clean tip
2. Heat component AND pad
3. Apply solder to joint (not iron)
4. Remove solder first
5. Remove iron
6. Don't move until cool

GOOD JOINT:
- Shiny, smooth
- Concave fillet
- Component secure
- No bridges

BAD JOINT:
- Dull, grainy
- Blobs or points
- Cold joint
- Insufficient solder

DESOLDERING:
Methods:
- Solder sucker (pump)
- Desoldering braid (wick)
- Hot air station

Process:
1. Heat joint fully
2. Remove solder
3. Lift component
4. Clean holes

CIRCUITS:

SERIES CIRCUIT:
Components in line:
- Same current through all
- Voltages add
- One failure breaks all

PARALLEL CIRCUIT:
Components side-by-side:
- Same voltage across all
- Currents add
- One failure doesn't affect others

SERIES-PARALLEL:
Combination of both:
- More complex
- Common in real circuits
- Analyze section by section

BREADBOARDING:

SOLDERLESS PROTOTYPING:
Advantages:
- No soldering
- Reusable
- Quick changes
- Learning tool

Layout:
- Holes connected in rows
- Power rails run length
- DIP chips straddle center

Tips:
- Plan layout first
- Keep wires short
- Use solid core wire
- Test sections incrementally

SAFETY:

ELECTRIC SHOCK:
Dangers:
- >30mA can kill
- AC more dangerous than DC
- Wet skin lowers resistance

Prevention:
- One hand rule
- Insulated tools
- No jewelry
- Dry hands

FIRE HAZARD:
Causes:
- Overheated components
- Short circuits
- Overloaded wiring
- Poor connections

Prevention:
- Proper fusing
- Correct wire gauge
- Good connections
- Don't exceed ratings

CHEMICAL HAZARDS:
- Lead in solder (wash hands)
- Flux fumes (ventilation)
- Battery acid (eye protection)
- Solvent vapors (fresh air)

REPAIR TECHNIQUES:

VISUAL INSPECTION:
Look for:
- Burnt components
- Bulging capacitors
- Cracked solder
- Corrosion
- Loose wires

SYSTEMATIC TESTING:
1. Power supply first
2. Verify input signals
3. Trace through circuit
4. Compare to schematic
5. Replace suspect parts

COMPONENT REPLACEMENT:
1. Identify exact part
2. Note orientation
3. Desolder carefully
4. Clean holes
5. Insert replacement
6. Solder properly
7. Trim leads
8. Test

READING SCHEMATICS:

SYMBOLS:
- Resistor: zigzag
- Capacitor: parallel lines
- Diode: triangle+line
- Transistor: circle with lines
- Battery: long/short lines
- Ground: descending lines

CONVENTIONS:
- Positive at top
- Ground at bottom
- Left to right flow
- Components labeled

UNDERSTANDING:
1. Locate power supply
2. Find input/output
3. Trace signal path
4. Identify stages
5. Component functions

SURVIVAL ELECTRONICS:

RADIO REPAIR:
Critical communication:
- Check batteries first
- Antenna connections
- Clean contacts
- Replace fuses
- Test speaker

SOLAR PANELS:
Maintain power:
- Clean surface regularly
- Check connections
- Test voltage
- Secure from wind
- Protect from damage

LED LIGHTS:
Efficient lighting:
- Low power
- Long life
- Run from batteries
- Build simple circuits

MAKESHIFT REPAIRS:
- Aluminum foil (temporary wire)
- Pencil lead (resistor)
- Steel wool (fuse)
- Battery terminals (from items)

COMPONENT TESTING:

RESISTORS:
- Measure with ohmmeter
- Compare to color code
- Out of circuit only
- Within tolerance?

CAPACITORS:
- Multimeter capacitance mode OR
- Resistance mode (should charge)
- Check for shorts
- Replace if bulging

DIODES:
- Diode test mode
- Forward: 0.6V (silicon)
- Reverse: OL (open)
- Both directions = shorted

TRANSISTORS:
- Test as two diodes
- Base-emitter: forward
- Base-collector: forward
- Collector-emitter: open

RESOURCES:

LEARNING:
- Online tutorials
- Electronics books
- YouTube videos
- Practice kits
- Community colleges

PARTS:
- Salvage old electronics
- Electronics suppliers
- Online retailers
- Local shops
- Surplus stores

TOOLS:
Start with:
- Multimeter (essential)
- Soldering iron
- Wire cutters
- Needle-nose pliers
- Screwdrivers

PRACTICE PROJECTS:
- LED flasher
- Battery tester
- Audio amplifier
- Power supply
- Radio kit
"""
}

for filename, content in diy_content.items():
    filepath = os.path.join(diy_dir, f"{filename}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))
    print(f"  ✓ {filename}")

print(f"✓ Added {len(diy_content)} DIY/technical guides")

# Continue in next part...
print("\n" + "=" * 60)
print("PHASE 1 COMPLETE - Continuing with agriculture...")
print("=" * 60)
