#include "Donor.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Donor::Donor()
    : Person(), email(""), donorType("Individual"), totalDonated(0.0) {}

Donor::Donor(const string& id, const string& name, int age,
             const string& contact, const string& email,
             const string& address, const string& donorType,
             double totalDonated)
    : Person(id, name, age, contact, address),
      email(Validation::sanitizeField(email)),
      donorType(Validation::sanitizeField(donorType)),
      totalDonated(totalDonated) {}

Donor::Donor(const string& name, const string& contact,
             const string& email)
    : Person("", name, 0, contact, ""),
      email(Validation::sanitizeField(email)),
      donorType("Individual"),
      totalDonated(0.0) {}

string Donor::getEmail() const { return email; }
string Donor::getDonorType() const { return donorType; }
double Donor::getTotalDonated() const { return totalDonated; }

void Donor::setEmail(const string& email) { this->email = Validation::sanitizeField(email); }
void Donor::setDonorType(const string& donorType) { this->donorType = Validation::sanitizeField(donorType); }
void Donor::setTotalDonated(double totalDonated) { this->totalDonated = totalDonated; }
void Donor::addDonationAmount(double amount) { totalDonated += amount; }

string Donor::displayInfo() const {
    ostringstream out;
    out << "Donor => " << Person::displayInfo()
        << ", Email: " << email
        << ", Type: " << donorType
        << ", Total Donated: " << Validation::toFixed2(totalDonated);
    return out.str();
}

string Donor::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(name) << '|'
        << age << '|'
        << Validation::sanitizeField(contact) << '|'
        << Validation::sanitizeField(email) << '|'
        << Validation::sanitizeField(address) << '|'
        << Validation::sanitizeField(donorType) << '|'
        << Validation::toFixed2(totalDonated);
    return out.str();
}

Donor Donor::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 8) {
        return Donor();
    }

    int parsedAge = 0;
    double parsedTotal = 0.0;
    Validation::safeToInt(p[2], parsedAge);
    Validation::safeToDouble(p[7], parsedTotal);

    return Donor(p[0], p[1], parsedAge, p[3], p[4], p[5], p[6], parsedTotal);
}

string Donor::header() {
    return "id|name|age|contact|email|address|donorType|totalDonated";
}

string Donor::row() const {
    return serialize();
}

double Donor::operator+(const Donor& other) const {
    return totalDonated + other.totalDonated;
}

bool Donor::operator>(const Donor& other) const {
    return totalDonated > other.totalDonated;
}
