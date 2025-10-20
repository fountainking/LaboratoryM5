#!/usr/bin/env python3
"""
The Book - PHASE 2 EXPANSION
Agriculture + More Languages + More Survival
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
prog_dir = os.path.join(base_dir, 'programming_languages')
survival_dir = os.path.join(base_dir, 'survival')
diy_dir = os.path.join(base_dir, 'diy_technical')
agri_dir = os.path.join(base_dir, 'agriculture')

os.makedirs(agri_dir, exist_ok=True)

print("=" * 60)
print("PHASE 2 EXPANSION")
print("=" * 60)

# ============================================================
# AGRICULTURE & GARDENING
# ============================================================
print("\n4. Creating agriculture & gardening guides...")

agriculture = {
    "Vegetable_Gardening": """VEGETABLE GARDENING GUIDE

COMPLETE FOOD PRODUCTION REFERENCE

OVERVIEW:
Growing your own food provides nutrition, self-sufficiency, and survival security. Even small spaces can produce significant yields.

SITE SELECTION:

SUNLIGHT:
Most vegetables need:
- Full sun (6-8 hours minimum)
- South-facing (Northern Hemisphere)
- Away from shade trees
- Open, not enclosed

WATER ACCESS:
- Near water source
- Can irrigate easily
- Good drainage (not swampy)
- Slight slope ideal

SOIL QUALITY:
Test for:
- pH 6.0-7.0 (most vegetables)
- Good drainage
- Dark, rich color
- Earthworm presence
- Crumbly texture

SOIL PREPARATION:

CLEARING:
1. Remove grass/weeds
2. Till or dig deeply
3. Remove rocks/debris
4. Break up clods
5. Level surface

AMENDING:
Add organic matter:
- Compost (best)
- Aged manure
- Peat moss
- Leaf mold
- Cover crop residue

RATIO:
Mix 2-4 inches of organic matter into top 6-8 inches of soil

RAISED BEDS:
Advantages:
- Better drainage
- Warmer soil
- Less bending
- Control soil quality
- Fewer weeds

Construction:
- 4 feet wide (reach center)
- Any length
- 6-12 inches tall
- Wood/stone/metal sides

SEED STARTING:

DIRECT SEEDING:
Plant directly in garden:
- Beans
- Corn
- Carrots
- Radishes
- Peas
- Squash
- Cucumbers

INDOOR STARTING:
Start indoors, transplant:
- Tomatoes
- Peppers
- Eggplant
- Cabbage
- Broccoli
- Lettuce

SEED DEPTH:
General rule:
Plant 2-3× seed diameter deep

SPACING:
Follow seed packet instructions
Don't overcrowd!

ESSENTIAL VEGETABLES:

TOMATOES:
- High yield
- Long season
- Stake or cage
- Warm season
- Water consistently

Growing tips:
- Plant deep (bury stem)
- Pinch suckers
- Mulch heavily
- Full sun required

LETTUCE:
- Quick growing
- Cool season
- Succession plant
- Partial shade OK
- Cut and come again

Growing tips:
- Plant every 2 weeks
- Harvest outer leaves
- Bolt in heat
- Grow spring/fall

BEANS:
- Easy to grow
- Bush or pole
- Nitrogen fixing
- Warm season
- Direct seed

Growing tips:
- Plant after frost
- Don't overwater
- Pick frequently
- Support pole types

CARROTS:
- Cool season
- Direct seed
- Loose soil needed
- Long storage
- Fall harvest best

Growing tips:
- Thin seedlings
- Keep moist
- Mulch for storage
- No fresh manure

SQUASH:
- High yielding
- Space intensive
- Warm season
- Summer/winter types
- Good storage (winter)

Growing tips:
- Hills or mounds
- Pollination critical
- Watch for borers
- Cure winter types

PEPPERS:
- Warm season
- Long season
- Productive
- Various heat levels
- Good fresh or dried

Growing tips:
- Start early indoors
- Transplant after frost
- Stake for support
- Consistent watering

WATERING:

AMOUNT:
Most vegetables need:
- 1-2 inches per week
- Deep watering better than frequent light
- Morning watering best
- Evening OK, avoid midday

