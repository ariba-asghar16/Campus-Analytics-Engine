// ============================================================================
// reports.cpp — M7: Enterprise Output Compiler and Dynamic File Redirections
// ============================================================================

#include "reports.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include "student_ops.h"
#include "course_ops.h"
#include "attendance.h"
#include "grades.h"
#include "fee_tracker.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>

using namespace std;


// ============================================================================
// printMeritList — selection sort active students by CGPA descending
// ============================================================================
void printMeritList() {
    vector<Student> active = getAllActiveStudents();

    if (active.empty()) {
        cout << "\n[INFO] No active students found.\n";
        return;
    }

    // ---- Manual Selection Sort by CGPA (descending) ----
    for (int i = 0; i < (int)active.size() - 1; i++) {
        int maxIdx = i;
        for (int j = i + 1; j < (int)active.size(); j++) {
            if (active[j].cgpa > active[maxIdx].cgpa) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            // Manual Swap
            Student temp = active[i];
            active[i] = active[maxIdx];
            active[maxIdx] = temp;
        }
    }

    // ---- Print Tabular Report ----
    cout << "\n"
              << string(60, '=') << "\n"
              << "                    MERIT LIST\n"
              << string(60, '=') << "\n";

    cout << left
              << setw(6)  << "Rank"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(8)  << "Dept"
              << setw(8)  << "CGPA"
              << "\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)active.size(); i++) {
        cout << left
                  << setw(6)  << (i + 1)
                  << setw(14) << active[i].roll
                  << setw(22) << active[i].name
                  << setw(8)  << active[i].dept
                  << setw(8)  << doubleToStr(active[i].cgpa, 2)
                  << "\n";
    }

    cout << string(60, '-') << "\n"
              << "  Total Students Listed: " << active.size() << "\n"
              << string(60, '=') << "\n";
}

