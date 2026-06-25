// ============================================================================
// attendance.cpp — M4: Registry Tracking and Recovery Implementation
// ============================================================================

#include "attendance.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

using namespace std;


// ============================================================================
// Module-level snapshot for undo functionality
// ============================================================================
// Stores a complete copy of the attendance data BEFORE the last marking
// session. If undo is invoked, this snapshot overwrites the file.
// ============================================================================
static vector<vector<string>> attendanceSnapshot;
static bool snapshotValid = false;

// ============================================================================
// Conversion Utilities
// ============================================================================

AttendanceRecord rowToAttendance(const vector<string>& row) {
    AttendanceRecord a;
    a.roll       = "";
    a.courseCode  = "";
    a.date       = "";
    a.status     = "";

    if (row.size() >= 4) {
        a.roll       = row[0];
        a.courseCode  = row[1];
        a.date       = row[2];
        a.status     = row[3];
    }
    return a;
}

vector<string> attendanceToRow(const AttendanceRecord& a) {
    vector<string> row;
    row.push_back(a.roll);
    row.push_back(a.courseCode);
    row.push_back(a.date);
    row.push_back(a.status);
    return row;
}

// ============================================================================
// validateDateFormat — Manual DD-MM-YYYY format validation
// ============================================================================
// Checks length == 10, positions 2 and 5 are '-', all others are digits.
// Also validates day/month ranges using a month-day boundary array.
// ============================================================================
static bool validateDateFormat(const string& date) {
    if (date.length() != 10) return false;

    // Check delimiters at positions 2 and 5
    if (date[2] != '-' || date[5] != '-') return false;

    // Check digits at all other positions
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }

    // Parse day, month, year
    int day   = (date[0] - '0') * 10 + (date[1] - '0');
    int month = (date[3] - '0') * 10 + (date[4] - '0');
    int year  = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
                (date[8] - '0') * 10   + (date[9] - '0');

    if (month < 1 || month > 12) return false;

    // Month-day boundaries (non-leap year default)
    int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Leap year check
    bool isLeap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    if (isLeap) daysInMonth[1] = 29;

    if (day < 1 || day > daysInMonth[month - 1]) return false;

    return true;
}

// ============================================================================
// markAttendance — Interactive bulk attendance marking with snapshot
// ============================================================================
void markAttendance() {
    string courseCode, semester, date;

    cout << "\n--- Mark Attendance ---\n";
    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    cout << "Enter Date (DD-MM-YYYY): ";
    getline(cin, date);
    date = trim(date);

    if (!validateDateFormat(date)) {
        cout << "[ERROR] Invalid date format. Use DD-MM-YYYY.\n";
        return;
    }


    // Manually collect enrolled students for this course+semester
    vector<vector<string>> enrollData = readTXT(ENROLLMENTS_FILE);
    vector<string> enrolledRolls;

    for (int i = 0; i < (int)enrollData.size(); i++) {
        if (enrollData[i].size() >= 4 &&
            enrollData[i][1] == courseCode &&
            enrollData[i][2] == semester &&
            toLower(enrollData[i][3]) == "enrolled") {
            enrolledRolls.push_back(enrollData[i][0]);
        }
    }

    if (enrolledRolls.empty()) {
        cout << "[INFO] No enrolled students for " << courseCode
                  << " in " << semester << ".\n";
        return;
    }

    // --- Save snapshot BEFORE writing (for undo) ---
    attendanceSnapshot = readTXT(ATTENDANCE_FILE);
    snapshotValid = true;

    // Collect attendance inputs
    vector<AttendanceRecord> newRecords;

    cout << "\nMarking attendance for " << courseCode << " on " << date << ":\n";
    cout << "Enter P (Present), A (Absent), or L (Late) for each student.\n\n";

    for (int i = 0; i < (int)enrolledRolls.size(); i++) {
        Student s;
        string displayName = enrolledRolls[i];
        if (searchByRoll(enrolledRolls[i], s)) {
            displayName = s.name + " (" + enrolledRolls[i] + ")";
        }

        string input;
        bool valid = false;
        while (!valid) {
            cout << "  " << setw(3) << (i + 1) << ". "
                      << setw(35) << left << displayName << " [P/A/L]: ";
            getline(cin, input);
            input = trim(input);

            // Validate P/A/L
            if (input.length() == 1) {
                char c = input[0];
                if (c >= 'a' && c <= 'z') c -= ('a' - 'A'); // to uppercase
                if (c == 'P' || c == 'A' || c == 'L') {
                    AttendanceRecord rec;
                    rec.roll       = enrolledRolls[i];
                    rec.courseCode  = courseCode;
                    rec.date       = date;
                    rec.status     = string(1, c);
                    newRecords.push_back(rec);
                    valid = true;
                }
            }
            if (!valid) {
                cout << "        [ERROR] Enter P, A, or L only.\n";
            }
        }
    }

    // Append all records to file
    for (int i = 0; i < (int)newRecords.size(); i++) {
        appendTXT(ATTENDANCE_FILE, attendanceToRow(newRecords[i]));
    }

    cout << "\n[SUCCESS] Attendance marked for " << newRecords.size()
              << " student(s) in " << courseCode << " on " << date << ".\n";
}

// ============================================================================
// getAttendancePct — Compute (P + 0.5*L) / Total * 100.0
// ============================================================================
double getAttendancePct(const string& roll,
                        const string& courseCode) {
    vector<vector<string>> data = readTXT(ATTENDANCE_FILE);

    int totalSessions = 0;
    double presentScore = 0.0;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 4 &&
            data[i][0] == roll &&
            data[i][1] == courseCode) {

            totalSessions++;
            string status = trim(data[i][3]);

            if (status == "P" || status == "p") {
                presentScore += 1.0;
            } else if (status == "L" || status == "l") {
                presentScore += 0.5;
            }
            // "A" adds 0
        }
    }

    if (totalSessions == 0) return -1.0;

    return (presentScore / totalSessions) * 100.0;
}