METHODS:
Drip irrigation:
- Most efficient
- Reduces disease
- Conserves water
- Can automate

Soaker hoses:
- Easy setup
- Even distribution
- Leave in place
- Cover with mulch

Hand watering:
- Control amount
- Time intensive
- Good for small gardens
- Water base, not leaves

SIGNS OF WATER STRESS:
Too little:
- Wilting
- Slow growth
- Fruit drop
- Leaf curl

Too much:
- Yellow leaves
- Fungal disease
- Slow growth
- Root rot

FERTILIZING:

ORGANIC OPTIONS:
- Compost (all-purpose)
- Manure (aged only!)
- Fish emulsion (nitrogen)
- Bone meal (phosphorus)
- Wood ash (potassium)

TIMING:
- Before planting (soil prep)
- Side-dress growing plants
- Foliar spray for quick boost
- Every 3-4 weeks during growth

NPK RATIO:
- N (Nitrogen): Leafy growth
- P (Phosphorus): Roots, flowers
- K (Potassium): Overall health

Vegetables need balanced fertilizer
Example: 10-10-10 or 5-10-10

PEST CONTROL:

PREVENTION:
Best defense:
- Healthy plants
- Crop rotation
- Companion planting
- Row covers
- Handpicking

ORGANIC METHODS:
- Neem oil (insects)
- Diatomaceous earth (crawling)
- Bt spray (caterpillars)
- Insecticidal soap
- Garlic spray

COMPANION PLANTING:
Helpful combinations:
- Tomatoes + basil (flavor, pests)
- Corn + beans + squash (3 sisters)
- Carrots + onions (pest control)
- Marigolds + anything (nematodes)

COMMON PESTS:
Aphids:
- Spray with water
- Ladybugs eat them
- Neem oil

Tomato hornworms:
- Handpick
- Bt spray
- Let wasps parasitize

Cabbage worms:
- Row covers
- Handpick
- Bt spray

Slugs/snails:
- Beer traps
- Copper barriers
- Handpick at night
- Diatomaceous earth

HARVESTING:

TIMING:
Each vegetable has signs:
- Tomatoes: Full color, slight give
- Peppers: Full size, firm
- Beans: Before seeds bulge
- Lettuce: Outer leaves anytime
- Squash: When tender (summer)

TECHNIQUE:
- Use clean, sharp tools
- Cut, don't pull
- Harvest regularly
- Morning is best (crisp)

STORAGE:
Immediate:
- Wash if dirty
- Cool quickly
- Don't wash if storing
- Remove damaged parts

EXTENDING SEASON:

EARLY START:
- Row covers
- Cold frames
- Wall o' water
- Black plastic mulch (warms soil)

LATE HARVEST:
- Row covers
- Low tunnels
- Mulch heavily
- Choose cold-hardy crops

SUCCESSION PLANTING:
Plant every 2 weeks:
- Lettuce
- Radishes
- Beans
- Carrots

Ensures continuous harvest

SEED SAVING:

EASY CROPS:
- Tomatoes
- Beans
- Peas
- Lettuce (if not hybrid)
- Peppers

METHOD:
1. Choose best plants
2. Let fruit fully mature
3. Extract seeds
4. Clean and dry
5. Store cool and dry
6. Label with date/variety

AVOID HYBRIDS:
Seeds won't grow true
Look for "heirloom" or "open-pollinated"

COMPOSTING:

BENEFITS:
- Free fertilizer
- Improves soil
- Reduces waste
- Feeds microbes

WHAT TO COMPOST:
Green (nitrogen):
- Kitchen scraps
- Fresh grass
- Green leaves
- Coffee grounds

Brown (carbon):
- Dry leaves
- Straw
- Paper
- Cardboard

WHAT NOT TO COMPOST:
- Meat/dairy
- Pet waste
- Diseased plants
- Weeds with seeds

PROCESS:
1. Layer greens and browns
2. Keep moist (like sponge)
3. Turn weekly
4. Ready in 2-6 months

CROP ROTATION:

