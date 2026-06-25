// ============================================================================
// grades.cpp — M5: Grade Calculating Matrices Implementation
// ============================================================================

#include "grades.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include "student_ops.h"
#include "attendance.h"
#include "course_ops.h"
#include <iostream>
#include <sstream>

using namespace std;


// ============================================================================
// Conversion Utilities
// ============================================================================

GradeEntry rowToGradeEntry(const vector<string>& row) {
    GradeEntry g;
    g.roll       = "";
    g.courseCode  = "";
    for (int i = 0; i < 5; i++) g.quizzes[i] = 0.0;
    g.assignment  = 0.0;
    g.midterm     = 0.0;
    g.finalExam   = 0.0;
    g.totalMarks  = 0.0;
    g.letterGrade = "";

    // grades.txt: roll,course_code,q1,q2,q3,q4,q5,assignment,midterm,final,total,grade
    if (row.size() >= 12) {
        g.roll       = row[0];
        g.courseCode  = row[1];

        for (int i = 0; i < 5; i++) {
            istringstream iss(row[2 + i]);
            iss >> g.quizzes[i];
        }

        istringstream issA(row[7]);
        issA >> g.assignment;

        istringstream issM(row[8]);
        issM >> g.midterm;

        istringstream issF(row[9]);
        issF >> g.finalExam;

        istringstream issT(row[10]);
        issT >> g.totalMarks;

        g.letterGrade = row[11];
    }
    return g;
}

vector<string> gradeEntryToRow(const GradeEntry& g) {
    vector<string> row;
    row.push_back(g.roll);
    row.push_back(g.courseCode);

    for (int i = 0; i < 5; i++) {
        row.push_back(doubleToStr(g.quizzes[i], 1));
    }

    row.push_back(doubleToStr(g.assignment, 1));
    row.push_back(doubleToStr(g.midterm, 1));
    row.push_back(doubleToStr(g.finalExam, 1));
    row.push_back(doubleToStr(g.totalMarks, 2));
    row.push_back(g.letterGrade);

    return row;
}

// ============================================================================
// bestThreeOfFive — Extract top 3 WITHOUT sorting
// ============================================================================
// Strategy: Find the minimum value twice, exclude those indices, sum the rest.
// This avoids any need for a sort function.
// ============================================================================
double bestThreeOfFive(const double quizzes[5], int quizCount) {
    if (quizCount <= 0) return 0.0;
    if (quizCount <= 3) {
        // Sum all available quizzes
        double sum = 0.0;
        for (int i = 0; i < quizCount; i++) {
            sum += quizzes[i];
        }
        return sum;
    }

    // We need to exclude (quizCount - 3) lowest scores
    // Mark indices to exclude
    bool excluded[5] = {false, false, false, false, false};
    int toExclude = quizCount - 3;

    for (int e = 0; e < toExclude; e++) {
        int minIdx = -1;
        double minVal = 999999.0;

        for (int i = 0; i < quizCount; i++) {
            if (!excluded[i] && quizzes[i] < minVal) {
                minVal = quizzes[i];
                minIdx = i;
            }
        }
        if (minIdx >= 0) {
            excluded[minIdx] = true;
        }
    }

    // Sum remaining (non-excluded) quizzes
    double sum = 0.0;
    for (int i = 0; i < quizCount; i++) {
        if (!excluded[i]) {
            sum += quizzes[i];
        }
    }
    return sum;
}

// ============================================================================
// computeWeightedTotal — Apply grading formula
// ============================================================================
// Formula: (QuizAvg/10)*10 + (Assignment/100)*10 + (Midterm/40)*30 + (Final/60)*50
//
// QuizAvg = bestThreeOfFive() / 3  (average of best 3 quizzes, each out of 10)
// ============================================================================
double computeWeightedTotal(const GradeEntry& g) {
    // Determine how many quizzes have valid scores
    int quizCount = 5;

    double best3Sum = bestThreeOfFive(g.quizzes, quizCount);
    double quizAvg = best3Sum / 3.0; // Average of best 3, out of 10

    double quizComponent   = (quizAvg / 10.0) * 10.0;        // 10% weight
    double assignComponent = (g.assignment / 100.0) * 10.0;   // 10% weight
    double midComponent    = (g.midterm / 40.0) * 30.0;        // 30% weight
    double finalComponent  = (g.finalExam / 60.0) * 50.0;     // 50% weight

    return quizComponent + assignComponent + midComponent + finalComponent;
}

