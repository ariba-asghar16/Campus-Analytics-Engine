// ============================================================================
// main.cpp — System Entry Point and Nested Menu Router
// ============================================================================
#include<conio.h>
#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include "reports.h"
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
using namespace std;


// Forward Declarations of Submenus
void studentMenu();
void courseMenu();
void attendanceMenu();
void gradesMenu();
void feeMenu();
void reportsMenu();
void searchAsYouType();



// Helper to check if a target field contains a query using substr and length
bool matchesQuery(const string& field, const string& query) {
    if (query.empty()) return true;
    if (query.length() > field.length()) return false;

    string fLow = toLower(field);
    string qLow = toLower(query);

    for (int i = 0; i <= (int)fLow.length() - (int)qLow.length(); i++) {
        if (fLow.substr(i, qLow.length()) == qLow) {
            return true;
        }
    }
    return false;
}

// ============================================================================
// main — Main router loop (Level 1)
// ============================================================================
int main() {
    string choiceStr;

    while (true) {
        cout << "\n==================================================\n"
                  << "         CAMPUS ANALYTICS ENGINE - MAIN MENU       \n"
                  << "==================================================\n"
                  << "  1. Student Directory Management\n"
                  << "  2. Course & Enrollment Management\n"
                  << "  3. Attendance Logging System\n"
                  << "  4. Grading & Class Performance\n"
                  << "  5. Student Financial Ledger\n"
                  << "  6. Enterprise Analytics & Reports\n"
                  << "  7. Search Active Students (Keystroke Search)\n"
                  << "  8. Exit System\n"
                  << "--------------------------------------------------\n"
                  << "Enter Choice (1-8): ";

        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 8) {
            cout << "\nExiting Campus Analytics Engine. Goodbye!\n";
            break;
        }

        switch (choice) {
            case 1:
                studentMenu();
                break;
            case 2:
                courseMenu();
                break;
            case 3:
                attendanceMenu();
                break;
            case 4:
                gradesMenu();
                break;
            case 5:
                feeMenu();
                break;
            case 6:
                reportsMenu();
                break;
            case 7:
                searchAsYouType();
                break;
            default:
                cout << "[ERROR] Invalid choice. Please enter 1-8.\n";
                break;
        }
    }

    return 0;
}

// ============================================================================
// studentMenu — Level 2: Student Lifecycle Operations
// ============================================================================
void studentMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Student Directory Management ---\n"
                  << "  1. Register New Student\n"
                  << "  2. Update Student Information\n"
                  << "  3. Deactivate Student Record (Soft Delete)\n"
                  << "  4. List Active Students (Sorted by Roll)\n"
                  << "  5. Go Back to Main Menu\n"
                  << "------------------------------------\n"
                  << "Enter Choice (1-5): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 5) break;

        switch (choice) {
            case 1:
                addStudent();
                break;
            case 2:
                updateStudent();
                break;
            case 3: {
                cout << "Enter Roll Number to Deactivate: ";
                string roll;
                getline(cin, roll);
                softDelete(trim(roll));
                break;
            }
            case 4:
                listActiveStudents();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-5.\n";
                break;
        }
    }
}

// ============================================================================
// courseMenu — Level 2: Enrollment Lifecycle Operations
// ============================================================================
void courseMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Course & Enrollment Management ---\n"
                  << "  1. Enroll Student in Course\n"
                  << "  2. Drop Student Course\n"
                  << "  3. List Course Registrations (Roster)\n"
                  << "  4. Go Back to Main Menu\n"
                  << "--------------------------------------\n"
                  << "Enter Choice (1-4): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 4) break;

        switch (choice) {
            case 1:
                enrollStudentInteractive();
                break;
            case 2:
                dropCourseInteractive();
                break;
            case 3:
                listEnrolledStudentsInteractive();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-4.\n";
                break;
        }
    }
}

// ============================================================================
// attendanceMenu — Level 2: Attendance Operations
// ============================================================================
void attendanceMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Attendance Logging System ---\n"
                  << "  1. Mark Daily Attendance\n"
                  << "  2. Print Daily Attendance Sheet\n"
                  << "  3. View Student Attendance Percentage\n"
                  << "  4. View Course Attendance Shortage (Below 75%)\n"
                  << "  5. Undo Last Attendance Marking Session\n"
                  << "  6. Go Back to Main Menu\n"
                  << "-----------------------------------\n"
                  << "Enter Choice (1-6): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 6) break;

        switch (choice) {
            case 1:
                markAttendance();
                break;
            case 2:
                printDailySheetInteractive();
                break;
            case 3:
                viewAttendancePctInteractive();
                break;
            case 4:
                viewShortageListInteractive();
                break;
            case 5:
                undoLastSession();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-6.\n";
                break;
        }
    }
}

