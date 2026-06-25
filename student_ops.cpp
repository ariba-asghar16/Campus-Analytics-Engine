// ============================================================================
// student_ops.cpp — M2: Student Directory Mutation and Filtering
// ============================================================================

#include "student_ops.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


// ============================================================================
// rowToStudent — Parse a CSV row vector into a Student struct
// ============================================================================
Student rowToStudent(const vector<string>& row) {
    Student s;
    s.roll   = "";
    s.name   = "";
    s.dept   = "";
    s.cgpa   = 0.0;
    s.status = "";

    if (row.size() >= 5) {
        s.roll   = row[0];
        s.name   = row[1];
        s.dept   = row[2];

        // Manual string-to-double conversion
        istringstream iss(row[3]);
        iss >> s.cgpa;

        s.status = toLower(row[4]);
    }
    return s;
}

// ============================================================================
// studentToRow — Serialize a Student struct into a CSV row vector
// ============================================================================
vector<string> studentToRow(const Student& s) {
    vector<string> row;
    row.push_back(s.roll);
    row.push_back(s.name);
    row.push_back(s.dept);
    row.push_back(doubleToStr(s.cgpa, 2));
    row.push_back(s.status);
    return row;
}

// ============================================================================
// validateRollFormat — Manual BSAI-YY-XXX pattern check
// ============================================================================
// Pattern: B S A I - D D - D D D
// Index:   0 1 2 3 4 5 6 7 8 9 10
// Length must be exactly 11.
// ============================================================================
bool validateRollFormat(const string& roll) {
    // Format: fYYYYNNNNNN  (e.g. f2025376200)
    // Length must be exactly 11 characters.
    if (roll.length() != 11) return false;

    // Position 0 must be 'f' or 'F'
    if (roll[0] != 'f' && roll[0] != 'F') return false;

    // Positions 1-10 must all be digits
    for (int i = 1; i <= 10; i++) {
        if (roll[i] < '0' || roll[i] > '9') return false;
    }

    return true;
}

// ============================================================================
// validateName — Ensure no digits in name
// ============================================================================
bool validateName(const string& name) {
    if (name.empty()) return false;

    for (int i = 0; i < (int)name.length(); i++) {
        if (name[i] >= '0' && name[i] <= '9') {
            return false;
        }
    }
    return true;
}

// ============================================================================
// containsSubstr — Manual case-insensitive substring search
// ============================================================================
// Iterates through 'haystack' checking for 'needle' at each position
// using character-by-character comparison after lowercasing.
// ============================================================================
bool containsSubstr(const string& haystack, const string& needle) {
    if (needle.empty()) return true;
    if (needle.length() > haystack.length()) return false;

    string hLow = toLower(haystack);
    string nLow = toLower(needle);

    for (int i = 0; i <= (int)hLow.length() - (int)nLow.length(); i++) {
        bool match = true;
        for (int j = 0; j < (int)nLow.length(); j++) {
            if (hLow[i + j] != nLow[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

// ============================================================================
// addStudent — Interactive student registration with full validation
// ============================================================================
bool addStudent() {
    string roll, name, dept, cgpaStr;

    // --- Roll Number ---
    cout << "\n--- Add New Student ---\n";
    cout << "Enter Roll Number (e.g. f2025376200): ";
    getline(cin, roll);
    roll = trim(roll);

    if (!validateRollFormat(roll)) {
        cout << "[ERROR] Invalid roll format. Expected: fYYYYNNNNNN (e.g. f2025376200)\n";
        return false;
    }

    // Check uniqueness
    vector<vector<string>> students = readTXT(STUDENTS_FILE);
    if (rowExists(students, 0, roll)) {
        cout << "[ERROR] Roll number '" << roll << "' already exists.\n";
        return false;
    }

    // --- Name ---
    cout << "Enter Name: ";
    getline(cin, name);
    name = trim(name);

    if (!validateName(name)) {
        cout << "[ERROR] Name must not contain numbers.\n";
        return false;
    }

    // --- Department ---
    cout << "Enter Department: ";
    getline(cin, dept);
    dept = trim(dept);

    if (dept.empty()) {
        cout << "[ERROR] Department cannot be empty.\n";
        return false;
    }

    // --- CGPA ---
    cout << "Enter CGPA (0.0 - 4.0): ";
    getline(cin, cgpaStr);

    double cgpa = 0.0;
    istringstream iss(trim(cgpaStr));
    iss >> cgpa;

    if (iss.fail() || cgpa < 0.0 || cgpa > 4.0) {
        cout << "[ERROR] CGPA must be between 0.0 and 4.0.\n";
        return false;
    }

    // --- Build and append record ---
    Student s;
    s.roll   = roll;
    s.name   = name;
    s.dept   = dept;
    s.cgpa   = cgpa;
    s.status = "active";

    appendTXT(STUDENTS_FILE, studentToRow(s));

    cout << "[SUCCESS] Student '" << name << "' (" << roll
              << ") added successfully.\n";
    return true;
}

// ============================================================================
// searchByRoll — Exact match lookup by roll number
// ============================================================================
bool searchByRoll(const string& roll, Student& outStudent) {
    vector<vector<string>> data = readTXT(STUDENTS_FILE);
    vector<string> row = findRow(data, 0, roll);

    if (row.empty()) {
        return false;
    }

    outStudent = rowToStudent(row);
    return true;
}

// ============================================================================
// searchByName — Substring search returning all matches
// ============================================================================
vector<Student> searchByName(const string& nameQuery) {
    vector<Student> results;
    vector<vector<string>> data = readTXT(STUDENTS_FILE);

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 5) {
            if (containsSubstr(data[i][1], nameQuery)) {
                results.push_back(rowToStudent(data[i]));
            }
        }
    }
    return results;
}

// ============================================================================
// updateStudent — Interactive field editor (roll is immutable)
// ============================================================================
bool updateStudent() {
    string roll;
    cout << "\n--- Update Student ---\n";
    cout << "Enter Roll Number to update: ";
    getline(cin, roll);
    roll = trim(roll);

    vector<vector<string>> data = readTXT(STUDENTS_FILE);
    int targetIdx = -1;

    // Linear search for the target record
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 5 && data[i][0] == roll) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        cout << "[ERROR] Student '" << roll << "' not found.\n";
        return false;
    }

    Student s = rowToStudent(data[targetIdx]);
    cout << "Current: " << s.name << " | " << s.dept
              << " | CGPA: " << s.cgpa << "\n";

    // --- Name Update ---
    string input;
    cout << "New Name (press Enter to keep '" << s.name << "'): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        if (!validateName(input)) {
            cout << "[ERROR] Name must not contain numbers. Keeping old name.\n";
        } else {
            s.name = input;
        }
    }

    // --- Department Update ---
    cout << "New Department (press Enter to keep '" << s.dept << "'): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        s.dept = input;
    }

    // --- CGPA Update ---
    cout << "New CGPA (press Enter to keep " << doubleToStr(s.cgpa, 2) << "): ";
    getline(cin, input);
    input = trim(input);
    if (!input.empty()) {
        double newCgpa = 0.0;
        istringstream iss(input);
        iss >> newCgpa;
        if (iss.fail() || newCgpa < 0.0 || newCgpa > 4.0) {
            cout << "[ERROR] Invalid CGPA. Keeping old value.\n";
        } else {
            s.cgpa = newCgpa;
        }
    }

    // --- Re-serialize ---
    data[targetIdx] = studentToRow(s);
    writeTXT(STUDENTS_FILE, "roll,name,dept,cgpa,status", data);

    cout << "[SUCCESS] Student '" << roll << "' updated.\n";
    return true;
}

