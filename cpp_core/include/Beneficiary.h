#ifndef BENEFICIARY_H
#define BENEFICIARY_H

#include "Person.h"

#include <string>

class Beneficiary : public Person {
private:
    int familySize;
    std::string needType;
    std::string campaignId;
    double totalReceived;

public:
    Beneficiary();
    Beneficiary(const std::string& id, const std::string& name, int age,
                const std::string& contact, const std::string& address,
                int familySize, const std::string& needType,
                const std::string& campaignId, double totalReceived = 0.0);
    Beneficiary(const std::string& name, const std::string& needType,
                const std::string& campaignId);

    int getFamilySize() const;
    std::string getNeedType() const;
    std::string getCampaignId() const;
    double getTotalReceived() const;

    void setFamilySize(int familySize);
    void setNeedType(const std::string& needType);
    void setCampaignId(const std::string& campaignId);
    void setTotalReceived(double totalReceived);
    void addReceivedAmount(double amount);

    std::string displayInfo() const override;
    std::string serialize() const;
    static Beneficiary deserialize(const std::string& line);
    static std::string header();
    std::string row() const;
};

#endif