// ============================================================================
// getShortageList — Collect students below 75% in a course
// ============================================================================
vector<string> getShortageList(const string& courseCode) {
    vector<string> shortageRolls;

    // Find unique rolls enrolled in this course
    vector<vector<string>> enrollData = readTXT(ENROLLMENTS_FILE);
    vector<string> uniqueRolls;

    for (int i = 0; i < (int)enrollData.size(); i++) {
        if (enrollData[i].size() >= 4 &&
            enrollData[i][1] == courseCode &&
            toLower(enrollData[i][3]) == "enrolled") {

            // Check uniqueness using manual loop
            bool duplicate = false;
            for (int j = 0; j < (int)uniqueRolls.size(); j++) {
                if (uniqueRolls[j] == enrollData[i][0]) {
                    duplicate = true;
                    break;
                }
            }
            if (!duplicate) {
                uniqueRolls.push_back(enrollData[i][0]);
            }
        }
    }

    // Check each student's attendance percentage
    for (int i = 0; i < (int)uniqueRolls.size(); i++) {
        double pct = getAttendancePct(uniqueRolls[i], courseCode);
        if (pct >= 0.0 && pct < 75.0) {
            shortageRolls.push_back(uniqueRolls[i]);
        }
    }

    return shortageRolls;
}

// ============================================================================
// undoLastSession — Restore from snapshot
// ============================================================================
bool undoLastSession() {
    if (!snapshotValid) {
        cout << "[ERROR] No attendance snapshot available for undo.\n";
        return false;
    }

    // Overwrite the file with the snapshot data
    writeTXT(ATTENDANCE_FILE,
             "roll,course_code,date,status",
             attendanceSnapshot);

    snapshotValid = false;
    attendanceSnapshot.clear();

    cout << "[SUCCESS] Last attendance session undone. File restored.\n";
    return true;
}

// ============================================================================
// printDailySheet — Console table for a specific date+course
// ============================================================================
void printDailySheet(const string& courseCode,
                     const string& date) {
    vector<vector<string>> data = readTXT(ATTENDANCE_FILE);

    cout << "\n"
              << string(60, '=') << "\n"
              << "  Daily Attendance Sheet: " << courseCode
              << " | Date: " << date << "\n"
              << string(60, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(10) << "Status"
              << "\n";
    cout << string(60, '-') << "\n";

    int count = 0;
    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 4 &&
            data[i][1] == courseCode &&
            data[i][2] == date) {

            count++;
            Student s;
            string name = data[i][0];
            if (searchByRoll(data[i][0], s)) {
                name = s.name;
            }

            string statusLabel;
            if (data[i][3] == "P") statusLabel = "Present";
            else if (data[i][3] == "A") statusLabel = "Absent";
            else if (data[i][3] == "L") statusLabel = "Late";
            else statusLabel = data[i][3];

            cout << left
                      << setw(4)  << count
                      << setw(14) << data[i][0]
                      << setw(22) << name
                      << setw(10) << statusLabel
                      << "\n";
        }
    }

    if (count == 0) {
        cout << "  No records found.\n";
    }

    cout << string(60, '-') << "\n"
              << "  Total: " << count << " record(s)\n"
              << string(60, '=') << "\n";
}

// ============================================================================
// printDailySheetInteractive
// ============================================================================
void printDailySheetInteractive() {
    string courseCode, date;
    cout << "\n--- Daily Attendance Sheet ---\n";
    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    cout << "Enter Date (DD-MM-YYYY): ";
    getline(cin, date);
    date = trim(date);

    printDailySheet(courseCode, date);
}

// ============================================================================
// viewAttendancePctInteractive
// ============================================================================
void viewAttendancePctInteractive() {
    string roll, courseCode;
    cout << "\n--- View Attendance Percentage ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    double pct = getAttendancePct(roll, courseCode);
    if (pct < 0) {
        cout << "[INFO] No attendance records found for " << roll
                  << " in " << courseCode << ".\n";
    } else {
        cout << "\n  Attendance for " << roll << " in " << courseCode
                  << ": " << doubleToStr(pct, 1) << "%";
        if (pct < 75.0) {
            cout << "  [SHORTAGE]";
        }
        cout << "\n";
    }
}

// ============================================================================
// viewShortageListInteractive
// ============================================================================
void viewShortageListInteractive() {
    string courseCode;
    cout << "\n--- Attendance Shortage List ---\n";
    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    vector<string> shortList = getShortageList(courseCode);

    if (shortList.empty()) {
        cout << "[INFO] No students below 75% attendance in "
                  << courseCode << ".\n";
        return;
    }

    cout << "\n"
              << string(55, '=') << "\n"
              << "  Shortage List: " << courseCode << " (Below 75%)\n"
              << string(55, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(12) << "Attendance"
              << "\n";
    cout << string(55, '-') << "\n";

    for (int i = 0; i < (int)shortList.size(); i++) {
        Student s;
        string name = shortList[i];
        if (searchByRoll(shortList[i], s)) {
            name = s.name;
        }
        double pct = getAttendancePct(shortList[i], courseCode);

        cout << left
                  << setw(4)  << (i + 1)
                  << setw(14) << shortList[i]
                  << setw(22) << name
                  << setw(12) << (doubleToStr(pct, 1) + "%")
                  << "\n";
    }

    cout << string(55, '-') << "\n"
              << "  Total: " << shortList.size() << " student(s)\n"
              << string(55, '=') << "\n";
}
