// ============================================================================
// fee_tracker.h — M6: Ledger Processing and Timeline Math
// ============================================================================
// Manages fee payments, late fine computation, date arithmetic,
// receipt generation, and defaulter identification.
// Depends on M1 (filehandler) and M2 (student_ops).
//
// CONSTRAINTS ENFORCED:
//   - No <ctime> — all date math via manual DD-MM-YYYY parsing
//   - Late fine: 2% compound per full week past deadline
//   - Manual Bubble Sort for defaulters by outstanding amount (descending)
//   - Formatted receipts using setw / setfill
// ============================================================================

#ifndef FEE_TRACKER_H
#define FEE_TRACKER_H

#include <string>
#include <vector>


using namespace std;
// ----------------------------------------------------------------------------
// FeeRecord Struct — Student fee ledger entry
// ----------------------------------------------------------------------------
struct FeeRecord {
  string roll;     // Student roll number
  string semester; // e.g., "Fall2023"
  double amountDue;     // Total fee amount
  double amountPaid;    // Amount paid so far
  string dueDate;  // DD-MM-YYYY
  string paidDate; // DD-MM-YYYY or empty if unpaid
};

// ----------------------------------------------------------------------------
// Conversion Utilities
// ----------------------------------------------------------------------------
FeeRecord rowToFeeRecord(const vector<string> &row);
vector<string> feeRecordToRow(const FeeRecord &f);

// ----------------------------------------------------------------------------
// recordPayment
// ----------------------------------------------------------------------------
// Interactive: prompts for roll, semester, payment amount, and paid date.
// Validates date format (DD-MM-YYYY) using manual index offsets.
// Updates the fee record in fees.txt.
// ----------------------------------------------------------------------------
void recordPayment();

// ----------------------------------------------------------------------------
// computeLateFine
// ----------------------------------------------------------------------------
// Computes late fee using 2% compound per full week past the deadline.
// Formula: fine = amountDue * ((1.02^weeks) - 1)
// where weeks = daysBetween(dueDate, paidDate) / 7 (integer division)
// Returns the fine amount.
// ----------------------------------------------------------------------------
double computeLateFine(double amountDue, const string &dueDate,
                       const string &paidDate);

// ----------------------------------------------------------------------------
// daysBetween
// ----------------------------------------------------------------------------
// Accepts two DD-MM-YYYY strings, parses into integers, converts each
// date into a total "absolute day number" since a reference epoch using
// a custom month-day boundary array, and returns the difference.
// Handles leap years. Does NOT use <ctime>.
// Returns the number of days between date1 and date2.
// (Positive if date2 is after date1)
// ----------------------------------------------------------------------------
int daysBetween(const string &date1, const string &date2);

// ----------------------------------------------------------------------------
// generateReceipt
// ----------------------------------------------------------------------------
// Prints a formatted invoice for a specific student and semester,
// including header, fine amounts, outstanding balance, and payment status.
// Uses setw and setfill for alignment.
// ----------------------------------------------------------------------------
void generateReceipt(const string &roll, const string &semester);

// ----------------------------------------------------------------------------
// generateReceiptInteractive
// ----------------------------------------------------------------------------
// Interactive wrapper: prompts for roll and semester.
// ----------------------------------------------------------------------------
void generateReceiptInteractive();

// ----------------------------------------------------------------------------
// getDefaulters
// ----------------------------------------------------------------------------
// Identifies all fee records with outstanding balance (amountDue > amountPaid)
// past their due dates. Sorts by outstanding amount in descending order
// using manual Bubble Sort.
// Returns a vector of FeeRecord structs.
// ----------------------------------------------------------------------------
vector<FeeRecord> getDefaulters(const string &currentDate);

// ----------------------------------------------------------------------------
// viewDefaultersInteractive
// ----------------------------------------------------------------------------
// Interactive: prompts for a reference date and displays the defaulter list.
// ----------------------------------------------------------------------------
void viewDefaultersInteractive();

#endif // FEE_TRACKER_H
//
