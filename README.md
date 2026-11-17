# Precision Irrigation Scheduler for Small Farms 🌾💧

A modular, robust C-based irrigation management and decision-support system designed to optimize agricultural water consumption, minimize operational costs, and prevent crop water stress on small-scale farms.

---

## 📋 Table of Contents
- [Real-World Context](#-real-world-context)
- [Problem Description](#-problem-description)
- [Key Features](#-key-features)
- [Course Outcome (CO) Mapping](#-course-outcome-co-mapping)
- [Syllabus Topics & Programming Concepts Applied](#-syllabus-topics--programming-concepts-applied)
- [Mathematical & Agronomic Model](#-mathematical--agronomic-model)
- [System Architecture & Data Structures](#-system-architecture--data-structures)
- [Project Structure](#-project-structure)
- [Compilation and Execution Guide](#-compilation-and-execution-guide)
- [Sample Execution & Walkthrough](#-sample-execution--walkthrough)
- [Author & License](#-author--license)

---

## 🌍 Real-World Context

Water scarcity and unpredictable climate patterns pose severe threats to agricultural productivity, particularly for smallholder farmers. Conventional irrigation practices frequently rely on fixed intervals or visual intuition, leading to over-irrigation (which leaches nutrients and inflates electricity/diesel pumping costs) or under-irrigation (which impairs yield through moisture stress).

The **Precision Irrigation Scheduler** addresses this challenge by synthesizing real-time agronomic variables:
1. **Soil Moisture Saturation Levels (%)** from field sensors.
2. **Crop-Specific Water Demand Coefficients ($K_c$)** across developmental stages (e.g., Wheat, Rice, Maize, Cotton, Sugarcane).
3. **Local Weather Forecasts & Evapotranspiration ($ET_0$)** alongside observed and predicted precipitation.

By processing these parameters, the system dynamically schedules optimal irrigation depths, projects daily and weekly financial expenditure, and performs drought-resilient "what-if" simulations when water supply is constrained.

---

## 🎯 Problem Description

Design and implement a modular command-line farm management software in C that:
- **Maintains Plot Inventories:** Tracks multi-plot farm characteristics including Plot ID, crop type, physical area (in hectares), current soil moisture (%), moisture stress threshold (%), and days elapsed since prior irrigation.
- **Ingests Environmental & Meteorological Inputs:** Captures daily reference evapotranspiration ($ET_0$ in mm), 24-hour rainfall (mm), and 7-day expected precipitation (mm).
- **Computes Volumetric Water Demand ($m^3$):** Applies empirical FAO-style irrigation depth equations adjusted for irrigation system efficiency (e.g., 85% drip/sprinkler efficiency) and area scaling.
- **Flags Critical Plots:** Triggers automated warnings whenever soil moisture drops below critical thresholds or prolonged drought occurs.
- **Simulates Water Scarcity Scenarios ("What-If" Analysis):** Prioritizes plots using weighted deficit criteria, allocates restricted reservoir water strictly by priority order, and reports deficits.
- **Audits Dynamic Irrigation Events:** Records all simulated irrigation allocations into a dynamic, heap-allocated linked list for historical auditing and traceability.
- **Manages Seasonal Crops Dynamically:** Builds and traverses a dynamic list of active seasonal crops and associated crop coefficients.

---

## ✨ Key Features

- **Empirical Crop Coefficient Engine:** Dynamic coefficient mapping for Wheat ($K_c=1.15$), Rice ($K_c=1.20$), Maize ($K_c=1.05$), Cotton ($K_c=0.90$), and Sugarcane ($K_c=0.75$).
- **Dual Schedule Forecasting:** Concurrently generates single-day and 7-day irrigation volumes ($m^3$) with localized monetary expenditure estimates (in ₹).
- **Greedy Priority-Based Water Allocation:** When supply falls short of total farm requirement, water is allocated according to a composite priority score:
  - $+10$ points if current moisture is below threshold.
  - $+10$ points if moisture is severely depleted ($>10\%$ below threshold).
  - $+5$ points if $\ge 3$ days have elapsed since last irrigation.
- **Dynamic Memory Management:** Zero static waste for irrigation event logging and seasonal crop lists using singly linked lists with proper heap cleanup (`free()`).
- **Interactive Menu-Driven Interface:** Full terminal interface providing user-friendly workflows.

---

## 🎓 Course Outcome (CO) Mapping

| Course Outcome (CO) | Description | Project Implementation |
| :--- | :--- | :--- |
| **CO1** | **Design scheduling algorithm** | Designed multi-variable irrigation scheduling algorithm balancing $ET_0$, $K_c$, effective rainfall, and soil moisture deficits. |
| **CO2** | **Implement thresholds and decision logic** | Established threshold comparison logic (`moisture < threshold`), multi-tiered priority ranking, and boundary condition handling ($deficit \ge 0$). |
| **CO3** | **Apply arrays and functions for scheduling and reporting** | Structured code into modular functions (`calculateWater`, `generateSchedule`, `sortPriority`, etc.) operating over 1D arrays of `struct Plot`. |
| **CO4** | **Use pointers for dynamic event lists** | Implemented dynamic singly linked lists (`struct Event*`, `struct Crop*`) with double pointers (`**head`) for in-place list modification and dynamic allocation. |
| **CO5** | **Deliver a modular solution covering syllabus areas** | Integrated structured programming practices, header modularity, clean error handling, safe pointer deallocation, and algorithmic documentation. |

---

## 📚 Syllabus Topics & Programming Concepts Applied

1. **Problem Solving & Algorithm Design:**
   - Irrigation water balance formulation.
   - Bubble sort algorithm for descending priority rank in `sortPriority()`.
   - Greedy allocation algorithm in `simulateWaterShortage()`.
2. **Data Types & Constants:**
   - Primitive types (`int`, `float`, `char`).
   - Symbolic preprocessor macro (`#define MAX_PLOTS 20`).
3. **Decision Making & Branching:**
   - Multi-way branching with `switch-case` for the main dashboard menu.
   - Nested `if-else` blocks for crop coefficient lookups and water deficit bounding.
4. **Iterative Loops:**
   - `for` loops for plot input reading, schedule generation, and priority sorting.
   - `while` loops for traversing dynamic linked lists (`Event` and `Crop`).
   - `do-while` loop for keeping the interactive menu session active until exit.
5. **Arrays & Strings:**
   - 1D array of structures `struct Plot plots[MAX_PLOTS]`.
   - Bounded character array strings (`char crop[30]`) with formatted `scanf("%29s")` preventing buffer overflow.
6. **Structures:**
   - Heterogeneous data grouping with `struct Plot`, `struct Event`, and `struct Crop`.
7. **Pointers & Dynamic Memory Management:**
   - Dynamic memory allocation on the heap via `malloc()`.
   - Double pointer passing (`struct Event **head`, `struct Crop **head`) for head-pointer manipulation.
   - Complete heap deallocation avoiding memory leaks via `freeEvents()` and `freeCrops()`.
8. **Mathematical Arithmetic Modeling:**
   - Soil moisture deficit estimation, precipitation offset, and volumetric metric conversions ($1\text{ mm} \times 1\text{ ha} = 10\text{ m}^3$).

---

## 🧮 Mathematical & Agronomic Model

### 1. Crop Evapotranspiration ($ET_c$)
$$ET_c = ET_0 \times K_c$$
*Where:*
- $ET_0$ = Reference evapotranspiration (mm/day)
- $K_c$ = Crop coefficient based on growth stage

### 2. Moisture Deficit & Net Irrigation Depth ($I_d$)
The moisture deficit represents water required to restore root-zone field capacity:
$$\text{Moisture Deficit} = \max\left(0, (\text{Threshold} - \text{Moisture}) \times 0.5\right)$$
$$\text{Evaporative Demand} = \max\left(0, ET_c - \text{Rainfall}\right)$$
$$\text{Irrigation Depth (mm)} = \max\left(\text{Moisture Deficit}, \text{Evaporative Demand}\right)$$

### 3. Volumetric Water Requirement ($V$)
To convert depth (mm) over area (hectares) to cubic meters ($m^3$), adjusted for application efficiency ($\eta = 0.85$):
$$V = \frac{I_d \times \text{Area (ha)} \times 10}{\eta}$$
*(Note: $1 \text{ mm} \times 1 \text{ ha} = 10^{-3} \text{ m} \times 10^4 \text{ m}^2 = 10 \text{ m}^3$)*

---

## 🏗️ System Architecture & Data Structures

```
                      +-----------------------------+
                      |       User Terminal         |
                      +--------------+--------------+
                                     |
                                     v
                       +-------------+-------------+
                       |    main() Menu Controller |
                       +-------------+-------------+
                                     |
          +--------------------------+--------------------------+
          |                          |                          |
          v                          v                          v
  +---------------+          +---------------+          +---------------+
  | Array of      |          | Linked List   |          | Linked List   |
  | struct Plot   |          | struct Event  |          | struct Crop   |
  +---------------+          +---------------+          +---------------+
  | - id          |          | - plotId      |          | - name        |
  | - crop, area  |          | - water (m3)  |          | - coefficient |
  | - moisture    |          | - next*       |          | - next*       |
  | - threshold   |          +---------------+          +---------------+
  | - priority    |
  +---------------+
          |
          +---> calculateWater()  --> FAO Irrigation Depth Model
          +---> sortPriority()    --> Bubble Sort on Urgency
          +---> simulateShortage()--> Greedy Capacity Allocation
```

---

## 📂 Project Structure

```
Precision-Irrigation-Scheduler/
│
├── main.c                   # Complete source code in C99 standard
├── README.md                # Comprehensive project documentation
└── .gitignore               # Ignored build artifacts & executables
```

---

## ⚙️ Compilation and Execution Guide

### Prerequisites
- A modern C compiler supporting C99 or later (`gcc`, `clang`, or MSVC).

### Using GCC (Linux / macOS / Windows MinGW)
```bash
# 1. Compile with all warnings enabled
gcc -Wall -Wextra -std=c99 main.c -o irrigation_scheduler

# 2. Run the executable
# On Windows:
.\irrigation_scheduler.exe

# On Linux / macOS:
./irrigation_scheduler
```

### Using Clang
```bash
clang -Wall -Wextra -std=c99 main.c -o irrigation_scheduler
./irrigation_scheduler
```

---

## 🧪 Sample Execution & Walkthrough

### 1. Configuration & Input
```text
========================================
  PRECISION IRRIGATION SCHEDULER
       FOR SMALL FARMS
========================================

Enter number of farm plots (Max 20): 2
Enter water cost per cubic metre (Rs.): 15.50

========== PLOT 1 ==========
Enter crop name (Wheat/Rice/Maize/Cotton/Sugarcane): Wheat
Enter plot area (hectares): 1.5
Enter current soil moisture (%): 28.0
Enter moisture threshold (%): 45.0
Days since last irrigation: 4
Enter daily reference ET (mm): 4.2
Enter today's rainfall (mm): 0.0
Enter expected weekly rainfall (mm): 5.0

========== PLOT 2 ==========
Enter crop name (Wheat/Rice/Maize/Cotton/Sugarcane): Rice
Enter plot area (hectares): 2.0
Enter current soil moisture (%): 55.0
Enter moisture threshold (%): 60.0
Days since last irrigation: 1
Enter daily reference ET (mm): 4.5
Enter today's rainfall (mm): 2.0
Enter expected weekly rainfall (mm): 10.0
```

### 2. Schedule Generation Output
```text
========== IRRIGATION SCHEDULE ==========

Plot ID: 1
Crop: Wheat
Daily Water: 150.00 m3
Weekly Water: 508.41 m3
ALERT: Immediate irrigation attention required!
-----------------------------------

Plot ID: 2
Crop: Rice
Daily Water: 80.00 m3
Weekly Water: 654.12 m3
ALERT: Immediate irrigation attention required!
-----------------------------------

TOTAL DAILY WATER: 230.00 m3
TOTAL WEEKLY WATER: 1162.53 m3
Daily Estimated Cost: Rs. 3565.00
Weekly Estimated Cost: Rs. 18019.21
```

### 3. Drought / Scarcity Simulation Output (Available Water = 100 m³)
```text
========== WATER SHORTAGE SIMULATION ==========

Priority-wise allocation:

Plot 1 | Priority: 25
Required: 150.00 m3
Allocated: 100.00 m3
Status: Partially irrigated

Plot 2 | Priority: 10
Required: 80.00 m3
Allocated: 0.00 m3
Status: Water unavailable!

========== SIMULATION REPORT ==========
Available Water: 100.00 m3
Used Water: 100.00 m3
Remaining Water: 0.00 m3
Estimated Cost: Rs. 1550.00
```

---

## 👨‍💻 Author & Attribution

- **Project:** Programming for Problem Solving (PPS) Academic Capstone Project
- **Topic:** Precision Irrigation Scheduler for Small Farms
- **Course Outcomes:** CO1, CO2, CO3, CO4, CO5
