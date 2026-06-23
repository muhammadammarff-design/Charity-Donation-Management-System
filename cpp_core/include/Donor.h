#ifndef DONOR_H
#define DONOR_H

#include "Person.h"

#include <string>

using namespace std;

class Donor : public Person {
private:
    string email;
    string donorType;
    double totalDonated;

public:
    Donor();
    Donor(const string& id, const string& name, int age,
          const string& contact, const string& email,
          const string& address, const string& donorType,
          double totalDonated = 0.0);
    Donor(const string& name, const string& contact,
          const string& email);

    string getEmail() const;
    string getDonorType() const;
    double getTotalDonated() const;

    void setEmail(const string& email);
    void setDonorType(const string& donorType);
    void setTotalDonated(double totalDonated);
    void addDonationAmount(double amount);

    string displayInfo() const override;
    string serialize() const;
    static Donor deserialize(const string& line);
    static string header();
    string row() const;

    double operator+(const Donor& other) const;
    bool operator>(const Donor& other) const;
};

#endif