WHY:
- Prevents disease
- Balances nutrients
- Reduces pests
- Improves soil

4-YEAR ROTATION:
Year 1: Tomatoes/peppers/eggplant
Year 2: Beans/peas
Year 3: Cabbage family
Year 4: Root vegetables

WINTER PLANNING:

GARDEN JOURNAL:
Record:
- What was planted where
- Dates (planting, harvest)
- Yields
- Problems
- Weather notes
- Next year plans

SEED ORDERS:
- Order early (best selection)
- Choose varieties suited to climate
- Try one new crop per year
- Stock up on favorites

TOOL MAINTENANCE:
- Clean and sharpen
- Oil moving parts
- Repair/replace broken
- Store properly

EMERGENCY FOOD GARDENS:

PRIORITY CROPS:
High calorie, storable:
- Potatoes (high yield, stores)
- Dried beans (protein, stores)
- Winter squash (calories, stores)
- Corn (calories, dried)
- Root vegetables (stores)

MAXIMIZE SPACE:
- Vertical growing
- Intensive spacing
- Succession planting
- Interplanting
- Container gardening

MINIMAL INPUT:
- Drought tolerant crops
- Disease resistant varieties
- Perennials (asparagus, rhubarb)
- Self-seeding (lettuce)
""",

    "Soil_Health": """SOIL HEALTH & MANAGEMENT

COMPREHENSIVE SOIL GUIDE

OVERVIEW:
Healthy soil is the foundation of productive gardens. Understanding and improving soil is critical for food security.

SOIL COMPONENTS:

MINERAL PARTICLES:
Sand:
- Large particles
- Drains quickly
- Warms fast
- Low nutrients

Silt:
- Medium particles
- Moderate drainage
- Fertile
- Good for most crops

Clay:
- Small particles
- Drains slowly
- Holds nutrients
- Hard when dry

IDEAL MIX (LOAM):
- 40% sand
- 40% silt
- 20% clay
- Best for most vegetables

ORGANIC MATTER:
Living and dead organisms:
- Decomposing plants
- Beneficial microbes
- Fungi
- Earthworms
- Insects

Benefits:
- Improves structure
- Holds water
- Provides nutrients
- Feeds microbes
- Prevents compaction

SOIL LIFE:
Billions per gram:
- Bacteria
- Fungi
- Protozoa
- Nematodes
- Arthropods

TESTING SOIL:

JAR TEST (Texture):
1. Fill jar 1/3 with soil
2. Add water to fill
3. Add drop of dish soap
4. Shake vigorously
5. Let settle 24-48 hours
6. Layers show sand/silt/clay

SQUEEZE TEST:
Sandy: Falls apart
Loamy: Forms ball, breaks easily
Clay: Forms ribbon, sticky

pH TESTING:
Use pH kit or meter:
- Most vegetables: 6.0-7.0
- Acidic: <6.0
- Alkaline: >7.0

HOME pH TEST:
Vinegar test (alkaline):
- Add vinegar to soil
- Fizzes = alkaline

Baking soda test (acidic):
- Mix soil with water
- Add baking soda
- Fizzes = acidic

NUTRIENT TESTING:
Professional test shows:
- Nitrogen (N)
- Phosphorus (P)
- Potassium (K)
- Micronutrients
- pH

Get test through:
- Extension service
- Garden centers
- Online labs

IMPROVING SOIL:

ADDING ORGANIC MATTER:
Best amendments:
- Compost (all-purpose)
- Aged manure (rich)
- Leaf mold (structure)
- Peat moss (acidifying)
- Coconut coir (sustainable)

AMOUNT:
Add 2-4 inches annually
Work into top 6-8 inches

COMPOST MAKING:

HOT COMPOSTING:
Fastest method (2-3 months):
1. Pile must be 3×3×3 feet minimum
2. Layer greens and browns
3. Keep moist
4. Turn weekly
5. Heat to 130-150°F
6. Ready when dark and crumbly

COLD COMPOSTING:
Slow method (6-12 months):
1. Pile materials
2. Add as available
3. No turning needed
4. Takes longer
5. Less effort

