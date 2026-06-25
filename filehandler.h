// ============================================================================
// filehandler.h — M1: Low-Level Parsing Engine for TXT File Operations
// ============================================================================
// Provides the foundational I/O layer for the Campus Analytics Engine.
// All modules depend on these functions for reading, writing, and searching
// flat-file text databases.
//
// CONSTRAINTS ENFORCED:
//   - No <algorithm>, no <map>, no <set>, no <ctime>
//   - CSV parsing is char-by-char (no getline token splitters)
//   - All data persisted in comma-separated .txt files
// ============================================================================

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <string>
#include <vector>

using namespace std;
// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
const string DATA_DIR = "data/";

const string STUDENTS_FILE    = DATA_DIR + "students.txt";
const string COURSES_FILE     = DATA_DIR + "courses.txt";
const string ENROLLMENTS_FILE = DATA_DIR + "enrollments.txt";
const string ATTENDANCE_FILE  = DATA_DIR + "attendance_log.txt";
const string FEES_FILE        = DATA_DIR + "fees.txt";
 const string GRADES_FILE      = DATA_DIR + "grades.txt";

// ----------------------------------------------------------------------------
// readTXT
// ----------------------------------------------------------------------------
// Opens the file at 'filePath', skips the first row (header line), and parses
// every subsequent line into a vector of fields using a character-by-character
// scanning loop. Handles quoted fields containing embedded commas.
//
// Returns: A 2D vector where each inner vector is one parsed row of fields.
// ----------------------------------------------------------------------------
vector<vector<string>> readTXT(const string &filePath);

// ----------------------------------------------------------------------------
// writeTXT
// ----------------------------------------------------------------------------
// Opens 'filePath' for explicit overwriting (truncation). Writes 'header' as
// the first line, then serializes each row in 'data' as a comma-separated
// line. If any field contains a comma, it is wrapped in double quotes.
// ----------------------------------------------------------------------------
void writeTXT(const string &filePath, const string &header,
              const vector<vector<string>> &data);

// ----------------------------------------------------------------------------
// appendTXT
// ----------------------------------------------------------------------------
// Opens 'filePath' in append mode and writes a single record line without
// loading the entire file back into memory. Fields containing commas are
// quoted. Flushes immediately after writing.
// ----------------------------------------------------------------------------
void appendTXT(const string &filePath, const vector<string> &row);

// ----------------------------------------------------------------------------
// findRow
// ----------------------------------------------------------------------------
// Performs a linear search through 'data' rows. Returns the first row where
// the field at column index 'colIndex' exactly matches 'value'.
// If no match is found, returns an empty vector.
// ----------------------------------------------------------------------------
vector<string> findRow(const vector<vector<string>> &data, int colIndex,
                       const string &value);

// ----------------------------------------------------------------------------
// findAllRows
// ----------------------------------------------------------------------------
// Returns ALL rows where the field at 'colIndex' matches 'value'.
// Used when multiple records share the same key (e.g., enrollments per
// student).
// ----------------------------------------------------------------------------
vector<vector<string>> findAllRows(const vector<vector<string>> &data,
                                   int colIndex, const string &value);

// ----------------------------------------------------------------------------
// rowExists
// ----------------------------------------------------------------------------
// Checks whether any row in 'data' has a field at 'colIndex' that exactly
// matches 'value'. Returns true/false without extracting the row.
// ----------------------------------------------------------------------------
bool rowExists(const vector<vector<string>> &data, int colIndex,
               const string &value);

// ----------------------------------------------------------------------------
// toLower
// ----------------------------------------------------------------------------
// Utility: converts a string to lowercase using manual character offsets.
// Used for case-insensitive comparisons throughout the engine.
// ----------------------------------------------------------------------------
string toLower(const string &s);

// ----------------------------------------------------------------------------
// trim
// ----------------------------------------------------------------------------
// Utility: strips leading and trailing whitespace from a string.
// ----------------------------------------------------------------------------
string trim(const string &s);

// ----------------------------------------------------------------------------
// doubleToStr
// ----------------------------------------------------------------------------
// Utility: converts a double to a string with a fixed number of decimal
// places. Used for serializing CGPA and monetary amounts.
// ----------------------------------------------------------------------------
string doubleToStr(double val, int decimals = 2);

#endif // FILEHANDLER_H
