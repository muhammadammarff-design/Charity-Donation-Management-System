#ifndef VALIDATION_H
#define VALIDATION_H

#include <string>
#include <vector>

namespace Validation {
    std::string trim(const std::string& value);
    std::string sanitizeField(const std::string& value);
    std::vector<std::string> split(const std::string& line, char delimiter = '|');

    bool isNonEmpty(const std::string& value);
    bool isValidAge(int age);
    bool isValidContact(const std::string& contact);
    bool isValidEmail(const std::string& email);
    bool isValidDate(const std::string& date);       // YYYY-MM-DD
    bool isValidMonth(const std::string& month);     // YYYY-MM
    bool isPositiveAmount(double amount);

    bool safeToInt(const std::string& value, int& output);
    bool safeToDouble(const std::string& value, double& output);
    std::string toFixed2(double value);
}

#endif
