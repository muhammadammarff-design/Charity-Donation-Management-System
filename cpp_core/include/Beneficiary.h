#ifndef BENEFICIARY_H
#define BENEFICIARY_H

#include "Person.h"

#include <string>

using namespace std;

class Beneficiary : public Person {
private:
    int familySize;
    string needType;
    string campaignId;
    double totalReceived;

public:
    Beneficiary();
    Beneficiary(const string& id, const string& name, int age,
                const string& contact, const string& address,
                int familySize, const string& needType,
                const string& campaignId, double totalReceived = 0.0);
    Beneficiary(const string& name, const string& needType,
                const string& campaignId);

    int getFamilySize() const;
    string getNeedType() const;
    string getCampaignId() const;
    double getTotalReceived() const;

    void setFamilySize(int familySize);
    void setNeedType(const string& needType);
    void setCampaignId(const string& campaignId);
    void setTotalReceived(double totalReceived);
    void addReceivedAmount(double amount);

    string displayInfo() const override;
    string serialize() const;
    static Beneficiary deserialize(const string& line);
    static string header();
    string row() const;
};

#endif