INGREDIENTS:
Greens (nitrogen):
- Kitchen scraps
- Grass clippings
- Green leaves
- Coffee grounds
- Fresh manure

Browns (carbon):
- Dry leaves
- Straw
- Sawdust
- Paper
- Cardboard

RATIO:
3 parts brown : 1 part green

MOISTURE:
Like wrung-out sponge
Too wet = smelly
Too dry = slow decomposition

ADJUSTING pH:

RAISING pH (less acidic):
Add:
- Lime (calcium carbonate)
- Wood ash
- Bone meal
- Crushed eggshells

Application:
- Apply in fall
- Work into soil
- Retest spring

LOWERING pH (more acidic):
Add:
- Sulfur
- Peat moss
- Pine needles
- Coffee grounds

Application:
- Takes months to work
- Apply in fall
- Retest regularly

IMPROVING DRAINAGE:

POOR DRAINAGE FIXES:
- Add compost
- Add sand (lots!)
- Create raised beds
- Install drainage tiles
- Improve grade

SIGNS OF POOR DRAINAGE:
- Standing water
- Mud
- Slow plant growth
- Yellow plants
- Root rot

IMPROVING SANDY SOIL:

PROBLEMS:
- Dries quickly
- Low nutrients
- Poor water retention

SOLUTIONS:
- Add compost heavily
- Add peat moss
- Mulch heavily
- Water frequently
- Fertilize more often

IMPROVING CLAY SOIL:

PROBLEMS:
- Poor drainage
- Hard when dry
- Compacts easily
- Slow to warm

SOLUTIONS:
- Add compost
- Add sand (LOTS!)
- Gypsum helps
- Don't work when wet
- Raised beds

DON'T:
Add only sand (makes concrete!)
Need both sand AND organic matter

COVER CROPS:

PURPOSE:
- Add nitrogen
- Prevent erosion
- Improve structure
- Suppress weeds
- Add organic matter

TYPES:
Nitrogen fixers:
- Clover
- Vetch
- Peas
- Beans

Biomass producers:
- Rye
- Oats
- Buckwheat
- Annual ryegrass

WHEN TO PLANT:
Fall:
- After harvest
- Grows over winter
- Till in spring

Spring:
- Between crops
- Quick growers
- Till before flowering

MULCHING:

BENEFITS:
- Conserves water
- Suppresses weeds
- Moderates temperature
- Adds organic matter (organic mulches)
- Prevents erosion

ORGANIC MULCHES:
- Straw (best for vegetables)
- Wood chips (paths, perennials)
- Leaves (free!)
- Grass clippings (thin layers)
- Compost (enriching)

INORGANIC MULCHES:
- Black plastic (warming)
- Landscape fabric (long-term)
- Gravel (permanent)

APPLICATION:
- 2-4 inches deep
- Keep away from plant stems
- Replenish as needed
- Don't mulch cool-season crops in cold weather

NO-TILL GARDENING:

BENEFITS:
- Preserves soil structure
- Protects microbes
- Saves work
- Improves water retention

METHOD:
1. Never dig or till
2. Add compost on top
3. Heavy mulching
4. Let worms incorporate
5. Plant through mulch

SHEET MULCHING:
1. Mow existing vegetation
2. Layer cardboard/newspaper
3. Add 4-6 inches compost
4. Top with straw mulch
5. Plant next season

EROSION CONTROL:

CAUSES:
- Water runoff
- Wind
- Bare soil
- Steep slopes

PREVENTION:
- Plant cover crops
- Mulch heavily
- Contour planting
- Terracing slopes
- Windbreaks

WATER MANAGEMENT:

IRRIGATION:
Efficient methods:
- Drip (most efficient)
- Soaker hoses
- Ollas (buried pots)
- Mulch (reduces need)

RAINWATER HARVESTING:
- Rain barrels
- Swales
- Ponds
- Berms

SOIL BUILDING TIMELINE:

YEAR 1:
- Test soil
- Add amendments
- Plant cover crop
- Start composting

YEAR 2:
- Heavy compost addition
- Mulch all beds
- Retest pH
- Observe improvement

YEAR 3:
- Maintenance mode
- Annual compost
- Continue mulching
- Excellent soil achieved

