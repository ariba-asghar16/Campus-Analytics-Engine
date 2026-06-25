// ============================================================================
// fee_tracker.cpp — M6: Ledger Processing and Timeline Math Implementation
// ============================================================================

#include "fee_tracker.h" // NOLINT // IWYU pragma: keep
#include "filehandler.h"
#include "student_ops.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;


// ============================================================================
// Conversion Utilities
// ============================================================================

FeeRecord rowToFeeRecord(const vector<string>& row) {
    FeeRecord f;
    f.roll       = "";
    f.semester   = "";
    f.amountDue  = 0.0;
    f.amountPaid = 0.0;
    f.dueDate    = "";
    f.paidDate   = "";

    // fees.txt: roll,semester,amount_due,amount_paid,due_date,paid_date
    if (row.size() >= 6) {
        f.roll     = row[0];
        f.semester = row[1];

        istringstream issD(row[2]);
        issD >> f.amountDue;

        istringstream issP(row[3]);
        issP >> f.amountPaid;

        f.dueDate  = row[4];
        f.paidDate = row[5];
    }
    return f;
}

vector<string> feeRecordToRow(const FeeRecord& f) {
    vector<string> row;
    row.push_back(f.roll);
    row.push_back(f.semester);
    row.push_back(doubleToStr(f.amountDue, 0));
    row.push_back(doubleToStr(f.amountPaid, 0));
    row.push_back(f.dueDate);
    row.push_back(f.paidDate);
    return row;
}

// ============================================================================
// Helper: Parse DD-MM-YYYY into day, month, year integers
// ============================================================================
static bool parseDateParts(const string& date, int& day, int& month, int& year) {
    if (date.length() != 10) return false;
    if (date[2] != '-' || date[5] != '-') return false;

    // Validate all non-delimiter positions are digits
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue;
        if (date[i] < '0' || date[i] > '9') return false;
    }

    day   = (date[0] - '0') * 10 + (date[1] - '0');
    month = (date[3] - '0') * 10 + (date[4] - '0');
    year  = (date[6] - '0') * 1000 + (date[7] - '0') * 100 +
            (date[8] - '0') * 10   + (date[9] - '0');

    if (month < 1 || month > 12) return false;
    if (day < 1 || day > 31) return false;

    return true;
}

// ============================================================================
// Helper: Check if a year is a leap year
// ============================================================================
static bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ============================================================================
// Helper: Convert a date to an absolute day count since a reference point
// ============================================================================
// Uses the approach of counting total days from year 0.
// This avoids needing <ctime> entirely.
// ============================================================================
static long dateToAbsoluteDays(int day, int month, int year) {
    // Month-day boundaries (cumulative days before each month, for non-leap year)
    int daysBeforeMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

    // Count days from complete years
    long total = 0;
    total += (long)(year - 1) * 365;

    // Add leap year days for complete years
    // Leap years up to (year-1): floor((y-1)/4) - floor((y-1)/100) + floor((y-1)/400)
    int y = year - 1;
    total += y / 4 - y / 100 + y / 400;

    // Add days for complete months in current year
    total += daysBeforeMonth[month - 1];

    // If past February and current year is leap, add 1
    if (month > 2 && isLeapYear(year)) {
        total += 1;
    }

    // Add current day
    total += day;

    return total;
}

// ============================================================================
// daysBetween — Manual date arithmetic (NO <ctime>)
// ============================================================================
int daysBetween(const string& date1, const string& date2) {
    int d1, m1, y1;
    int d2, m2, y2;

    if (!parseDateParts(date1, d1, m1, y1)) return 0;
    if (!parseDateParts(date2, d2, m2, y2)) return 0;

    long abs1 = dateToAbsoluteDays(d1, m1, y1);
    long abs2 = dateToAbsoluteDays(d2, m2, y2);

    return (int)(abs2 - abs1);
}

// ============================================================================
// computeLateFine — 2% compound per full week past deadline
// ============================================================================
// Fine = amountDue * ((1.02^weeks) - 1)
// weeks = daysBetween(dueDate, paidDate) / 7  (integer division, floor)
// ============================================================================
double computeLateFine(double amountDue,
                       const string& dueDate,
                       const string& paidDate) {
    if (paidDate.empty() || paidDate.length() < 10) {
        // Unpaid — cannot compute fine without a reference date
        return 0.0;
    }

    int days = daysBetween(dueDate, paidDate);
    if (days <= 0) return 0.0; // Paid on time or early

    int weeks = days / 7;
    if (weeks <= 0) return 0.0;

    // Compute 1.02^weeks manually (no pow function needed for small integers)
    double multiplier = 1.0;
    for (int i = 0; i < weeks; i++) {
        multiplier *= 1.02;
    }

    return amountDue * (multiplier - 1.0);
}


