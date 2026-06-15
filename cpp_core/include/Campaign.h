#ifndef CAMPAIGN_H
#define CAMPAIGN_H

#include <string>

class Campaign {
private:
    std::string id;
    std::string title;
    std::string description;
    double targetAmount;
    double collectedAmount;
    double allocatedAmount;
    std::string startDate;
    std::string endDate;
    std::string status;

public:
    Campaign();
    Campaign(const std::string& id, const std::string& title,
             const std::string& description, double targetAmount,
             double collectedAmount, double allocatedAmount,
             const std::string& startDate, const std::string& endDate,
             const std::string& status);
    Campaign(const std::string& title, double targetAmount);

    std::string getId() const;
    std::string getTitle() const;
    std::string getDescription() const;
    double getTargetAmount() const;
    double getCollectedAmount() const;
    double getAllocatedAmount() const;
    std::string getStartDate() const;
    std::string getEndDate() const;
    std::string getStatus() const;

    void setId(const std::string& id);
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    void setTargetAmount(double targetAmount);
    void setCollectedAmount(double collectedAmount);
    void setAllocatedAmount(double allocatedAmount);
    void setStartDate(const std::string& startDate);
    void setEndDate(const std::string& endDate);
    void setStatus(const std::string& status);
    void addCollectedAmount(double amount);
    void addAllocatedAmount(double amount);

    double getAvailableBalance() const;
    double getProgressPercent() const;

    std::string displayInfo() const;
    std::string serialize() const;
    static Campaign deserialize(const std::string& line);
    static std::string header();
    std::string row() const;

    double operator+(const Campaign& other) const;
    bool operator<(const Campaign& other) const;
    bool operator<(double amount) const;
};

#endif