// ============================================================================
// getLetterGrade — Threshold-based grade assignment
// ============================================================================
string getLetterGrade(double totalMarks) {
    if (totalMarks >= 85.0) return "A";
    if (totalMarks >= 80.0) return "B+";
    if (totalMarks >= 70.0) return "B";
    if (totalMarks >= 65.0) return "C+";
    if (totalMarks >= 60.0) return "C";
    if (totalMarks >= 50.0) return "L";
    return "F";
}

// ============================================================================
// gradePointFromLetter — GPA point mapping
// ============================================================================
double gradePointFromLetter(const string& grade) {
    if (grade == "A")  return 4.0;
    if (grade == "B+") return 3.5;
    if (grade == "B")  return 3.0;
    if (grade == "C+") return 2.5;
    if (grade == "C")  return 2.0;
    if (grade == "L")  return 1.0;
    return 0.0; // F or unknown
}

// ============================================================================
// applyAttendancePenalty — Force F if attendance < 75%
// ============================================================================
string applyAttendancePenalty(const string& roll,
                                   const string& courseCode,
                                   const string& currentGrade) {
    double pct = getAttendancePct(roll, courseCode);
    if (pct >= 0.0 && pct < 75.0) {
        return "F";
    }
    return currentGrade;
}

// ============================================================================
// enterMarks — Interactive grade entry with validation
// ============================================================================
void enterMarks() {
    string roll, courseCode;

    cout << "\n--- Enter Marks ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    // Verify student exists
    Student s;
    if (!searchByRoll(roll, s)) {
        cout << "[ERROR] Student '" << roll << "' not found.\n";
        return;
    }

    // Verify course exists
    Course c;
    if (!getCourseByCode(courseCode, c)) {
        cout << "[ERROR] Course '" << courseCode << "' not found.\n";
        return;
    }

    GradeEntry g;
    g.roll       = roll;
    g.courseCode  = courseCode;

    // --- Quiz Scores (5 quizzes, each 0-10) ---
    cout << "\nEnter 5 Quiz Scores (0-10 each):\n";
    for (int i = 0; i < 5; i++) {
        bool valid = false;
        while (!valid) {
            cout << "  Quiz " << (i + 1) << ": ";
            string input;
            getline(cin, input);
            istringstream iss(trim(input));
            double val = 0.0;
            iss >> val;
            if (!iss.fail() && val >= 0.0 && val <= 10.0) {
                g.quizzes[i] = val;
                valid = true;
            } else {
                cout << "    [ERROR] Must be between 0 and 10.\n";
            }
        }
    }

    // --- Assignment Score (0-100) ---
    {
        bool valid = false;
        while (!valid) {
            cout << "Assignment Score (0-100): ";
            string input;
            getline(cin, input);
            istringstream iss(trim(input));
            double val = 0.0;
            iss >> val;
            if (!iss.fail() && val >= 0.0 && val <= 100.0) {
                g.assignment = val;
                valid = true;
            } else {
                cout << "  [ERROR] Must be between 0 and 100.\n";
            }
        }
    }

    // --- Midterm Score (0-40) ---
    {
        bool valid = false;
        while (!valid) {
            cout << "Midterm Score (0-40): ";
            string input;
            getline(cin, input);
            istringstream iss(trim(input));
            double val = 0.0;
            iss >> val;
            if (!iss.fail() && val >= 0.0 && val <= 40.0) {
                g.midterm = val;
                valid = true;
            } else {
                cout << "  [ERROR] Must be between 0 and 40.\n";
            }
        }
    }

    // --- Final Exam Score (0-60) ---
    {
        bool valid = false;
        while (!valid) {
            cout << "Final Exam Score (0-60): ";
            string input;
            getline(cin, input);
            istringstream iss(trim(input));
            double val = 0.0;
            iss >> val;
            if (!iss.fail() && val >= 0.0 && val <= 60.0) {
                g.finalExam = val;
                valid = true;
            } else {
                cout << "  [ERROR] Must be between 0 and 60.\n";
            }
        }
    }

    // --- Compute weighted total and letter grade ---
    g.totalMarks = computeWeightedTotal(g);
    g.letterGrade = getLetterGrade(g.totalMarks);

    // --- Apply attendance penalty ---
    g.letterGrade = applyAttendancePenalty(roll, courseCode, g.letterGrade);

    // --- Check if entry already exists and update, or append ---
    vector<vector<string>> data = readTXT(GRADES_FILE);
    bool updated = false;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 2 &&
            data[i][0] == roll &&
            data[i][1] == courseCode) {
            data[i] = gradeEntryToRow(g);
            updated = true;
            break;
        }
    }

    if (updated) {
        writeTXT(GRADES_FILE,
                 "roll,course_code,q1,q2,q3,q4,q5,assignment,midterm,final,total,grade",
                 data);
    } else {
        appendTXT(GRADES_FILE, gradeEntryToRow(g));
    }

    // --- Display summary ---
    cout << "\n"
              << string(50, '-') << "\n"
              << "  Grade Summary for " << roll << " in " << courseCode << "\n"
              << string(50, '-') << "\n"
              << "  Best 3 Quiz Avg : " << doubleToStr(bestThreeOfFive(g.quizzes, 5) / 3.0, 2) << "/10\n"
              << "  Assignment      : " << doubleToStr(g.assignment, 1) << "/100\n"
              << "  Midterm         : " << doubleToStr(g.midterm, 1) << "/40\n"
              << "  Final Exam      : " << doubleToStr(g.finalExam, 1) << "/60\n"
              << "  Weighted Total  : " << doubleToStr(g.totalMarks, 2) << "/100\n"
              << "  Letter Grade    : " << g.letterGrade;

    double pct = getAttendancePct(roll, courseCode);
    if (pct >= 0.0 && pct < 75.0) {
        cout << " [ATTENDANCE PENALTY]";
    }
    cout << "\n" << string(50, '-') << "\n";
    cout << "[SUCCESS] Marks recorded.\n";
}

