#ifndef DONOR_H
#define DONOR_H

#include "Person.h"

#include <string>

class Donor : public Person {
private:
    std::string email;
    std::string donorType;
    double totalDonated;

public:
    Donor();
    Donor(const std::string& id, const std::string& name, int age,
          const std::string& contact, const std::string& email,
          const std::string& address, const std::string& donorType,
          double totalDonated = 0.0);
    Donor(const std::string& name, const std::string& contact,
          const std::string& email);

    std::string getEmail() const;
    std::string getDonorType() const;
    double getTotalDonated() const;

    void setEmail(const std::string& email);
    void setDonorType(const std::string& donorType);
    void setTotalDonated(double totalDonated);
    void addDonationAmount(double amount);

    std::string displayInfo() const override;
    std::string serialize() const;
    static Donor deserialize(const std::string& line);
    static std::string header();
    std::string row() const;

    double operator+(const Donor& other) const;
    bool operator>(const Donor& other) const;
};

#endif
