```
 ██████╗ █████╗███╗   ███╗██████╗ ██╗   ██╗███████╗
██╔════╝██╔══██╗████╗ ████║██╔══██╗██║   ██║██╔════╝
██║     ███████║██╔████╔██║██████╔╝██║   ██║███████╗
██║     ██╔══██║██║╚██╔╝██║██╔═══╝ ██║   ██║╚════██║
╚██████╗██║  ██║██║ ╚═╝ ██║██║     ╚██████╔╝███████║
 ╚═════╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝      ╚═════╝ ╚══════╝

 █████╗ ███╗   ██╗ █████╗ ██╗  ██╗   ██╗████████╗██╗ ██████╗███████╗
██╔══██╗████╗  ██║██╔══██╗██║  ╚██╗ ██╔╝╚══██╔══╝██║██╔════╝██╔════╝
███████║██╔██╗ ██║███████║██║   ╚████╔╝    ██║   ██║██║     ███████╗
██╔══██║██║╚██╗██║██╔══██║██║    ╚██╔╝     ██║   ██║██║     ╚════██║
██║  ██║██║ ╚████║██║  ██║███████╗██║      ██║   ██║╚██████╗███████║
╚═╝  ╚═╝╚═╝  ╚═══╝╚═╝  ╚═╝╚══════╝╚═╝      ╚═╝   ╚═╝ ╚═════╝╚══════╝

███████╗███╗   ██╗ ██████╗ ██╗███╗   ██╗███████╗
██╔════╝████╗  ██║██╔════╝ ██║████╗  ██║██╔════╝
█████╗  ██╔██╗ ██║██║  ███╗██║██╔██╗ ██║█████╗
██╔══╝  ██║╚██╗██║██║   ██║██║██║╚██╗██║██╔══╝
███████╗██║ ╚████║╚██████╔╝██║██║ ╚████║███████╗
╚══════╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝╚═╝  ╚═══╝╚══════╝
```

<div align="center">

**A modular, flat-file academic management system written in pure C++17**

