// ============================================================================
// reports.h — M7: Enterprise Output Compiler and Dynamic File Redirections
// ============================================================================
// Generates comprehensive system reports: merit lists, attendance/fee
// defaulters, semester results, department summaries, and exports to file.
// Depends on M5 (grades), M4 (attendance), M6 (fee_tracker).
//
// CONSTRAINTS ENFORCED:
//   - Manual sorting loops for merit lists (no <algorithm>)
//   - Stream redirection via cout.rdbuf() for file export
//   - Nested parallel vectors for department grouping
// ============================================================================

#ifndef REPORTS_H
#define REPORTS_H

#include <string>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// printMeritList
// ----------------------------------------------------------------------------
// Prints all active students ordered by CGPA in descending order.
// Uses manual sorting loops (Selection Sort). Displays ranking numbers.
// ----------------------------------------------------------------------------
void printMeritList();

// ----------------------------------------------------------------------------
// printAttendanceDefaulters
// ----------------------------------------------------------------------------
// Identifies students below 75% attendance in ANY course.
// Outputs a tabular report with roll, name, course, and percentage.
// ----------------------------------------------------------------------------
void printAttendanceDefaulters();

// ----------------------------------------------------------------------------
// printFeeDefaulters
// ----------------------------------------------------------------------------
// Displays fee defaulter report showing outstanding fees and late weeks.
// Prompts for a reference date.
// ----------------------------------------------------------------------------
void printFeeDefaulters();

// ----------------------------------------------------------------------------
// printSemesterResult
// ----------------------------------------------------------------------------
// Prints full semester transcripts for a specific student and semester.
// Includes course grades, GPA, and attendance status per course.
// Formatted with console border indicators.
// ----------------------------------------------------------------------------
void printSemesterResult();

// ----------------------------------------------------------------------------
// printDepartmentSummary
// ----------------------------------------------------------------------------
// Groups students by department using nested parallel vectors.
// For each department, displays:
//   - Total registrations
//   - Average CGPA
//   - Passing rate (students with CGPA >= 2.0)
// ----------------------------------------------------------------------------
void printDepartmentSummary();

// ----------------------------------------------------------------------------
// exportReportToFile
// ----------------------------------------------------------------------------
// Interactive: prompts for report type and output filename.
// Temporarily redirects cout to an ofstream via rdbuf() swap,
// executes the selected report function, then restores cout to default.
// ----------------------------------------------------------------------------
void exportReportToFile();

#endif // REPORTS_H