// ============================================================================
// recordPayment — Interactive fee payment with date validation
// ============================================================================
void recordPayment() {
    string roll, semester, amtStr, paidDate;

    cout << "\n--- Record Fee Payment ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    // Find the fee record
    vector<vector<string>> data = readTXT(FEES_FILE);
    int targetIdx = -1;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 6 &&
            data[i][0] == roll &&
            data[i][1] == semester) {
            targetIdx = i;
            break;
        }
    }

    if (targetIdx == -1) {
        cout << "[ERROR] No fee record found for " << roll
                  << " in " << semester << ".\n";
        return;
    }

    FeeRecord f = rowToFeeRecord(data[targetIdx]);
    double outstanding = f.amountDue - f.amountPaid;

    cout << "  Amount Due  : " << doubleToStr(f.amountDue, 0) << "\n";
    cout << "  Amount Paid : " << doubleToStr(f.amountPaid, 0) << "\n";
    cout << "  Outstanding : " << doubleToStr(outstanding, 0) << "\n";

    if (outstanding <= 0) {
        cout << "[INFO] Fee already fully paid.\n";
        return;
    }

    cout << "Enter Payment Amount: ";
    getline(cin, amtStr);
    double payment = 0.0;
    istringstream iss(trim(amtStr));
    iss >> payment;

    if (iss.fail() || payment <= 0) {
        cout << "[ERROR] Invalid payment amount.\n";
        return;
    }

    // Validate paid date
    cout << "Enter Payment Date (DD-MM-YYYY): ";
    getline(cin, paidDate);
    paidDate = trim(paidDate);

    int d, m, y;
    if (!parseDateParts(paidDate, d, m, y)) {
        cout << "[ERROR] Invalid date format. Use DD-MM-YYYY.\n";
        return;
    }

    // Update record
    f.amountPaid += payment;
    if (f.amountPaid > f.amountDue) {
        f.amountPaid = f.amountDue; // Cap at amount due
    }
    f.paidDate = paidDate;

    data[targetIdx] = feeRecordToRow(f);
    writeTXT(FEES_FILE,
             "roll,semester,amount_due,amount_paid,due_date,paid_date",
             data);

    cout << "[SUCCESS] Payment of " << doubleToStr(payment, 0)
              << " recorded for " << roll << ".\n";

    // Show late fine if applicable
    double fine = computeLateFine(f.amountDue, f.dueDate, paidDate);
    if (fine > 0) {
        int days = daysBetween(f.dueDate, paidDate);
        cout << "  Late by " << days << " day(s) (" << (days / 7)
                  << " week(s)). Fine: " << doubleToStr(fine, 2) << "\n";
    }
}

// ============================================================================
// generateReceipt — Formatted invoice output
// ============================================================================
void generateReceipt(const string& roll, const string& semester) {
    vector<vector<string>> data = readTXT(FEES_FILE);
    FeeRecord f;
    bool found = false;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 6 &&
            data[i][0] == roll &&
            data[i][1] == semester) {
            f = rowToFeeRecord(data[i]);
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "[ERROR] No fee record found for " << roll
                  << " in " << semester << ".\n";
        return;
    }

    Student s;
    string name = roll;
    if (searchByRoll(roll, s)) name = s.name;

    double outstanding = f.amountDue - f.amountPaid;
    double fine = 0.0;
    int lateWeeks = 0;

    if (!f.paidDate.empty() && f.paidDate.length() >= 10) {
        int days = daysBetween(f.dueDate, f.paidDate);
        if (days > 0) {
            lateWeeks = days / 7;
            fine = computeLateFine(f.amountDue, f.dueDate, f.paidDate);
        }
    }

    // ---- Formatted Receipt ----
    cout << "\n"
              << string(50, '=') << "\n"
              << "           FEE RECEIPT / INVOICE\n"
              << string(50, '=') << "\n";

    cout << left
              << setw(20) << "  Student"     << ": " << name << "\n"
              << setw(20) << "  Roll No."    << ": " << roll << "\n"
              << setw(20) << "  Semester"    << ": " << semester << "\n"
              << setw(20) << "  Due Date"    << ": " << f.dueDate << "\n";

    if (!f.paidDate.empty() && f.paidDate.length() >= 10) {
        cout << setw(20) << "  Paid Date" << ": " << f.paidDate << "\n";
    }

    cout << string(50, '-') << "\n";

    cout << left
              << setw(30) << "  Total Fee"
              << right << setw(15) << doubleToStr(f.amountDue, 0)
              << "\n";

    cout << left
              << setw(30) << "  Amount Paid"
              << right << setw(15) << doubleToStr(f.amountPaid, 0)
              << "\n";

    if (fine > 0) {
        string fineLabel = "  Late Fine (" + to_string(lateWeeks) + " wk @ 2%)";
        cout << left
                  << setw(30) << fineLabel
                  << right << setw(15) << doubleToStr(fine, 2)
                  << "\n";
    }

    cout << string(50, '-') << "\n";

    double totalOwed = outstanding + fine;
    string statusStr = (totalOwed <= 0) ? "PAID IN FULL" : "BALANCE DUE";

    cout << left
              << setw(30) << "  Outstanding Balance"
              << right << setw(15) << doubleToStr(totalOwed, 2)
              << "\n";

    cout << left
              << setw(30) << "  Status"
              << right << setw(15) << statusStr
              << "\n";

    cout << string(50, '=') << "\n";
}