![Language](https://img.shields.io/badge/Language-C%2B%2B17-blue?style=for-the-badge&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Linux-orange?style=for-the-badge&logo=linux)
![Build](https://img.shields.io/badge/Build-g%2B%2B-green?style=for-the-badge&logo=gnu)
![License](https://img.shields.io/badge/License-MIT-purple?style=for-the-badge)
![Author](https://img.shields.io/badge/Made%20by-Areeba%20Asghar-ff69b4?style=for-the-badge)

</div>

---

## 📖 Table of Contents

- [✨ Overview](#-overview)
- [🏗️ System Architecture](#️-system-architecture)
- [⚙️ Modules](#️-modules)
- [🔧 Build Instructions](#-build-instructions)
- [🚀 How to Run](#-how-to-run)
- [📁 Project Structure](#-project-structure)
- [🗃️ Data Format](#️-data-format)
- [🎮 Usage Guide](#-usage-guide)
- [📸 Sample Output](#-sample-output)
- [⚡ Design Constraints](#-design-constraints)
- [👩‍💻 Author](#-author)

---

## ✨ Overview

**Campus Analytics Engine** is a fully functional, terminal-based academic management system built entirely in **C++17** with no external libraries. It manages students, courses, attendance, grades, and financial records using a flat-file database architecture.

> 🎯 Built as a demonstration of **modular systems programming** without OOP, standard containers like `map`/`set`, or external dependencies.

### 🌟 Key Highlights

| Feature | Detail |
|---|---|
| 💾 **Storage** | Flat-file CSV (no SQL, no JSON) |
| 🔒 **No STL containers** | No `map`, `set`, or `algorithm` |
| 📅 **No `<ctime>`** | Manual DD-MM-YYYY date arithmetic |
| 🔃 **Manual sorting** | Selection Sort & Bubble Sort |
| 🧩 **7 independent modules** | Fully decoupled architecture |
| 🔍 **Live search** | Keystroke-by-keystroke student finder |

---

## 🏗️ System Architecture

```
┌─────────────────────────────────────────────────────┐
│                    main.cpp                         │
│              (Menu Router & Entry Point)             │
└──────────┬──────────────────────────────────────────┘
           │
     ┌─────▼──────────────────────────────────────┐
     │              filehandler.cpp                │
     │  (I/O Engine · Parsing · String Utilities)  │
     └─────┬──────────────────┬───────────────────┘
           │                  │
    ┌──────▼──────┐    ┌──────▼──────┐
    │ student_ops │    │ course_ops  │
    │  (M1 · M2)  │    │  (M2 · M3)  │
    └──────┬──────┘    └──────┬──────┘
           │                  │
    ┌──────▼──────────────────▼──────────────────┐
    │    attendance  │   grades   │  fee_tracker  │
    │     (M3)       │   (M4)     │     (M5)      │
    └──────────────────┬─────────────────────────┘
                       │
                ┌──────▼──────┐
                │   reports   │
                │    (M6)     │
                └─────────────┘
```

---

## ⚙️ Modules

| # | Module | File | Responsibility |
|---|---|---|---|
| M1 | 🎓 Student Directory | `student_ops.cpp` | Register, update, deactivate, list students |
| M2 | 📚 Course & Enrollment | `course_ops.cpp` | Enroll/drop students, prerequisite checks |
| M3 | 📋 Attendance Logger | `attendance.cpp` | Mark daily attendance, compute percentages |
| M4 | 📊 Grading Engine | `grades.cpp` | Weighted marks, GPA, statistics |
| M5 | 💰 Fee Tracker | `fee_tracker.cpp` | Payments, late fines, receipts |
| M6 | 📈 Reports | `reports.cpp` | Merit list, transcripts, exports |
| 🔩 | File Handler | `filehandler.cpp` | All I/O, parsing, and utility functions |

---

## 🔧 Build Instructions

### Prerequisites

- `g++` with C++17 support
- Linux / macOS (uses POSIX `termios` for live search)

### Compile

```bash
g++ -std=c++17 -O2 -Wall -o campus_engine \
    main.cpp filehandler.cpp student_ops.cpp course_ops.cpp \
    attendance.cpp grades.cpp fee_tracker.cpp reports.cpp
```

### One-liner

```bash
g++ -std=c++17 -O2 -o campus_engine *.cpp
```

---

## 🚀 How to Run

```bash
# 1. Create the data directory
mkdir -p data

# 2. Run the engine
./campus_engine
```

> 💡 The `data/` folder must exist in the **same directory** you launch from. The program will create and manage all `.txt` files inside it automatically.

---

## 📁 Project Structure

```
campus-analytics-engine/
│
├── 📄 main.cpp              # Entry point & menu router
│
├── 🔩 filehandler.h/.cpp    # Core I/O engine & utilities
│
├── 🎓 student_ops.h/.cpp    # Student directory management
├── 📚 course_ops.h/.cpp     # Course & enrollment system
├── 📋 attendance.h/.cpp     # Attendance logging
├── 📊 grades.h/.cpp         # Grading & GPA engine
├── 💰 fee_tracker.h/.cpp    # Financial ledger
├── 📈 reports.h/.cpp        # Enterprise report compiler
│
├── 🗃️ data/                 # Flat-file database (auto-created)
│   ├── students.txt
│   ├── courses.txt
│   ├── enrollments.txt
│   ├── attendance_log.txt
│   ├── grades.txt
│   └── fees.txt
│
└── 📖 README.md
```

---

## 🗃️ Data Format

All data is stored as **comma-separated `.txt` files** with a header row.

### 👤 students.txt
```
roll,name,dept,cgpa,status
f2025376200,Mustafa Haider,CS,3.70,active
f2025376201,Areeba Asghar,CS,3.95,active
```
> Roll format: `fYYYYNNNNNN` — lowercase `f` + 4-digit year + 6-digit ID

### 📚 courses.txt
```
code,title,credits,seats,prereq
CS101,Programming Fundamentals,3,40,NONE
AI201,Intro to Artificial Intelligence,3,35,CS101
```

### 📋 enrollments.txt
```
roll,course_code,semester,status
f2025376200,CS101,Spring2025,enrolled
```

### 📋 attendance_log.txt
```
roll,course_code,date,status
f2025376200,CS101,15-02-2025,P
```
> Status codes: `P` = Present · `A` = Absent · `L` = Leave

### 📊 grades.txt
```
roll,course_code,q1,q2,q3,q4,q5,assignment,midterm,final,total,grade
f2025376200,CS101,9.0,8.5,9.5,8.0,9.0,88.0,37.0,54.0,90.42,A
```

### 💰 fees.txt
```
roll,semester,amount_due,amount_paid,due_date,paid_date
f2025376200,Spring2025,65000,65000,15-01-2025,10-01-2025
```

---

## 🎮 Usage Guide

```
==================================================
       CAMPUS ANALYTICS ENGINE - MAIN MENU
==================================================
  1. Student Directory Management
  2. Course & Enrollment Management
  3. Attendance Logging System
  4. Grading & Class Performance
  5. Student Financial Ledger
  6. Enterprise Analytics & Reports
  7. Search Active Students (Keystroke Search)
  8. Exit System
--------------------------------------------------
```

### 🎓 1 — Student Directory
| Option | Action |
|---|---|
| Register New Student | Add with roll `fYYYYNNNNNN`, name, dept, CGPA |
| Update Student | Modify name, dept, or CGPA |
| Deactivate Student | Soft-delete (keeps records intact) |
| List Active Students | Sorted alphabetically by roll |

### 📚 2 — Course & Enrollment
- Enroll students (checks prerequisites + seat limits)
- Drop courses
- View class roster for any course + semester

### 📋 3 — Attendance
- Mark attendance for an entire class session (P/A/L per student)
- View any student's attendance % per course
- See who is below the **75% threshold** (at-risk list)
- Undo last session

### 📊 4 — Grades
- Enter marks: 5 quizzes + assignment + midterm + final
- View full breakdown with weighted totals
- Compute Semester GPA (credit-weighted)
- Course statistics: Mean, Median, Highest, Lowest

### 💰 5 — Financial Ledger
- Record fee payments
- Generate formatted receipts / invoices
- View defaulters with **2% compound weekly late fine**

### 📈 6 — Reports
- 🏆 **Merit List** — all active students ranked by CGPA
- ⚠️ **Attendance Defaulters** — below 75% in any course
- 💸 **Fee Defaulters** — sorted by outstanding balance
- 📜 **Semester Transcript** — full academic record per student
- 🏛️ **Department Summary** — avg CGPA & pass rate per dept
- 💾 **Export to File** — redirect any report to a `.txt` file

### 🔍 7 — Live Keystroke Search
Type any part of a roll number or name — results filter in real time. Press `ESC` to exit.

---

## 📸 Sample Output

### Merit List
```
============================================================
                    MERIT LIST
============================================================
Rank  Roll          Name                  Dept    CGPA
------------------------------------------------------------
1     f2025376201   Ayesha Tariq          CS      3.85
2     f2025376200   Mustafa Haider        CS      3.70
3     f2024375101   Fatima Nawaz          AI      3.40
------------------------------------------------------------
  Total Students Listed: 11
============================================================
```

### Fee Receipt
```
==================================================
           FEE RECEIPT / INVOICE
==================================================
  Student             : Mustafa Haider
  Roll No.            : f2025376200
  Semester            : Spring2025
  Due Date            : 15-01-2025
  Paid Date           : 10-01-2025
--------------------------------------------------
  Total Fee                          65000
  Amount Paid                        65000
--------------------------------------------------
  Outstanding Balance                 0.00
  Status                         PAID IN FULL
==================================================
```

---

## ⚡ Design Constraints

This project was built under strict academic constraints:

| ❌ Forbidden | ✅ Used Instead |
|---|---|
| `#include <algorithm>` | Manual Selection / Bubble Sort |
| `#include <ctime>` | Manual DD-MM-YYYY arithmetic |
| `map`, `set`, `unordered_map` | Parallel `vector` arrays |
| OOP (classes, inheritance) | Plain `struct` containers |
| Regex / `<regex>` | Manual character-by-character parsing |

---

## 👩‍💻 Author

<div align="center">

```
  ╔══════════════════════════════════════╗
  ║                                      ║
  ║   Made with ❤️  by Areeba Asghar     ║
  ║                                      ║
  ║   Campus Analytics Engine  v1.0      ║
  ║   C++17 · Modular · Flat-File DB     ║
  ║                                      ║
  ╚══════════════════════════════════════╝
```

[![GitHub](https://img.shields.io/badge/GitHub-areeba--asghar-181717?style=for-the-badge&logo=github)](https://github.com/areeba-asghar)

</div>

---

<div align="center">

*© 2025 Areeba Asghar — MIT License*

</div>