// ============================================================================
// printAttendanceDefaulters — students below 75% in any course
// ============================================================================
void printAttendanceDefaulters() {
    vector<vector<string>> enrollData = readTXT(ENROLLMENTS_FILE);

    if (enrollData.empty()) {
        cout << "\n[INFO] No enrollments found.\n";
        return;
    }

    cout << "\n"
              << string(75, '=') << "\n"
              << "               ATTENDANCE DEFAULTERS (BELOW 75%)\n"
              << string(75, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(12) << "Course"
              << setw(12) << "Semester"
              << setw(10) << "Attendance"
              << "\n";
    cout << string(75, '-') << "\n";

    int count = 0;
    for (int i = 0; i < (int)enrollData.size(); i++) {
        if (enrollData[i].size() >= 4 && toLower(enrollData[i][3]) == "enrolled") {
            string roll = enrollData[i][0];
            string courseCode = enrollData[i][1];
            string semester = enrollData[i][2];

            double pct = getAttendancePct(roll, courseCode);

            // Defaulter threshold is strictly below 75% (but must have marked sessions, i.e., pct >= 0.0)
            if (pct >= 0.0 && pct < 75.0) {
                count++;
                Student s;
                string name = roll;
                if (searchByRoll(roll, s)) {
                    name = s.name;
                }

                cout << left
                          << setw(4)  << count
                          << setw(14) << roll
                          << setw(22) << name
                          << setw(12) << courseCode
                          << setw(12) << semester
                          << setw(10) << (doubleToStr(pct, 1) + "%")
                          << "\n";
            }
        }
    }

    if (count == 0) {
        cout << "  No attendance defaulters found.\n";
    }

    cout << string(75, '-') << "\n"
              << "  Total Defaulter Entries: " << count << "\n"
              << string(75, '=') << "\n";
}

// ============================================================================
// printFeeDefaulters — call fee tracker defaulter search
// ============================================================================
void printFeeDefaulters() {
    // Reuses the interactive viewDefaulters from fee tracker module.
    viewDefaultersInteractive();
}

// ============================================================================
// printSemesterResult — transcript output for student+semester
// ============================================================================
void printSemesterResult() {
    string roll, semester;

    cout << "\n--- Generate Semester Transcript ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    Student s;
    if (!searchByRoll(roll, s)) {
        cout << "[ERROR] Student '" << roll << "' not found.\n";
        return;
    }

    vector<Enrollment> enrollments = getStudentEnrollments(roll, semester);
    if (enrollments.empty()) {
        cout << "[INFO] No active course registrations for " << roll
                  << " in " << semester << ".\n";
        return;
    }

    double gpa = computeGPA(roll, semester);

    cout << "\n"
              << "+--------------------------------------------------+\n"
              << "|               ACADEMIC TRANSCRIPT                |\n"
              << "+--------------------------------------------------+\n";
    cout << "| Student Name : " << setw(33) << left << s.name << " |\n"
              << "| Roll Number  : " << setw(33) << left << roll << " |\n"
              << "| Department   : " << setw(33) << left << s.dept << " |\n"
              << "| Semester     : " << setw(33) << left << semester << " |\n"
              << "+--------------------------------------------------+\n";

    cout << left
              << "| " << setw(8)  << "Course"
              << setw(23) << "Title"
              << setw(5)  << "Cr"
              << setw(7)  << "Grade"
              << setw(6)  << "Attn"
              << "|\n";
    cout << "|--------------------------------------------------|\n";

    vector<vector<string>> gradesData = readTXT(GRADES_FILE);

    for (int i = 0; i < (int)enrollments.size(); i++) {
        string courseCode = enrollments[i].courseCode;
        Course c;
        getCourseByCode(courseCode, c);

        // Find grade record and compute dynamically for precision
        string letterGrade = "N/A";
        for (int j = 0; j < (int)gradesData.size(); j++) {
            if (gradesData[j].size() >= 12 &&
                gradesData[j][0] == roll &&
                gradesData[j][1] == courseCode) {
                GradeEntry ge = rowToGradeEntry(gradesData[j]);
                ge.totalMarks = computeWeightedTotal(ge);
                letterGrade = getLetterGrade(ge.totalMarks);
                break;
            }
        }

        // Apply attendance penalty
        letterGrade = applyAttendancePenalty(roll, courseCode, letterGrade);

        double pct = getAttendancePct(roll, courseCode);
        string attnStr = "N/A";
        if (pct >= 0.0) {
            attnStr = doubleToStr(pct, 0) + "%";
        }

        string displayTitle = c.title;
        if (displayTitle.length() > 21) {
            displayTitle = displayTitle.substr(0, 18) + "...";
        }

        cout << "| " << left
                  << setw(8)  << courseCode
                  << setw(23) << displayTitle
                  << setw(5)  << c.credits
                  << setw(7)  << letterGrade
                  << setw(6)  << attnStr
                  << "|\n";
    }

    cout << "+--------------------------------------------------+\n"
              << "| Semester GPA : " << setw(33) << left << doubleToStr(gpa, 2) << " |\n"
              << "+--------------------------------------------------+\n";
}

// ============================================================================
// printDepartmentSummary — group students by department using parallel vectors
// ============================================================================
void printDepartmentSummary() {
    vector<Student> active = getAllActiveStudents();

    if (active.empty()) {
        cout << "\n[INFO] No active students found.\n";
        return;
    }

    // Parallel Vectors for Department Analytics
    vector<string> depts;
    vector<int> regCounts;
    vector<double> cgpaSums;
    vector<int> passCounts; // CGPA >= 2.0

    for (int i = 0; i < (int)active.size(); i++) {
        string d = active[i].dept;

        // Check if department is already added using manual loop
        int idx = -1;
        for (int j = 0; j < (int)depts.size(); j++) {
            if (depts[j] == d) {
                idx = j;
                break;
            }
        }

        if (idx == -1) {
            // New department found
            depts.push_back(d);
            regCounts.push_back(1);
            cgpaSums.push_back(active[i].cgpa);
            passCounts.push_back((active[i].cgpa >= 2.0) ? 1 : 0);
        } else {
            // Existing department — accumulate values
            regCounts[idx]++;
            cgpaSums[idx] += active[i].cgpa;
            if (active[i].cgpa >= 2.0) {
                passCounts[idx]++;
            }
        }
    }

    // ---- Print Table ----
    cout << "\n"
              << string(60, '=') << "\n"
              << "               DEPARTMENT PERFORMANCE SUMMARY\n"
              << string(60, '=') << "\n";

    cout << left
              << setw(15) << "Department"
              << setw(15) << "Registrations"
              << setw(15) << "Average CGPA"
              << setw(15) << "Passing Rate"
              << "\n";
    cout << string(60, '-') << "\n";

    for (int i = 0; i < (int)depts.size(); i++) {
        double avgCgpa = cgpaSums[i] / regCounts[i];
        double passRate = ((double)passCounts[i] / regCounts[i]) * 100.0;

        cout << left
                  << setw(15) << depts[i]
                  << setw(15) << regCounts[i]
                  << setw(15) << doubleToStr(avgCgpa, 2)
                  << setw(15) << (doubleToStr(passRate, 1) + "%")
                  << "\n";
    }

    cout << string(60, '=') << "\n";
}

// ============================================================================
// exportReportToFile — rdbuf stream redirection compiler
// ============================================================================
void exportReportToFile() {
    cout << "\n--- Export System Report to File ---\n";
    cout << "Select Report to Export:\n"
              << "  1. Merit List\n"
              << "  2. Attendance Defaulters\n"
              << "  3. Fee Defaulters\n"
              << "  4. Semester Result\n"
              << "  5. Department Summary\n"
              << "Enter Choice (1-5): ";

    string choiceStr;
    getline(cin, choiceStr);
    int choice = 0;
    istringstream iss(trim(choiceStr));
    iss >> choice;

    if (choice < 1 || choice > 5) {
        cout << "[ERROR] Invalid selection.\n";
        return;
    }

    cout << "Enter Output Filename (e.g. merit_list.txt): ";
    string filename;
    getline(cin, filename);
    filename = trim(filename);

    if (filename.empty()) {
        cout << "[ERROR] Filename cannot be empty.\n";
        return;
    }

    ofstream fout(filename);
    if (!fout.is_open()) {
        cout << "[ERROR] Cannot create or open file: " << filename << "\n";
        return;
    }

    // --- Redirect cout to output file ---
    streambuf* originalCoutBuffer = cout.rdbuf();
    cout.rdbuf(fout.rdbuf());

    // Execute the requested report function (output redirected to file)
    if (choice == 1) {
        printMeritList();
    } else if (choice == 2) {
        printAttendanceDefaulters();
    } else if (choice == 3) {
        printFeeDefaulters();
    } else if (choice == 4) {
        printSemesterResult();
    } else if (choice == 5) {
        printDepartmentSummary();
    }

    // --- Restore cout to standard console buffer ---
    cout.rdbuf(originalCoutBuffer);
    fout.close();

    cout << "[SUCCESS] Report compiled and exported to '" << filename << "' successfully.\n";
}