// ============================================================================
// computeGPA — Credit-weighted semester GPA
// ============================================================================
double computeGPA(const string& roll, const string& semester) {
    // Get enrolled courses for this semester
    vector<Enrollment> enrollments = getStudentEnrollments(roll, semester);
    vector<vector<string>> grades = readTXT(GRADES_FILE);
    vector<vector<string>> courses = readTXT(COURSES_FILE);

    double totalPoints = 0.0;
    int totalCredits = 0;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        string cCode = enrollments[i].courseCode;

        // Find grade for this course
        string letterGrade = "";
        for (int j = 0; j < (int)grades.size(); j++) {
            if (grades[j].size() >= 12 &&
                grades[j][0] == roll &&
                grades[j][1] == cCode) {
                letterGrade = grades[j][11];
                break;
            }
        }

        if (letterGrade.empty()) continue; // No grade recorded yet

        // Apply attendance penalty
        letterGrade = applyAttendancePenalty(roll, cCode, letterGrade);

        // Find credit hours for this course
        int credits = 0;
        for (int j = 0; j < (int)courses.size(); j++) {
            if (courses[j].size() >= 3 && courses[j][0] == cCode) {
                istringstream iss(courses[j][2]);
                iss >> credits;
                break;
            }
        }

        double gp = gradePointFromLetter(letterGrade);
        totalPoints += gp * credits;
        totalCredits += credits;
    }

    if (totalCredits == 0) return 0.0;
    return totalPoints / totalCredits;
}

// ============================================================================
// computeClassStats — Highest, Lowest, Mean, Median
// ============================================================================
Stats computeClassStats(const string& courseCode) {
    Stats st;
    st.highest = 0.0;
    st.lowest  = 0.0;
    st.mean    = 0.0;
    st.median  = 0.0;

    vector<vector<string>> grades = readTXT(GRADES_FILE);
    vector<double> totals;

    // Collect all totals for this course
    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 11 && grades[i][1] == courseCode) {
            double t = 0.0;
            istringstream iss(grades[i][10]);
            iss >> t;
            totals.push_back(t);
        }
    }

    if (totals.empty()) return st;

    // --- Manual insertion sort for median calculation ---
    // (Cannot use <algorithm>, so we sort manually)
    for (int i = 1; i < (int)totals.size(); i++) {
        double key = totals[i];
        int j = i - 1;
        while (j >= 0 && totals[j] > key) {
            totals[j + 1] = totals[j];
            j--;
        }
        totals[j + 1] = key;
    }

    // Highest and Lowest (from sorted array)
    st.lowest  = totals[0];
    st.highest = totals[(int)totals.size() - 1];

    // Mean
    double sum = 0.0;
    for (int i = 0; i < (int)totals.size(); i++) {
        sum += totals[i];
    }
    st.mean = sum / totals.size();

    // Median
    int n = (int)totals.size();
    if (n % 2 == 1) {
        st.median = totals[n / 2];
    } else {
        st.median = (totals[n / 2 - 1] + totals[n / 2]) / 2.0;
    }

    return st;
}

