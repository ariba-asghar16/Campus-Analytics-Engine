// ============================================================================
// attendance.h — M4: Registry Tracking and Recovery State Snapshots
// ============================================================================
// Manages attendance marking, percentage computation, shortage detection,
// daily sheet printing, and undo functionality via in-memory snapshots.
// Depends on M1 (filehandler) and M3 (course_ops).
//
// CONSTRAINTS ENFORCED:
//   - Status values: "P" (Present), "A" (Absent), "L" (Late)
//   - Dates in DD-MM-YYYY format (no <ctime>)
//   - Manual accumulator loops for percentage computation
//   - Undo via vector snapshot backup/restore
// ============================================================================

#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <string>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// AttendanceRecord Struct — Single attendance entry
// ----------------------------------------------------------------------------
struct AttendanceRecord {
    string roll;       // Student roll number
    string courseCode;  // Course code
    string date;       // DD-MM-YYYY
    string status;     // "P", "A", or "L"
};

// ----------------------------------------------------------------------------
// Conversion Utilities
// ----------------------------------------------------------------------------
AttendanceRecord rowToAttendance(const vector<string>& row);
vector<string> attendanceToRow(const AttendanceRecord& a);

// ----------------------------------------------------------------------------
// markAttendance
// ----------------------------------------------------------------------------
// Interactive: prompts for a course code, semester, and date. Retrieves
// all enrolled students for that course+semester, then loops through each
// student prompting for P/A/L input.
//
// Before writing, saves a snapshot of the current attendance data for
// potential undo. Appends all new records to attendance_log.txt.
// ----------------------------------------------------------------------------
void markAttendance();

// ----------------------------------------------------------------------------
// getAttendancePct
// ----------------------------------------------------------------------------
// Computes attendance percentage for a specific student+course using:
//   Formula: (Present + 0.5 * Late) / TotalSessions * 100.0
// Returns the percentage as a double. Returns -1.0 if no sessions found.
// ----------------------------------------------------------------------------
double getAttendancePct(const string& roll,
                        const string& courseCode);

// ----------------------------------------------------------------------------
// getShortageList
// ----------------------------------------------------------------------------
// Returns a vector of roll numbers whose attendance percentage in the
// given course is strictly below 75%.
// ----------------------------------------------------------------------------
vector<string> getShortageList(const string& courseCode);

// ----------------------------------------------------------------------------
// undoLastSession
// ----------------------------------------------------------------------------
// Checks for a valid snapshot. If one exists, restores the attendance
// file to the snapshot state and clears the snapshot.
// Returns true if undo was performed successfully.
// ----------------------------------------------------------------------------
bool undoLastSession();

// ----------------------------------------------------------------------------
// printDailySheet
// ----------------------------------------------------------------------------
// Prints a formatted console table showing all attendance records for
// a specific course on a specific date.
// ----------------------------------------------------------------------------
void printDailySheet(const string& courseCode,
                     const string& date);

// ----------------------------------------------------------------------------
// printDailySheetInteractive
// ----------------------------------------------------------------------------
// Interactive wrapper: prompts for course code and date, then calls
// printDailySheet().
// ----------------------------------------------------------------------------
void printDailySheetInteractive();

// ----------------------------------------------------------------------------
// viewAttendancePctInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for roll and course, displays attendance percentage.
// ----------------------------------------------------------------------------
void viewAttendancePctInteractive();

// ----------------------------------------------------------------------------
// viewShortageListInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for course code, displays all shortage students.
// ----------------------------------------------------------------------------
void viewShortageListInteractive();

#endif // ATTENDANCE_H
