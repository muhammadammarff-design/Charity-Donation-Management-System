#ifndef DONATION_H
#define DONATION_H

#include <string>

using namespace std;

class Donation {
private:
    string id;
    string donorId;
    string campaignId;
    double amount;
    string date;
    string paymentMethod;
    string note;

public:
    Donation();
    Donation(const string& id, const string& donorId,
             const string& campaignId, double amount,
             const string& date, const string& paymentMethod,
             const string& note);
    Donation(double amount, const string& date);

    string getId() const;
    string getDonorId() const;
    string getCampaignId() const;
    double getAmount() const;
    string getDate() const;
    string getPaymentMethod() const;
    string getNote() const;

    void setId(const string& id);
    void setDonorId(const string& donorId);
    void setCampaignId(const string& campaignId);
    void setAmount(double amount);
    void setDate(const string& date);
    void setPaymentMethod(const string& paymentMethod);
    void setNote(const string& note);

    string displayInfo() const;
    string serialize() const;
    static Donation deserialize(const string& line);
    static string header();
    string row() const;

    double operator+(const Donation& other) const;
    bool operator>(const Donation& other) const;
};

#endif
