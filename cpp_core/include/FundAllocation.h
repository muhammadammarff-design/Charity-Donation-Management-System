#ifndef FUND_ALLOCATION_H
#define FUND_ALLOCATION_H

#include <string>

using namespace std;

class FundAllocation {
private:
    string id;
    string beneficiaryId;
    string campaignId;
    double amount;
    string date;
    string purpose;
    string approvedBy;

public:
    FundAllocation();
    FundAllocation(const string& id, const string& beneficiaryId,
                   const string& campaignId, double amount,
                   const string& date, const string& purpose,
                   const string& approvedBy);

    string getId() const;
    string getBeneficiaryId() const;
    string getCampaignId() const;
    double getAmount() const;
    string getDate() const;
    string getPurpose() const;
    string getApprovedBy() const;

    void setId(const string& id);
    void setBeneficiaryId(const string& beneficiaryId);
    void setCampaignId(const string& campaignId);
    void setAmount(double amount);
    void setDate(const string& date);
    void setPurpose(const string& purpose);
    void setApprovedBy(const string& approvedBy);

    string displayInfo() const;
    string serialize() const;
    static FundAllocation deserialize(const string& line);
    static string header();
    string row() const;
};

#endif