ONGOING:
- Add compost annually
- Rotate crops
- Maintain organic matter
- Monitor pH

EMERGENCY SOIL IMPROVEMENT:

NO COMPOST AVAILABLE:
Use:
- Aged manure
- Leaf mold
- Grass clippings
- Any organic matter

NO AMENDMENTS AT ALL:
- Deep mulching
- Plant legumes (fix nitrogen)
- Let plants decompose in place
- Save all organic waste

POOR INITIAL SOIL:
- Raised beds with brought-in soil
- Container gardening
- Focus on soil building first year
- Plant soil-improving crops

LONG-TERM FERTILITY:

SUSTAINABLE PRACTICES:
- Return all plant waste
- Minimal tilling
- Crop rotation
- Cover cropping
- Composting
- Mulching

AVOID:
- Synthetic fertilizers (temporary)
- Over-tilling (damages structure)
- Removing all plant material
- Mono-cropping
- Bare soil
"""
}

for filename, content in agriculture.items():
    filepath = os.path.join(agri_dir, f"{filename}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))
    print(f"  ✓ {filename}")

print(f"✓ Added {len(agriculture)} agriculture guides")

# ============================================================
# MORE PROGRAMMING LANGUAGES
# ============================================================
print("\n5. Adding more programming languages...")

more_langs = {
    "R_Programming": """R PROGRAMMING GUIDE

COMPREHENSIVE R REFERENCE

OVERVIEW:
R is a language for statistical computing and graphics. Widely used in data science, research, and analytics.

INSTALLATION:
Download from r-project.org
RStudio IDE recommended

HELLO WORLD:
```r
print("Hello World!")
cat("Hello", "World\\n")
```

VARIABLES:
```r
x <- 5              # Assignment
y = 10              # Also works
name <- "John"
age <- 25
active <- TRUE
```

VECTORS:
```r
# Numeric vector
numbers <- c(1, 2, 3, 4, 5)

# Character vector
names <- c("Alice", "Bob", "Charlie")

# Logical vector
flags <- c(TRUE, FALSE, TRUE)

# Access elements
numbers[1]      # First element (1-indexed!)
numbers[2:4]    # Elements 2-4
numbers[c(1,3,5)]  # Elements 1, 3, 5
```

FUNCTIONS:
```r
# Define function
add <- function(a, b) {
  return(a + b)
}

# Call function
result <- add(5, 3)

# Default parameters
greet <- function(name = "Guest") {
  paste("Hello", name)
}
```

DATA FRAMES:
```r
# Create data frame
df <- data.frame(
  name = c("Alice", "Bob", "Charlie"),
  age = c(25, 30, 35),
  score = c(85, 92, 78)
)

# Access columns
df$name
df[, "age"]
df[, 1]

# Access rows
df[1, ]      # First row
df[1:2, ]    # First two rows

# Subset
subset(df, age > 25)
```

READING DATA:
```r
# CSV
data <- read.csv("file.csv")
write.csv(data, "output.csv")

# Excel (requires package)
library(readxl)
data <- read_excel("file.xlsx")
```

BASIC STATISTICS:
```r
x <- c(1, 2, 3, 4, 5)

mean(x)      # Average
median(x)    # Median
sd(x)        # Standard deviation
var(x)       # Variance
sum(x)       # Sum
min(x)       # Minimum
max(x)       # Maximum
```

PLOTTING:
```r
# Basic plot
plot(x, y)

# Histogram
hist(data$age)

# Boxplot
boxplot(data$score)

# Scatter plot
plot(data$age, data$score)

# ggplot2 (advanced)
library(ggplot2)
ggplot(data, aes(x=age, y=score)) +
  geom_point()
```

CONTROL STRUCTURES:
```r
# If statement
if (x > 10) {
  print("Greater")
} else if (x > 5) {
  print("Medium")
} else {
  print("Small")
}

# For loop
for (i in 1:10) {
  print(i)
}

# While loop
while (condition) {
  # code
}
```

