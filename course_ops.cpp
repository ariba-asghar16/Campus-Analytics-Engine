// ============================================================================
// course_ops.cpp — M3: Academic Registration Policies Implementation
// ============================================================================

#include "course_ops.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


// ============================================================================
// Conversion Utilities
// ============================================================================

Course rowToCourse(const vector<string>& row) {
    Course c;
    c.code    = "";
    c.title   = "";
    c.credits = 0;
    c.seats   = 0;
    c.prereq  = "NONE";

    if (row.size() >= 5) {
        c.code    = row[0];
        c.title   = row[1];

        istringstream issC(row[2]);
        issC >> c.credits;

        istringstream issS(row[3]);
        issS >> c.seats;

        c.prereq = row[4];
    }
    return c;
}

vector<string> courseToRow(const Course& c) {
    vector<string> row;
    row.push_back(c.code);
    row.push_back(c.title);

    ostringstream ossC;
    ossC << c.credits;
    row.push_back(ossC.str());

    ostringstream ossS;
    ossS << c.seats;
    row.push_back(ossS.str());

    row.push_back(c.prereq);
    return row;
}

Enrollment rowToEnrollment(const vector<string>& row) {
    Enrollment e;
    e.roll       = "";
    e.courseCode  = "";
    e.semester   = "";
    e.status     = "";

    if (row.size() >= 4) {
        e.roll       = row[0];
        e.courseCode  = row[1];
        e.semester   = row[2];
        e.status     = toLower(row[3]);
    }
    return e;
}

vector<string> enrollmentToRow(const Enrollment& e) {
    vector<string> row;
    row.push_back(e.roll);
    row.push_back(e.courseCode);
    row.push_back(e.semester);
    row.push_back(e.status);
    return row;
}

// ============================================================================
// getCourseByCode — Lookup course by code
// ============================================================================
bool getCourseByCode(const string& code, Course& outCourse) {
    vector<vector<string>> data = readTXT(COURSES_FILE);
    vector<string> row = findRow(data, 0, code);

    if (row.empty()) return false;

    outCourse = rowToCourse(row);
    return true;
}

// ============================================================================
// getEnrolledCount — Count active enrollments for a course
// ============================================================================
int getEnrolledCount(const string& courseCode) {
    vector<vector<string>> data = readTXT(ENROLLMENTS_FILE);
    int count = 0;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 4) {
            if (data[i][1] == courseCode && toLower(data[i][3]) == "enrolled") {
                count++;
            }
        }
    }
    return count;
}

// ============================================================================
// getCreditLoad — Sum credit hours for a student in a semester
// ============================================================================
// Nested loop: iterate enrollments, cross-reference course credits.
// Only counts enrollments with status="enrolled".
// ============================================================================
int getCreditLoad(const string& roll, const string& semester) {
    vector<vector<string>> enrollments = readTXT(ENROLLMENTS_FILE);
    vector<vector<string>> courses = readTXT(COURSES_FILE);
    int totalCredits = 0;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 4 &&
            enrollments[i][0] == roll &&
            enrollments[i][2] == semester &&
            toLower(enrollments[i][3]) == "enrolled") {

            // Find the course to get its credit value
            string cCode = enrollments[i][1];
            for (int j = 0; j < (int)courses.size(); j++) {
                if (courses[j].size() >= 3 && courses[j][0] == cCode) {
                    int cr = 0;
                    istringstream iss(courses[j][2]);
                    iss >> cr;
                    totalCredits += cr;
                    break;
                }
            }
        }
    }
    return totalCredits;
}

// ============================================================================
// checkPrerequisite — Verify student completed prereq with non-F grade
// ============================================================================
bool checkPrerequisite(const string& roll,
                       const string& courseCode) {
    // Look up the course to find its prerequisite
    Course course;
    if (!getCourseByCode(courseCode, course)) {
        return false; // Course doesn't exist
    }

    // If no prerequisite, pass immediately
    if (course.prereq == "NONE" || course.prereq.empty()) {
        return true;
    }

    // Read grades file and check if student has a non-F grade in prereq
    // grades.txt: roll,course_code,q1,q2,q3,q4,q5,assignment,midterm,final,total,grade
    vector<vector<string>> grades = readTXT(GRADES_FILE);

    for (int i = 0; i < (int)grades.size(); i++) {
        if (grades[i].size() >= 12) {
            if (grades[i][0] == roll && grades[i][1] == course.prereq) {
                // Check the grade (column 11)
                string grade = trim(grades[i][11]);
                if (grade != "F" && grade != "f" && !grade.empty()) {
                    return true; // Student passed the prerequisite
                }
                return false; // Student has an F or empty grade
            }
        }
    }

    // No grade record found for the prerequisite — not completed
    return false;
}