// ============================================================================
// gradesMenu — Level 2: Performance Evaluation Operations
// ============================================================================
void gradesMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Grading & Class Performance ---\n"
                  << "  1. Enter Student Marks\n"
                  << "  2. View Student Grades Breakdown\n"
                  << "  3. View Semester GPA\n"
                  << "  4. View Course Statistics (Mean/Median/High/Low)\n"
                  << "  5. Go Back to Main Menu\n"
                  << "-----------------------------------\n"
                  << "Enter Choice (1-5): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 5) break;

        switch (choice) {
            case 1:
                enterMarks();
                break;
            case 2:
                viewGradesInteractive();
                break;
            case 3:
                viewGPAInteractive();
                break;
            case 4:
                viewClassStatsInteractive();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-5.\n";
                break;
        }
    }
}

// ============================================================================
// feeMenu — Level 2: Financial Ledger Operations
// ============================================================================
void feeMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Student Financial Ledger ---\n"
                  << "  1. Record Fee Payment\n"
                  << "  2. Generate Fee Receipt/Invoice\n"
                  << "  3. View Fee Defaulters List\n"
                  << "  4. Go Back to Main Menu\n"
                  << "--------------------------------\n"
                  << "Enter Choice (1-4): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 4) break;

        switch (choice) {
            case 1:
                recordPayment();
                break;
            case 2:
                generateReceiptInteractive();
                break;
            case 3:
                viewDefaultersInteractive();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-4.\n";
                break;
        }
    }
}

// ============================================================================
// reportsMenu — Level 2: Analytics and Exports Router
// ============================================================================
void reportsMenu() {
    string choiceStr;
    while (true) {
        cout << "\n--- Enterprise Analytics & Reports ---\n"
                  << "  1. Print Merit List (descending CGPA)\n"
                  << "  2. Print Attendance Defaulters List\n"
                  << "  3. Print Fee Defaulters List\n"
                  << "  4. Print Semester Transcript\n"
                  << "  5. Print Department Summary\n"
                  << "  6. Export Report to File\n"
                  << "  7. Go Back to Main Menu\n"
                  << "--------------------------------------\n"
                  << "Enter Choice (1-7): ";
        getline(cin, choiceStr);
        int choice = 0;
        istringstream iss(trim(choiceStr));
        iss >> choice;

        if (choice == 7) break;

        switch (choice) {
            case 1:
                printMeritList();
                break;
            case 2:
                printAttendanceDefaulters();
                break;
            case 3:
                printFeeDefaulters();
                break;
            case 4:
                printSemesterResult();
                break;
            case 5:
                printDepartmentSummary();
                break;
            case 6:
                exportReportToFile();
                break;
            default:
                cout << "[ERROR] Invalid choice. Enter 1-7.\n";
                break;
        }
    }
}

// ============================================================================
// searchAsYouType — Keystroke search console query loop (Bonus)
// ============================================================================
void searchAsYouType() {
    string query = "";
    vector<Student> active = getAllActiveStudents();

    while (true) {
        // Clear screen (Linux)
        system("clear");

        cout << "========================================================\n";
        cout << "          SEARCH ACTIVE STUDENTS (Keystroke Search)     \n";
        cout << "========================================================\n";
        cout << "  Type characters to search (Esc to exit, Backspace to delete)\n";
        cout << "  Query: " << query << "\n";
        cout << "--------------------------------------------------------\n";

        // Print header
        cout << left
                  << setw(14) << "Roll"
                  << setw(22) << "Name"
                  << setw(8)  << "Dept"
                  << setw(8)  << "CGPA"
                  << "\n";
        cout << string(55, '-') << "\n";

        int matchCount = 0;
        for (int i = 0; i < (int)active.size(); i++) {
            if (matchesQuery(active[i].roll, query) || matchesQuery(active[i].name, query)) {
                matchCount++;
                cout << left
                          << setw(14) << active[i].roll
                          << setw(22) << active[i].name
                          << setw(8)  << active[i].dept
                          << setw(8)  << doubleToStr(active[i].cgpa, 2)
                          << "\n";
            }
        }

        cout << string(55, '-') << "\n";
        cout << "  Matches: " << matchCount << "\n";
        cout << "========================================================\n";

        int ch = _getch();
        if (ch == 27) { // ESC key
            break;
        } else if (ch == 8 || ch == 127) { // Backspace
            if (!query.empty()) {
                query.pop_back();
            }
        } else if (ch >= 32 && ch <= 126) { // Printable chars
            query += (char)ch;
        }
    }
}
