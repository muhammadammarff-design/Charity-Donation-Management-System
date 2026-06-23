#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <string>

using namespace std;

class Campaign {
private:
    string id;
    string title;
    string description;
    double targetAmount;
    double collectedAmount;
    double allocatedAmount;
    string startDate;
    string endDate;
    string status;

public:
    Campaign();
    Campaign(const string& id, const string& title,
             const string& description, double targetAmount,
             double collectedAmount, double allocatedAmount,
             const string& startDate, const string& endDate,
             const string& status);
    Campaign(const string& title, double targetAmount);

    string getId() const;
    string getTitle() const;
    string getDescription() const;
    double getTargetAmount() const;
    double getCollectedAmount() const;
    double getAllocatedAmount() const;
    string getStartDate() const;
    string getEndDate() const;
    string getStatus() const;

    void setId(const string& id);
    void setTitle(const string& title);
    void setDescription(const string& description);
    void setTargetAmount(double targetAmount);
    void setCollectedAmount(double collectedAmount);
    void setAllocatedAmount(double allocatedAmount);
    void setStartDate(const string& startDate);
    void setEndDate(const string& endDate);
    void setStatus(const string& status);
    void addCollectedAmount(double amount);
    void addAllocatedAmount(double amount);

    double getAvailableBalance() const;
    double getProgressPercent() const;

    string displayInfo() const;
    string serialize() const;
    static Campaign deserialize(const string& line);
    static string header();
    string row() const;

    double operator+(const Campaign& other) const;
    bool operator<(const Campaign& other) const;
    bool operator<(double amount) const;
};

#endif