// ============================================================================
// softDelete — Set status to "inactive" (no physical deletion)
// ============================================================================
bool softDelete(const string& roll) {
    vector<vector<string>> data = readTXT(STUDENTS_FILE);

    bool found = false;
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 5 && data[i][0] == roll) {
            if (toLower(data[i][4]) == "inactive") {
                cout << "[INFO] Student '" << roll
                          << "' is already inactive.\n";
                return false;
            }
            data[i][4] = "inactive";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "[ERROR] Student '" << roll << "' not found.\n";
        return false;
    }

    writeTXT(STUDENTS_FILE, "roll,name,dept,cgpa,status", data);
    cout << "[SUCCESS] Student '" << roll << "' deactivated (soft-deleted).\n";
    return true;
}

// ============================================================================
// getAllActiveStudents — Returns active students (no console output)
// ============================================================================
vector<Student> getAllActiveStudents() {
    vector<Student> active;
    vector<vector<string>> data = readTXT(STUDENTS_FILE);

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 5 && toLower(data[i][4]) == "active") {
            active.push_back(rowToStudent(data[i]));
        }
    }
    return active;
}

// ============================================================================
// listActiveStudents — Print sorted table of active students
// ============================================================================
// Uses manual Selection Sort on roll numbers (alphanumeric ascending).
// ============================================================================
void listActiveStudents() {
    vector<Student> active = getAllActiveStudents();

    if (active.empty()) {
        cout << "\n[INFO] No active students found.\n";
        return;
    }

    // ---- Manual Selection Sort by roll number (ascending) ----
    for (int i = 0; i < (int)active.size() - 1; i++) {
        int minIdx = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j].roll < active[minIdx].roll) {
                minIdx = j;
            }
        }
        if (minIdx != i) {
            // Manual swap
            Student temp = active[i];
            active[i] = active[minIdx];
            active[minIdx] = temp;
        }
    }

    // ---- Print formatted table ----
    cout << "\n"
              << string(72, '=') << "\n"
              << "  ACTIVE STUDENT DIRECTORY\n"
              << string(72, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(8)  << "Dept"
              << setw(8)  << "CGPA"
              << setw(10) << "Status"
              << "\n";
    cout << string(72, '-') << "\n";

    for (int i = 0; i < (int)active.size(); i++) {
        cout << left
                  << setw(4)  << (i + 1)
                  << setw(14) << active[i].roll
                  << setw(22) << active[i].name
                  << setw(8)  << active[i].dept
                  << setw(8)  << doubleToStr(active[i].cgpa, 2)
                  << setw(10) << active[i].status
                  << "\n";
    }

    cout << string(72, '-') << "\n"
              << "  Total Active Students: " << active.size() << "\n"
              << string(72, '=') << "\n";
}
