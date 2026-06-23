#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>
#include <vector>

using namespace std;

namespace Validation {
    string trim(const string& value);
    string sanitizeField(const string& value);
    vector<string> split(const string& line, char delimiter = '|');

    bool isNonEmpty(const string& value);
    bool isValidAge(int age);
    bool isValidContact(const string& contact);
    bool isValidEmail(const string& email);
    bool isValidDate(const string& date);       // YYYY-MM-DD
    bool isValidMonth(const string& month);     // YYYY-MM
    bool isPositiveAmount(double amount);

    bool safeToInt(const string& value, int& output);
    bool safeToDouble(const string& value, double& output);
    string toFixed2(double value);
}

#endif
