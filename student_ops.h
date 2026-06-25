// ============================================================================
// student_ops.h — M2: Student Directory Mutation and Filtering
// ============================================================================
// Manages the student lifecycle: creation, search, update, soft-delete,
// and sorted listing. All operations interact with students.txt via M1.
//
// CONSTRAINTS ENFORCED:
//   - Roll format validated via manual char-loop (no regex)
//   - Name must contain zero digits
//   - CGPA bounded [0.0, 4.0]
//   - Selection Sort on roll numbers (manual, no <algorithm>)
// ============================================================================

#ifndef STUDENT_OPS_H
#define STUDENT_OPS_H

#include <string>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// Student Struct — Flat data container (no OOP)
// ----------------------------------------------------------------------------
struct Student {
    string roll;       // Format: fYYYYNNNNNN (e.g. f2025376200)
    string name;       // Alphabetic only (no digits)
    string dept;       // Department code (e.g., "AI")
    double cgpa;            // Range: [0.0, 4.0]
    string status;     // "active" or "inactive"
};

// ----------------------------------------------------------------------------
// Conversion Utilities
// ----------------------------------------------------------------------------

// Parses a CSV row vector into a Student struct.
Student rowToStudent(const vector<string>& row);

// Serializes a Student struct into a CSV row vector.
vector<string> studentToRow(const Student& s);

// ----------------------------------------------------------------------------
// addStudent
// ----------------------------------------------------------------------------
// Interactive function: prompts user for roll, name, dept, cgpa.
// Validates:
//   1. Roll matches BSAI-YY-XXX pattern via manual character offsets
//   2. Roll is unique (not already in students.txt)
//   3. Name contains no digits
//   4. CGPA is in [0.0, 4.0]
// On success, appends the new record to students.txt with status="active".
// Returns true if the student was successfully added.
// ----------------------------------------------------------------------------
bool addStudent();

// ----------------------------------------------------------------------------
// validateRollFormat
// ----------------------------------------------------------------------------
// Checks that 'roll' matches the pattern fYYYYNNNNNN where:
//   - Position 0: 'f' or 'F'
//   - Positions 1-4: four digits (year, e.g. 2025)
//   - Positions 5-10: six digits (student ID)
// Total length must be exactly 11 characters.
// Returns true if valid.
// ----------------------------------------------------------------------------
bool validateRollFormat(const string& roll);

// ----------------------------------------------------------------------------
// validateName
// ----------------------------------------------------------------------------
// Ensures 'name' contains no digit characters (0-9).
// Allows letters, spaces, hyphens, and periods.
// Returns true if valid.
// ----------------------------------------------------------------------------
bool validateName(const string& name);

// ----------------------------------------------------------------------------
// searchByRoll
// ----------------------------------------------------------------------------
// Searches students.txt for a record matching 'roll'.
// If found, populates the output Student struct and returns true.
// If not found, returns false and the struct contains default values.
// ----------------------------------------------------------------------------
bool searchByRoll(const string& roll, Student& outStudent);

// ----------------------------------------------------------------------------
// searchByName
// ----------------------------------------------------------------------------
// Builds and returns a vector of all Student records whose names contain
// 'nameQuery' as a case-insensitive substring. Uses manual character loops
// for substring matching.
// ----------------------------------------------------------------------------
vector<Student> searchByName(const string& nameQuery);

// ----------------------------------------------------------------------------
// updateStudent
// ----------------------------------------------------------------------------
// Interactive function: prompts for roll number, locates the record, and
// allows editing of name, dept, and cgpa (roll cannot be changed).
// Re-serializes the entire students.txt file after modification.
// Returns true if the update was successful.
// ----------------------------------------------------------------------------
bool updateStudent();

// ----------------------------------------------------------------------------
// softDelete
// ----------------------------------------------------------------------------
// Changes the status of the student with 'roll' from "active" to "inactive".
// Does NOT physically remove the record from the file.
// Returns true if the student was found and deactivated.
// ----------------------------------------------------------------------------
bool softDelete(const string& roll);

// ----------------------------------------------------------------------------
// listActiveStudents
// ----------------------------------------------------------------------------
// Extracts all students with status="active", sorts them by roll number
// using a manual Selection Sort, and prints a formatted table to console.
// ----------------------------------------------------------------------------
void listActiveStudents();

// ----------------------------------------------------------------------------
// getAllActiveStudents
// ----------------------------------------------------------------------------
// Returns a vector of all active Student structs (no console output).
// Used internally by other modules.
// ----------------------------------------------------------------------------
vector<Student> getAllActiveStudents();

// ----------------------------------------------------------------------------
// containsSubstr
// ----------------------------------------------------------------------------
// Manual case-insensitive substring check. Returns true if 'haystack'
// contains 'needle'. Uses only loops and character comparisons.
// ----------------------------------------------------------------------------
bool containsSubstr(const string& haystack, const string& needle);

#endif // STUDENT_OPS_H
