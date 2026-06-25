// ============================================================================
// course_ops.h — M3: Academic Registration Policies and Validation
// ============================================================================
// Manages course enrollment lifecycle: registration, dropping, credit load
// tracking, and prerequisite validation. Depends on M1 (filehandler) and
// M2 (student_ops) for data access and student verification.
//
// CONSTRAINTS ENFORCED:
//   - EnrollResult enum for explicit state reporting
//   - Max credit load: 21 hours per semester
//   - Prerequisite checking via grades file (non-F grade required)
//   - Drop only allowed if zero attendance logs exist
// ============================================================================

#ifndef COURSE_OPS_H
#define COURSE_OPS_H

#include <string>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// EnrollResult Enum — Explicit enrollment outcome states
// ----------------------------------------------------------------------------
enum EnrollResult {
    ENROLL_SUCCESS,
    ENROLL_STUDENT_INACTIVE,
    ENROLL_NO_SEATS,
    ENROLL_ALREADY_ENROLLED,
    ENROLL_CREDIT_OVERLOAD,
    ENROLL_PREREQ_FAIL,
    ENROLL_STUDENT_NOT_FOUND,
    ENROLL_COURSE_NOT_FOUND
};

// ----------------------------------------------------------------------------
// Course Struct — Academic course record
// ----------------------------------------------------------------------------
struct Course {
    string code;       // e.g., "CS101"
    string title;      // e.g., "Programming Fundamentals"
    int credits;            // Credit hours (1-4 typical)
    int seats;              // Total available seats
    string prereq;     // Prerequisite course code, or "NONE"
};

// ----------------------------------------------------------------------------
// Enrollment Struct — Student-course registration record
// ----------------------------------------------------------------------------
struct Enrollment {
    string roll;       // Student roll number
    string courseCode;  // Course code
    string semester;   // e.g., "Fall2023"
    string status;     // "enrolled" or "dropped"
};

// ----------------------------------------------------------------------------
// Conversion Utilities
// ----------------------------------------------------------------------------
Course rowToCourse(const vector<string>& row);
vector<string> courseToRow(const Course& c);
Enrollment rowToEnrollment(const vector<string>& row);
vector<string> enrollmentToRow(const Enrollment& e);

// ----------------------------------------------------------------------------
// enrollStudent
// ----------------------------------------------------------------------------
// Validates ALL of the following before registering:
//   1. Student exists and is active (not inactive/soft-deleted)
//   2. Course exists and has available seats (seats > enrolled count)
//   3. Student is not already enrolled in this course+semester
//   4. Adding this course does not push credit load beyond 21 hours
//   5. Prerequisites are satisfied (non-F grade in prereq course)
// Returns an EnrollResult enum value indicating the outcome.
// ----------------------------------------------------------------------------
EnrollResult enrollStudent(const string& roll,
                           const string& courseCode,
                           const string& semester);

// ----------------------------------------------------------------------------
// enrollStudentInteractive
// ----------------------------------------------------------------------------
// Interactive wrapper: prompts for roll, course code, semester, calls
// enrollStudent(), and prints a human-readable result message.
// ----------------------------------------------------------------------------
void enrollStudentInteractive();

// ----------------------------------------------------------------------------
// dropCourse
// ----------------------------------------------------------------------------
// Changes enrollment status to "dropped" ONLY if zero attendance records
// exist for the student+course combination. If attendance exists, the
// drop is blocked.
// Returns true if the drop was successful.
// ----------------------------------------------------------------------------
bool dropCourse(const string& roll,
                const string& courseCode,
                const string& semester);

// ----------------------------------------------------------------------------
// dropCourseInteractive
// ----------------------------------------------------------------------------
// Interactive wrapper: prompts for roll, course code, semester, calls
// dropCourse(), and prints a human-readable result message.
// ----------------------------------------------------------------------------
void dropCourseInteractive();

// ----------------------------------------------------------------------------
// getCreditLoad
// ----------------------------------------------------------------------------
// Computes the total credit hours a student is enrolled in for a given
// semester. Iterates over enrollments, cross-references course credits,
// and accumulates the total. Only counts "enrolled" (not "dropped") records.
// ----------------------------------------------------------------------------
int getCreditLoad(const string& roll, const string& semester);

// ----------------------------------------------------------------------------
// checkPrerequisite
// ----------------------------------------------------------------------------
// Checks whether the student has completed the prerequisite for a course.
// If the course prereq is "NONE", returns true immediately.
// Otherwise, reads the grades file to verify the student earned a non-F
// grade in the prerequisite course.
// ----------------------------------------------------------------------------
bool checkPrerequisite(const string& roll,
                       const string& courseCode);

// ----------------------------------------------------------------------------
// getEnrolledCount
// ----------------------------------------------------------------------------
// Counts the number of currently enrolled (status="enrolled") students
// in a specific course across all semesters. Used for seat availability.
// ----------------------------------------------------------------------------
int getEnrolledCount(const string& courseCode);

// ----------------------------------------------------------------------------
// listEnrolledStudents
// ----------------------------------------------------------------------------
// Displays a formatted table of all students currently enrolled in a
// given course and semester. Cross-references student names from M2.
// ----------------------------------------------------------------------------
void listEnrolledStudents(const string& courseCode,
                          const string& semester);

// ----------------------------------------------------------------------------
// listEnrolledStudentsInteractive
// ----------------------------------------------------------------------------
// Interactive wrapper: prompts for course code and semester, then calls
// listEnrolledStudents().
// ----------------------------------------------------------------------------
void listEnrolledStudentsInteractive();

// ----------------------------------------------------------------------------
// getCourseByCode
// ----------------------------------------------------------------------------
// Looks up a course by its code. Returns true and populates outCourse
// if found, false otherwise.
// ----------------------------------------------------------------------------
bool getCourseByCode(const string& code, Course& outCourse);

// ----------------------------------------------------------------------------
// getStudentEnrollments
// ----------------------------------------------------------------------------
// Returns all enrollment records for a given student and semester
// where status = "enrolled".
// ----------------------------------------------------------------------------
vector<Enrollment> getStudentEnrollments(const string& roll,
                                               const string& semester);

#endif // COURSE_OPS_H
