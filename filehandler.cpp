// ============================================================================
// filehandler.cpp — M1: Low-Level Parsing Engine Implementation
// ============================================================================

#include "filehandler.h" // NOLINT // IWYU pragma: keep
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// ============================================================================
// readTXT — Character-by-character CSV parser
// ============================================================================
// Reads the file line by line, skips the header (row 0), then scans each
// character to split on commas while respecting quoted fields.
// ============================================================================
vector<vector<string>> readTXT(const string &filePath) {
  vector<vector<string>> result;
  ifstream fin(filePath);

  if (!fin.is_open()) {
    cerr << "[ERROR] Cannot open file: " << filePath << endl;
    return result;
  }

  string line;
  int lineNum = 0;

  while (getline(fin, line)) {
    lineNum++;
    // Skip the header row
    if (lineNum == 1)
      continue;

    // Skip empty lines
    if (line.empty())
      continue;

    // Character-by-character field extraction
    vector<string> row;
    string field = "";
    bool inQuotes = false;

    for (int i = 0; i < (int)line.length(); i++) {
      char c = line[i];

      if (c == '"') {
        // Toggle quote state — handles embedded commas inside quotes
        inQuotes = !inQuotes;
      } else if (c == ',' && !inQuotes) {
        // Field delimiter found outside quotes — commit current field
        row.push_back(trim(field));
        field = "";
      } else {
        // Regular character — accumulate into current field
        field += c;
      }
    }
    // Push the last field (no trailing comma)
    row.push_back(trim(field));
    result.push_back(row);
  }

  fin.close();
  return result;
}

// ============================================================================
// writeTXT — Full file overwrite with header
// ============================================================================
// Truncates the target file, writes the header as line 1, then serializes
// each data row. Fields containing commas are wrapped in double quotes.
// ============================================================================
void writeTXT(const string &filePath, const string &header,
              const vector<vector<string>> &data) {
  ofstream fout(filePath, ios::trunc);

  if (!fout.is_open()) {
    cerr << "[ERROR] Cannot write to file: " << filePath << endl;
    return;
  }

  // Write header line
  fout << header << "\n";

  // Serialize each row
  for (int i = 0; i < (int)data.size(); i++) {
    for (int j = 0; j < (int)data[i].size(); j++) {
      // Check if field contains a comma — if so, wrap in quotes
      bool hasComma = false;
      for (int k = 0; k < (int)data[i][j].length(); k++) {
        if (data[i][j][k] == ',') {
          hasComma = true;
          break;
        }
      }

      if (hasComma) {
        fout << "\"" << data[i][j] << "\"";
      } else {
        fout << data[i][j];
      }

      // Comma delimiter between fields (not after the last one)
      if (j < (int)data[i].size() - 1) {
        fout << ",";
      }
    }
    fout << "\n";
  }

  fout.close();
}

// ============================================================================
// appendTXT — Single-record append without full file reload
// ============================================================================
void appendTXT(const string &filePath, const vector<string> &row) {
  ofstream fout(filePath, ios::app);

  if (!fout.is_open()) {
    cerr << "[ERROR] Cannot append to file: " << filePath << endl;
    return;
  }

  for (int j = 0; j < (int)row.size(); j++) {
    // Quote fields containing commas
    bool hasComma = false;
    for (int k = 0; k < (int)row[j].length(); k++) {
      if (row[j][k] == ',') {
        hasComma = true;
        break;
      }
    }

    if (hasComma) {
      fout << "\"" << row[j] << "\"";
    } else {
      fout << row[j];
    }

    if (j < (int)row.size() - 1) {
      fout << ",";
    }
  }
  fout << "\n";
  fout.flush();
  fout.close();
}

// ============================================================================
// findRow — Linear search returning the first matching row
// ============================================================================
vector<string> findRow(const vector<vector<string>> &data, int colIndex,
                       const string &value) {

  for (int i = 0; i < (int)data.size(); i++) {
    if (colIndex < (int)data[i].size()) {
      if (data[i][colIndex] == value) {
        return data[i];
      }
    }
  }
  // No match found — return empty vector
  return vector<string>();
}

// ============================================================================
// findAllRows — Linear search returning ALL matching rows
// ============================================================================
vector<vector<string>> findAllRows(const vector<vector<string>> &data,
                                   int colIndex, const string &value) {

  vector<vector<string>> matches;

  for (int i = 0; i < (int)data.size(); i++) {
    if (colIndex < (int)data[i].size()) {
      if (data[i][colIndex] == value) {
        matches.push_back(data[i]);
      }
    }
  }
  return matches;
}

// ============================================================================
// rowExists — Boolean existence check at a specific column
// ============================================================================
bool rowExists(const vector<vector<string>> &data, int colIndex,
               const string &value) {

  for (int i = 0; i < (int)data.size(); i++) {
    if (colIndex < (int)data[i].size()) {
      if (data[i][colIndex] == value) {
        return true;
      }
    }
  }
  return false;
}

// ============================================================================
// toLower — Manual case conversion using character offsets
// ============================================================================
string toLower(const string &s) {
  string result = s;
  for (int i = 0; i < (int)result.length(); i++) {
    if (result[i] >= 'A' && result[i] <= 'Z') {
      result[i] += ('a' - 'A');
    }
  }
  return result;
}

// ============================================================================
// trim — Strip leading and trailing whitespace
// ============================================================================
string trim(const string &s) {
  if (s.empty())
    return s;

  int start = 0;
  int end = (int)s.length() - 1;

  // Advance start past whitespace
  while (start <= end && (s[start] == ' ' || s[start] == '\t' ||
                          s[start] == '\r' || s[start] == '\n')) {
    start++;
  }

  // Retract end past whitespace
  while (end >= start && (s[end] == ' ' || s[end] == '\t' || s[end] == '\r' ||
                          s[end] == '\n')) {
    end--;
  }

  // Extract the trimmed substring
  if (start > end)
    return "";
  return s.substr(start, end - start + 1);
}

// ============================================================================
// doubleToStr — Convert double to string with fixed decimal places
// ============================================================================
string doubleToStr(double val, int decimals) {
  ostringstream oss;
  oss.precision(decimals);
  oss << fixed << val;
  return oss.str();
}
