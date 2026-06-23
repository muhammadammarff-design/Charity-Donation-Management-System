#include "Validation.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <regex>
#include <sstream>

using namespace std;

namespace Validation {

string trim(const string& value) {
    size_t start = 0;
    while (start < value.size() && isspace(static_cast<unsigned char>(value[start]))) {
        start++;
    }

    size_t end = value.size();
    while (end > start && isspace(static_cast<unsigned char>(value[end - 1]))) {
        end--;
    }

    return value.substr(start, end - start);
}

string sanitizeField(const string& value) {
    string cleaned = trim(value);
    for (char& ch : cleaned) {
        if (ch == '|' || ch == '\n' || ch == '\r' || ch == '\t') {
            ch = ' ';
        }
    }
    return cleaned;
}

vector<string> split(const string& line, char delimiter) {
    vector<string> parts;
    string current;
    stringstream ss(line);

    while (getline(ss, current, delimiter)) {
        parts.push_back(current);
    }

    // Preserve trailing empty field when a line ends with delimiter.
    if (!line.empty() && line.back() == delimiter) {
        parts.push_back("");
    }

    return parts;
}

bool isNonEmpty(const string& value) {
    return !trim(value).empty();
}

bool isValidAge(int age) {
    return age >= 1 && age <= 120;
}

bool isValidContact(const string& contact) {
    string trimmed = trim(contact);
    if (trimmed.empty()) {
        return false;
    }

    int digitCount = 0;
    for (char ch : trimmed) {
        if (isdigit(static_cast<unsigned char>(ch))) {
            digitCount++;
        } else if (ch == '+' || ch == '-' || ch == ' ' || ch == '(' || ch == ')') {
            continue;
        } else {
            return false;
        }
    }

    return digitCount >= 7 && digitCount <= 15;
}

bool isValidEmail(const string& email) {
    string trimmed = trim(email);
    if (trimmed.empty()) {
        return false;
    }

    // Normal student-project level validation: user@domain.tld
    static const regex pattern(R"(^[A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}$)");
    return regex_match(trimmed, pattern);
}

bool isValidDate(const string& date) {
    static const regex pattern(R"(^\d{4}-\d{2}-\d{2}$)");
    if (!regex_match(date, pattern)) {
        return false;
    }

    int year = 0, month = 0, day = 0;
    try {
        year = stoi(date.substr(0, 4));
        month = stoi(date.substr(5, 2));
        day = stoi(date.substr(8, 2));
    } catch (...) {
        return false;
    }

    if (year < 2000 || year > 2100 || month < 1 || month > 12) {
        return false;
    }

    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    bool leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
    if (leap) {
        daysInMonth[1] = 29;
    }

    return day >= 1 && day <= daysInMonth[month - 1];
}

bool isValidMonth(const string& month) {
    static const regex pattern(R"(^\d{4}-\d{2}$)");
    if (!regex_match(month, pattern)) {
        return false;
    }

    int yearValue = 0, monthValue = 0;
    try {
        yearValue = stoi(month.substr(0, 4));
        monthValue = stoi(month.substr(5, 2));
    } catch (...) {
        return false;
    }

    return yearValue >= 2000 && yearValue <= 2100 && monthValue >= 1 && monthValue <= 12;
}

bool isPositiveAmount(double amount) {
    return amount > 0.0;
}

bool safeToInt(const string& value, int& output) {
    try {
        size_t pos = 0;
        int parsed = stoi(trim(value), &pos);
        if (pos != trim(value).size()) {
            return false;
        }
        output = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool safeToDouble(const string& value, double& output) {
    try {
        size_t pos = 0;
        double parsed = stod(trim(value), &pos);
        if (pos != trim(value).size()) {
            return false;
        }
        output = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

string toFixed2(double value) {
    ostringstream out;
    out << fixed << setprecision(2) << value;
    return out.str();
}

}