// ============================================================================
// getStudentEnrollments — Get all active enrollments for a student+semester
// ============================================================================
vector<Enrollment> getStudentEnrollments(const string& roll,
                                               const string& semester) {
    vector<Enrollment> results;
    vector<vector<string>> data = readTXT(ENROLLMENTS_FILE);

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 4 &&
            data[i][0] == roll &&
            data[i][2] == semester &&
            toLower(data[i][3]) == "enrolled") {
            results.push_back(rowToEnrollment(data[i]));
        }
    }
    return results;
}

// ============================================================================
// enrollStudent — Full enrollment validation pipeline
// ============================================================================
EnrollResult enrollStudent(const string& roll,
                           const string& courseCode,
                           const string& semester) {
    // 1. Check student exists and is active
    Student student;
    if (!searchByRoll(roll, student)) {
        return ENROLL_STUDENT_NOT_FOUND;
    }
    if (student.status != "active") {
        return ENROLL_STUDENT_INACTIVE;
    }

    // 2. Check course exists
    Course course;
    if (!getCourseByCode(courseCode, course)) {
        return ENROLL_COURSE_NOT_FOUND;
    }

    // 3. Check seat availability
    int enrolled = getEnrolledCount(courseCode);
    if (enrolled >= course.seats) {
        return ENROLL_NO_SEATS;
    }

    // 4. Check for double registration (same student, course, semester)
    vector<vector<string>> enrollments = readTXT(ENROLLMENTS_FILE);
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 4 &&
            enrollments[i][0] == roll &&
            enrollments[i][1] == courseCode &&
            enrollments[i][2] == semester &&
            toLower(enrollments[i][3]) == "enrolled") {
            return ENROLL_ALREADY_ENROLLED;
        }
    }

    // 5. Check credit overload (max 21 hours)
    int currentLoad = getCreditLoad(roll, semester);
    if (currentLoad + course.credits > 21) {
        return ENROLL_CREDIT_OVERLOAD;
    }

    // 6. Check prerequisite
    if (!checkPrerequisite(roll, courseCode)) {
        return ENROLL_PREREQ_FAIL;
    }

    // All checks passed — create enrollment record
    Enrollment e;
    e.roll       = roll;
    e.courseCode  = courseCode;
    e.semester   = semester;
    e.status     = "enrolled";

    appendTXT(ENROLLMENTS_FILE, enrollmentToRow(e));
    return ENROLL_SUCCESS;
}

// ============================================================================
// enrollStudentInteractive — User-facing enrollment with messages
// ============================================================================
void enrollStudentInteractive() {
    string roll, courseCode, semester;

    cout << "\n--- Enroll Student in Course ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    cout << "Enter Semester (e.g., Fall2023): ";
    getline(cin, semester);
    semester = trim(semester);

    EnrollResult result = enrollStudent(roll, courseCode, semester);

    switch (result) {
        case ENROLL_SUCCESS:
            cout << "[SUCCESS] " << roll << " enrolled in " << courseCode
                      << " for " << semester << ".\n";
            break;
        case ENROLL_STUDENT_NOT_FOUND:
            cout << "[ERROR] Student '" << roll << "' not found.\n";
            break;
        case ENROLL_STUDENT_INACTIVE:
            cout << "[ERROR] Student '" << roll << "' is inactive.\n";
            break;
        case ENROLL_COURSE_NOT_FOUND:
            cout << "[ERROR] Course '" << courseCode << "' not found.\n";
            break;
        case ENROLL_NO_SEATS:
            cout << "[ERROR] No seats available in '" << courseCode << "'.\n";
            break;
        case ENROLL_ALREADY_ENROLLED:
            cout << "[ERROR] " << roll << " is already enrolled in "
                      << courseCode << " for " << semester << ".\n";
            break;
        case ENROLL_CREDIT_OVERLOAD:
            cout << "[ERROR] Enrolling would exceed 21-credit limit for "
                      << roll << " in " << semester << ".\n";
            break;
        case ENROLL_PREREQ_FAIL:
            cout << "[ERROR] Prerequisite not met for '" << courseCode
                      << "'.\n";
            break;
    }
}