APPLY FUNCTIONS:
```r
# Apply to each element
sapply(numbers, function(x) x * 2)

# Apply to rows/columns
apply(matrix, 1, mean)  # Row means
apply(matrix, 2, mean)  # Column means

# Apply to list
lapply(list, function(x) x * 2)
```

PACKAGES:
```r
# Install package
install.packages("package_name")

# Load package
library(package_name)

# Useful packages
library(dplyr)     # Data manipulation
library(ggplot2)   # Plotting
library(tidyr)     # Data tidying
```

DATA MANIPULATION (dplyr):
```r
library(dplyr)

# Filter rows
filter(df, age > 25)

# Select columns
select(df, name, age)

# Mutate (add column)
mutate(df, age_squared = age^2)

# Arrange (sort)
arrange(df, age)

# Summarize
summarize(df, mean_age = mean(age))

# Pipe operator
df %>%
  filter(age > 25) %>%
  select(name, score) %>%
  arrange(score)
```

COMMON USE CASES:
- Statistical analysis
- Data visualization
- Machine learning
- Bioinformatics
- Financial modeling
- Research

BEST PRACTICES:
- Use RStudio
- Comment your code
- Use meaningful variable names
- Load packages at top
- Save scripts
- Use projects
- Version control
""",

    "SQL_Database": """SQL DATABASE GUIDE

COMPREHENSIVE SQL REFERENCE

OVERVIEW:
SQL (Structured Query Language) is used to manage and query relational databases. Essential for data storage and retrieval.

BASIC CONCEPTS:

DATABASE:
Collection of tables

TABLE:
Collection of rows and columns
Like a spreadsheet

ROW:
Single record (e.g., one person)

COLUMN:
Field in record (e.g., name, age)

PRIMARY KEY:
Unique identifier for each row

FOREIGN KEY:
Links to another table's primary key

DATA TYPES:
```sql
INT             -- Integer
VARCHAR(n)      -- Variable string (max n chars)
TEXT            -- Long text
DATE            -- Date (YYYY-MM-DD)
DATETIME        -- Date and time
DECIMAL(p,s)    -- Decimal number
BOOLEAN         -- True/False
```

CREATING DATABASE:
```sql
CREATE DATABASE company;
USE company;
```

CREATING TABLE:
```sql
CREATE TABLE employees (
    id INT PRIMARY KEY AUTO_INCREMENT,
    name VARCHAR(100),
    age INT,
    department VARCHAR(50),
    salary DECIMAL(10, 2),
    hire_date DATE
);
```

INSERTING DATA:
```sql
-- Single row
INSERT INTO employees (name, age, department, salary)
VALUES ('John Doe', 30, 'Sales', 50000.00);

-- Multiple rows
INSERT INTO employees (name, age, department, salary)
VALUES
    ('Jane Smith', 28, 'Marketing', 55000.00),
    ('Bob Johnson', 35, 'IT', 65000.00);
```

SELECTING DATA:
```sql
-- All columns, all rows
SELECT * FROM employees;

-- Specific columns
SELECT name, salary FROM employees;

-- With condition
SELECT * FROM employees WHERE age > 30;

-- Multiple conditions
SELECT * FROM employees
WHERE age > 25 AND department = 'Sales';

-- OR condition
SELECT * FROM employees
WHERE department = 'Sales' OR department = 'Marketing';

-- LIKE (pattern matching)
SELECT * FROM employees WHERE name LIKE 'John%';

-- IN clause
SELECT * FROM employees
WHERE department IN ('Sales', 'Marketing', 'IT');

-- BETWEEN
SELECT * FROM employees WHERE age BETWEEN 25 AND 35;
```

ORDERING RESULTS:
```sql
-- Ascending
SELECT * FROM employees ORDER BY salary;

-- Descending
SELECT * FROM employees ORDER BY salary DESC;

-- Multiple columns
SELECT * FROM employees ORDER BY department, salary DESC;
```

LIMITING RESULTS:
```sql
-- First 10 rows
SELECT * FROM employees LIMIT 10;

-- Skip 5, take 10
SELECT * FROM employees LIMIT 5, 10;
```

