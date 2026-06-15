#ifndef FUND_ALLOCATION_H
#define FUND_ALLOCATION_H

#include <string>

class FundAllocation {
private:
    std::string id;
    std::string beneficiaryId;
    std::string campaignId;
    double amount;
    std::string date;
    std::string purpose;
    std::string approvedBy;

public:
    FundAllocation();
    FundAllocation(const std::string& id, const std::string& beneficiaryId,
                   const std::string& campaignId, double amount,
                   const std::string& date, const std::string& purpose,
                   const std::string& approvedBy);

    std::string getId() const;
    std::string getBeneficiaryId() const;
    std::string getCampaignId() const;
    double getAmount() const;
    std::string getDate() const;
    std::string getPurpose() const;
    std::string getApprovedBy() const;

    void setId(const std::string& id);
    void setBeneficiaryId(const std::string& beneficiaryId);
    void setCampaignId(const std::string& campaignId);
    void setAmount(double amount);
    void setDate(const std::string& date);
    void setPurpose(const std::string& purpose);
    void setApprovedBy(const std::string& approvedBy);

    std::string displayInfo() const;
    std::string serialize() const;
    static FundAllocation deserialize(const std::string& line);
    static std::string header();
    std::string row() const;
};

#endif