// ============================================================================
// dropCourse — Drop only if zero attendance records exist
// ============================================================================
bool dropCourse(const string& roll,
                const string& courseCode,
                const string& semester) {
    // Check for attendance records
    vector<vector<string>> attendance = readTXT(ATTENDANCE_FILE);
    for (int i = 0; i < (int)attendance.size(); i++) {
        if (attendance[i].size() >= 3 &&
            attendance[i][0] == roll &&
            attendance[i][1] == courseCode) {
            cout << "[ERROR] Cannot drop — attendance records exist for "
                      << roll << " in " << courseCode << ".\n";
            return false;
        }
    }

    // Find and update enrollment record
    vector<vector<string>> enrollments = readTXT(ENROLLMENTS_FILE);
    bool found = false;

    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 4 &&
            enrollments[i][0] == roll &&
            enrollments[i][1] == courseCode &&
            enrollments[i][2] == semester &&
            toLower(enrollments[i][3]) == "enrolled") {
            enrollments[i][3] = "dropped";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "[ERROR] Active enrollment not found for " << roll
                  << " in " << courseCode << " (" << semester << ").\n";
        return false;
    }

    writeTXT(ENROLLMENTS_FILE, "roll,course_code,semester,status", enrollments);
    cout << "[SUCCESS] " << roll << " dropped from " << courseCode
              << " (" << semester << ").\n";
    return true;
}

// ============================================================================
// dropCourseInteractive — User-facing drop with messages
// ============================================================================
void dropCourseInteractive() {
    string roll, courseCode, semester;

    cout << "\n--- Drop Course ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    dropCourse(roll, courseCode, semester);
}

// ============================================================================
// listEnrolledStudents — Formatted roster for a course+semester
// ============================================================================
void listEnrolledStudents(const string& courseCode,
                          const string& semester) {
    vector<vector<string>> enrollments = readTXT(ENROLLMENTS_FILE);
    vector<string> matchedRolls;

    // Collect enrolled rolls for this course+semester
    for (int i = 0; i < (int)enrollments.size(); i++) {
        if (enrollments[i].size() >= 4 &&
            enrollments[i][1] == courseCode &&
            enrollments[i][2] == semester &&
            toLower(enrollments[i][3]) == "enrolled") {
            matchedRolls.push_back(enrollments[i][0]);
        }
    }

    if (matchedRolls.empty()) {
        cout << "\n[INFO] No enrolled students for " << courseCode
                  << " in " << semester << ".\n";
        return;
    }

    // Print header
    Course course;
    getCourseByCode(courseCode, course);

    cout << "\n"
              << string(60, '=') << "\n"
              << "  Enrolled Students: " << courseCode << " - "
              << course.title << " (" << semester << ")\n"
              << string(60, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(22) << "Name"
              << setw(8)  << "Dept"
              << setw(8)  << "CGPA"
              << "\n";
    cout << string(60, '-') << "\n";

    int count = 0;
    for (int i = 0; i < (int)matchedRolls.size(); i++) {
        Student s;
        if (searchByRoll(matchedRolls[i], s)) {
            count++;
            cout << left
                      << setw(4)  << count
                      << setw(14) << s.roll
                      << setw(22) << s.name
                      << setw(8)  << s.dept
                      << setw(8)  << doubleToStr(s.cgpa, 2)
                      << "\n";
        }
    }

    cout << string(60, '-') << "\n"
              << "  Total: " << count << " student(s)\n"
              << string(60, '=') << "\n";
}

// ============================================================================
// listEnrolledStudentsInteractive — User-facing roster lookup
// ============================================================================
void listEnrolledStudentsInteractive() {
    string courseCode, semester;

    cout << "\n--- List Enrolled Students ---\n";
    cout << "Enter Course Code: ";
    getline(cin, courseCode);
    courseCode = trim(courseCode);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    listEnrolledStudents(courseCode, semester);
}