UPDATING DATA:
```sql
-- Update single column
UPDATE employees SET salary = 55000 WHERE id = 1;

-- Update multiple columns
UPDATE employees
SET salary = 60000, department = 'Sales'
WHERE id = 2;

-- Update with condition
UPDATE employees
SET salary = salary * 1.1
WHERE department = 'Sales';
```

DELETING DATA:
```sql
-- Delete specific row
DELETE FROM employees WHERE id = 1;

-- Delete with condition
DELETE FROM employees WHERE age > 65;

-- Delete all rows (CAREFUL!)
DELETE FROM employees;
```

AGGREGATE FUNCTIONS:
```sql
-- Count
SELECT COUNT(*) FROM employees;

-- Sum
SELECT SUM(salary) FROM employees;

-- Average
SELECT AVG(salary) FROM employees;

-- Min/Max
SELECT MIN(salary), MAX(salary) FROM employees;

-- Group by
SELECT department, AVG(salary)
FROM employees
GROUP BY department;

-- Having (filter after grouping)
SELECT department, AVG(salary) as avg_sal
FROM employees
GROUP BY department
HAVING avg_sal > 50000;
```

JOINS:
```sql
-- INNER JOIN
SELECT employees.name, departments.dept_name
FROM employees
INNER JOIN departments ON employees.dept_id = departments.id;

-- LEFT JOIN (all from left table)
SELECT employees.name, departments.dept_name
FROM employees
LEFT JOIN departments ON employees.dept_id = departments.id;

-- RIGHT JOIN (all from right table)
SELECT employees.name, departments.dept_name
FROM employees
RIGHT JOIN departments ON employees.dept_id = departments.id;
```

SUBQUERIES:
```sql
-- In WHERE clause
SELECT * FROM employees
WHERE salary > (SELECT AVG(salary) FROM employees);

-- In FROM clause
SELECT * FROM (
    SELECT name, salary FROM employees WHERE age > 30
) AS older_employees;
```

INDEXES:
```sql
-- Create index (speeds up searches)
CREATE INDEX idx_name ON employees(name);

-- Composite index
CREATE INDEX idx_dept_salary ON employees(department, salary);

-- Drop index
DROP INDEX idx_name ON employees;
```

CONSTRAINTS:
```sql
CREATE TABLE users (
    id INT PRIMARY KEY AUTO_INCREMENT,
    email VARCHAR(100) UNIQUE NOT NULL,
    age INT CHECK (age >= 18),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

ALTER TABLE:
```sql
-- Add column
ALTER TABLE employees ADD COLUMN email VARCHAR(100);

-- Drop column
ALTER TABLE employees DROP COLUMN email;

-- Modify column
ALTER TABLE employees MODIFY COLUMN name VARCHAR(150);

-- Add constraint
ALTER TABLE employees ADD CONSTRAINT chk_age CHECK (age >= 18);
```

TRANSACTIONS:
```sql
START TRANSACTION;

UPDATE accounts SET balance = balance - 100 WHERE id = 1;
UPDATE accounts SET balance = balance + 100 WHERE id = 2;

COMMIT;  -- Save changes
-- or --
ROLLBACK;  -- Undo changes
```

COMMON USE CASES:
- Application databases
- Data warehousing
- Reporting
- E-commerce
- User management
- Analytics

BEST PRACTICES:
- Use meaningful table/column names
- Always use WHERE in UPDATE/DELETE
- Index frequently queried columns
- Normalize database structure
- Use transactions for related changes
- Backup regularly
- Use prepared statements (prevent SQL injection)

POPULAR DATABASES:
- MySQL (open source)
- PostgreSQL (open source)
- SQL Server (Microsoft)
- SQLite (embedded)
- Oracle (enterprise)
"""
}

for lang_name, content in more_langs.items():
    filepath = os.path.join(prog_dir, f"{lang_name}.txt")
    with open(filepath, 'w', encoding='utf-8') as f:
        f.write(wrap_text(content))
    print(f"  ✓ {lang_name}")

print(f"✓ Added {len(more_langs)} more programming languages")

print("\n" + "=" * 60)
print("PHASE 2 COMPLETE")
print("=" * 60)
