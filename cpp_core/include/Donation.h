#ifndef DONATION_H
#define DONATION_H

#include <string>

class Donation {
private:
    std::string id;
    std::string donorId;
    std::string campaignId;
    double amount;
    std::string date;
    std::string paymentMethod;
    std::string note;

public:
    Donation();
    Donation(const std::string& id, const std::string& donorId,
             const std::string& campaignId, double amount,
             const std::string& date, const std::string& paymentMethod,
             const std::string& note);
    Donation(double amount, const std::string& date);

    std::string getId() const;
    std::string getDonorId() const;
    std::string getCampaignId() const;
    double getAmount() const;
    std::string getDate() const;
    std::string getPaymentMethod() const;
    std::string getNote() const;

    void setId(const std::string& id);
    void setDonorId(const std::string& donorId);
    void setCampaignId(const std::string& campaignId);
    void setAmount(double amount);
    void setDate(const std::string& date);
    void setPaymentMethod(const std::string& paymentMethod);
    void setNote(const std::string& note);

    std::string displayInfo() const;
    std::string serialize() const;
    static Donation deserialize(const std::string& line);
    static std::string header();
    std::string row() const;

    double operator+(const Donation& other) const;
    bool operator>(const Donation& other) const;
};

#endif
