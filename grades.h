// ============================================================================
// grades.h — M5: Grade Calculating Matrices and Raw Class Statistics
// ============================================================================
// Manages grade entry, weighted total computation, letter grade assignment,
// GPA calculation, class statistics, and attendance-based penalties.
// Depends on M1 (filehandler), M2 (student_ops), M4 (attendance).
//
// CONSTRAINTS ENFORCED:
//   - Best 3 of 5 quizzes extracted WITHOUT sorting
//   - Weighted formula: Quiz*0.10 + Assignment*0.10 + Midterm*0.30 + Final*0.50
//   - Letter grades: >=85 A, >=80 B+, >=70 B, >=65 C+, >=60 C, >=50 L, <50 F
//   - Attendance < 75% forces grade to 'F'
//   - Stats struct for class-level analytics
// ============================================================================

#ifndef GRADES_H
#define GRADES_H

#include <string>
#include <vector>

using namespace std;


// ----------------------------------------------------------------------------
// GradeEntry Struct — Student performance record for one course
// ----------------------------------------------------------------------------
struct GradeEntry {
    string roll;
    string courseCode;
    double quizzes[5];      // Up to 5 quiz scores (each max 10)
    double assignment;       // Assignment score (max 100, scaled by weight)
    double midterm;          // Midterm score (max 40)
    double finalExam;        // Final exam score (max 60)
    double totalMarks;       // Computed weighted total
    string letterGrade; // Computed letter grade
};

// ----------------------------------------------------------------------------
// Stats Struct — Standalone class-level statistics container
// ----------------------------------------------------------------------------
struct Stats {
    double highest;
    double lowest;
    double mean;
    double median;
};

// ----------------------------------------------------------------------------
// Conversion Utilities
// ----------------------------------------------------------------------------
GradeEntry rowToGradeEntry(const vector<string>& row);
vector<string> gradeEntryToRow(const GradeEntry& g);

// ----------------------------------------------------------------------------
// enterMarks
// ----------------------------------------------------------------------------
// Interactive: prompts for roll, course code, then collects up to 5 quiz
// scores (each 0-10), assignment (0-100), midterm (0-40), and final (0-60).
// Validates each input against its expected range.
// Computes weighted total and letter grade, then writes to grades.txt.
// ----------------------------------------------------------------------------
void enterMarks();

// ----------------------------------------------------------------------------
// bestThreeOfFive
// ----------------------------------------------------------------------------
// Extracts the sum of the highest 3 scores out of 5 quizzes WITHOUT calling
// any sort utility. Finds and excludes the two lowest scores manually.
// Handles edge cases where fewer than 3 quizzes have valid scores.
// Returns the sum of the best 3 (or fewer if < 3 entries).
// ----------------------------------------------------------------------------
double bestThreeOfFive(const double quizzes[5], int quizCount);

// ----------------------------------------------------------------------------
// computeWeightedTotal
// ----------------------------------------------------------------------------
// Applies the grading formula:
//   Total = (QuizAvg/10)*10 + (Assignment/100)*10 + (Midterm/40)*30 + (Final/60)*50
//
// Where QuizAvg is bestThreeOfFive()/3, giving a value out of 10.
// Returns total marks out of 100.
// ----------------------------------------------------------------------------
double computeWeightedTotal(const GradeEntry& g);

// ----------------------------------------------------------------------------
// getLetterGrade
// ----------------------------------------------------------------------------
// Converts total marks to a letter grade:
//   >=85 -> A    >=80 -> B+   >=70 -> B   >=65 -> C+
//   >=60 -> C    >=50 -> L    <50  -> F
// ----------------------------------------------------------------------------
string getLetterGrade(double totalMarks);

// ----------------------------------------------------------------------------
// computeGPA
// ----------------------------------------------------------------------------
// Loops through all courses for a student in a semester, maps letter grades
// to GPA points, and computes a credit-weighted average.
// GPA scale: A=4.0, B+=3.5, B=3.0, C+=2.5, C=2.0, L=1.0, F=0.0
// Returns the semester GPA.
// ----------------------------------------------------------------------------
double computeGPA(const string& roll, const string& semester);

// ----------------------------------------------------------------------------
// computeClassStats
// ----------------------------------------------------------------------------
// Collects all grade totals for a specific course and computes the class
// statistics: Highest, Lowest, Mean, and Median values.
// For median: uses a manual insertion sort on the collected totals (since
// we need ordering for median but cannot use <algorithm>).
// Returns a Stats struct.
// ----------------------------------------------------------------------------
Stats computeClassStats(const string& courseCode);

// ----------------------------------------------------------------------------
// applyAttendancePenalty
// ----------------------------------------------------------------------------
// Checks if the student's attendance percentage (via M4's getAttendancePct)
// is below 75%. If so, overrides the letter grade to 'F' regardless of
// actual marks. Returns the final (possibly penalized) letter grade.
// ----------------------------------------------------------------------------
string applyAttendancePenalty(const string& roll,
                                   const string& courseCode,
                                   const string& currentGrade);

// ----------------------------------------------------------------------------
// viewGradesInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for roll and course, displays full grade breakdown.
// ----------------------------------------------------------------------------
void viewGradesInteractive();

// ----------------------------------------------------------------------------
// viewClassStatsInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for course code, displays Highest/Lowest/Mean/Median.
// ----------------------------------------------------------------------------
void viewClassStatsInteractive();

// ----------------------------------------------------------------------------
// viewGPAInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for roll and semester, displays semester GPA.
// ----------------------------------------------------------------------------
void viewGPAInteractive();

// ----------------------------------------------------------------------------
// gradePointFromLetter
// ----------------------------------------------------------------------------
// Converts a letter grade string to its numeric GPA point value.
// ----------------------------------------------------------------------------
double gradePointFromLetter(const string& grade);

#endif // GRADES_H