// ============================================================================
// viewGradesInteractive
// ============================================================================
void viewGradesInteractive() {
    string roll, courseCode;
    cout << "\n--- View Grades ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    vector<vector<string>> grades = readTXT(GRADES_FILE);
    bool found = false;

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 12 &&
            grades[i][0] == roll &&
            grades[i][1] == courseCode) {

            GradeEntry g = rowToGradeEntry(grades[i]);
            // Recompute total and grade for accuracy
            g.totalMarks = computeWeightedTotal(g);
            g.letterGrade = getLetterGrade(g.totalMarks);
            g.letterGrade = applyAttendancePenalty(roll, courseCode, g.letterGrade);

            Student s;
            string name = roll;
            if (searchByRoll(roll, s)) name = s.name;

            cout << "\n"
                      << string(50, '=') << "\n"
                      << "  Grade Report: " << name << " (" << roll << ")\n"
                      << "  Course: " << courseCode << "\n"
                      << string(50, '=') << "\n";

            cout << "  Quizzes       : ";
            for (int q = 0; q < 5; q++) {
                cout << doubleToStr(g.quizzes[q], 1);
                if (q < 4) cout << ", ";
            }
            cout << "\n";
            cout << "  Best 3 Avg    : " << doubleToStr(bestThreeOfFive(g.quizzes, 5) / 3.0, 2) << "/10\n";
            cout << "  Assignment    : " << doubleToStr(g.assignment, 1) << "/100\n";
            cout << "  Midterm       : " << doubleToStr(g.midterm, 1) << "/40\n";
            cout << "  Final Exam    : " << doubleToStr(g.finalExam, 1) << "/60\n";
            cout << "  Weighted Total: " << doubleToStr(g.totalMarks, 2) << "/100\n";
            cout << "  Letter Grade  : " << g.letterGrade;

            double pct = getAttendancePct(roll, courseCode);
            if (pct >= 0.0 && pct < 75.0) {
                cout << " [ATTENDANCE PENALTY - " << doubleToStr(pct, 1) << "%]";
            }
            cout << "\n" << string(50, '=') << "\n";

            found = true;
            break;
        }
    }

    if (!found) {
        cout << "[INFO] No grades found for " << roll << " in " << courseCode << ".\n";
    }
}

// ============================================================================
// viewClassStatsInteractive
// ============================================================================
void viewClassStatsInteractive() {
    string courseCode;
    cout << "\n--- Class Statistics ---\n";
    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    Stats st = computeClassStats(courseCode);

    cout << "\n"
              << string(40, '=') << "\n"
              << "  Class Stats: " << courseCode << "\n"
              << string(40, '=') << "\n"
              << "  Highest : " << doubleToStr(st.highest, 2) << "\n"
              << "  Lowest  : " << doubleToStr(st.lowest, 2) << "\n"
              << "  Mean    : " << doubleToStr(st.mean, 2) << "\n"
              << "  Median  : " << doubleToStr(st.median, 2) << "\n"
              << string(40, '=') << "\n";
}

// ============================================================================
// viewGPAInteractive
// ============================================================================
void viewGPAInteractive() {
    string roll, semester;
    cout << "\n--- View Semester GPA ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    double gpa = computeGPA(roll, semester);

    Student s;
    string name = roll;
    if (searchByRoll(roll, s)) name = s.name;

    cout << "\n  Semester GPA for " << name << " (" << roll
              << ") in " << semester << ": " << doubleToStr(gpa, 2) << "\n";
}