// ============================================================================
// generateReceiptInteractive
// ============================================================================
void generateReceiptInteractive() {
    string roll, semester;
    cout << "\n--- Generate Fee Receipt ---\n";
    cout << "Enter Roll Number: ";
    getline(cin, roll);
    roll = trim(roll);

    cout << "Enter Semester: ";
    getline(cin, semester);
    semester = trim(semester);

    generateReceipt(roll, semester);
}

// ============================================================================
// getDefaulters — Identify overdue records, Bubble Sort by outstanding desc
// ============================================================================
vector<FeeRecord> getDefaulters(const string& currentDate) {
    vector<vector<string>> data = readTXT(FEES_FILE);
    vector<FeeRecord> defaulters;

    for (int i = 0; i < (int)data.size(); i++) {
        if (data[i].size() >= 6) {
            FeeRecord f = rowToFeeRecord(data[i]);
            double outstanding = f.amountDue - f.amountPaid;

            if (outstanding > 0) {
                // Check if past due date
                int days = daysBetween(f.dueDate, currentDate);
                if (days > 0) {
                    defaulters.push_back(f);
                }
            }
        }
    }

    // ---- Manual Bubble Sort by outstanding amount (descending) ----
    for (int i = 0; i < (int)defaulters.size() - 1; i++) {
        for (int j = 0; j < (int)defaulters.size() - 1 - i; j++) {
            double outA = defaulters[j].amountDue - defaulters[j].amountPaid;
            double outB = defaulters[j + 1].amountDue - defaulters[j + 1].amountPaid;

            if (outA < outB) {
                // Swap
                FeeRecord temp = defaulters[j];
                defaulters[j] = defaulters[j + 1];
                defaulters[j + 1] = temp;
            }
        }
    }

    return defaulters;
}

// ============================================================================
// viewDefaultersInteractive
// ============================================================================
void viewDefaultersInteractive() {
    string currentDate;
    cout << "\n--- Fee Defaulters ---\n";
    cout << "Enter Reference Date (DD-MM-YYYY): ";
    getline(cin, currentDate);
    currentDate = trim(currentDate);

    vector<FeeRecord> defaulters = getDefaulters(currentDate);

    if (defaulters.empty()) {
        cout << "[INFO] No fee defaulters found.\n";
        return;
    }

    cout << "\n"
              << string(75, '=') << "\n"
              << "  FEE DEFAULTERS REPORT (as of " << currentDate << ")\n"
              << string(75, '=') << "\n";

    cout << left
              << setw(4)  << "#"
              << setw(14) << "Roll"
              << setw(14) << "Semester"
              << setw(12) << "Due"
              << setw(12) << "Paid"
              << setw(12) << "Balance"
              << setw(8)  << "Weeks"
              << "\n";
    cout << string(75, '-') << "\n";

    for (int i = 0; i < (int)defaulters.size(); i++) {
        double outstanding = defaulters[i].amountDue - defaulters[i].amountPaid;
        int days = daysBetween(defaulters[i].dueDate, currentDate);
        int weeks = days / 7;

        cout << left
                  << setw(4)  << (i + 1)
                  << setw(14) << defaulters[i].roll
                  << setw(14) << defaulters[i].semester
                  << setw(12) << doubleToStr(defaulters[i].amountDue, 0)
                  << setw(12) << doubleToStr(defaulters[i].amountPaid, 0)
                  << setw(12) << doubleToStr(outstanding, 0)
                  << setw(8)  << weeks
                  << "\n";
    }

    cout << string(75, '-') << "\n"
              << "  Total Defaulters: " << defaulters.size() << "\n"
              << string(75, '=') << "\n";
}
