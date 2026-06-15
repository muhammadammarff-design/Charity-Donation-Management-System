#ifndef CHARITY_SYSTEM_H
#define CHARITY_SYSTEM_H

#include "Beneficiary.h"
#include "Campaign.h"
#include "Donation.h"
#include "Donor.h"
#include "FundAllocation.h"
#include "Report.h"

#include <string>
#include <vector>

struct OperationResult {
    bool success;
    std::string message;
    std::string id;
};

struct SystemSummary {
    int donorCount;
    int beneficiaryCount;
    int campaignCount;
    int donationCount;
    int allocationCount;
    double totalDonations;
    double totalAllocations;
    double remainingBalance;
};

class CharitySystem {
private:
    std::vector<Donor> donors;
    std::vector<Beneficiary> beneficiaries;
    std::vector<Campaign> campaigns;
    std::vector<Donation> donations;
    std::vector<FundAllocation> allocations;
    std::vector<Report> reports;

    int findDonorIndex(const std::string& id) const;
    int findBeneficiaryIndex(const std::string& id) const;
    int findCampaignIndex(const std::string& id) const;
    bool donorHasDonations(const std::string& donorId) const;
    bool beneficiaryHasAllocations(const std::string& beneficiaryId) const;
    bool campaignHasReferences(const std::string& campaignId) const;
    std::string nextId(const std::string& prefix, const std::vector<std::string>& existingIds) const;
    std::string nextDonorId() const;
    std::string nextBeneficiaryId() const;
    std::string nextCampaignId() const;
    std::string nextDonationId() const;
    std::string nextAllocationId() const;
    std::string nextReportId() const;
    void recalculateTotals();

public:
    CharitySystem();

    bool loadAll();
    bool saveAll();
    bool clearAll();
    bool seedDemoData();

    OperationResult addDonor(const std::string& name, int age, const std::string& contact,
                             const std::string& email, const std::string& address,
                             const std::string& donorType);
    OperationResult updateDonor(const std::string& id, const std::string& name, int age,
                                const std::string& contact, const std::string& email,
                                const std::string& address, const std::string& donorType);
    OperationResult deleteDonor(const std::string& id);

    OperationResult addBeneficiary(const std::string& name, int age, const std::string& contact,
                                   const std::string& address, int familySize,
                                   const std::string& needType, const std::string& campaignId);
    OperationResult updateBeneficiary(const std::string& id, const std::string& name, int age,
                                      const std::string& contact, const std::string& address,
                                      int familySize, const std::string& needType,
                                      const std::string& campaignId);
    OperationResult deleteBeneficiary(const std::string& id);

    OperationResult addCampaign(const std::string& title, const std::string& description,
                                double targetAmount, const std::string& startDate,
                                const std::string& endDate, const std::string& status);
    OperationResult updateCampaign(const std::string& id, const std::string& title,
                                   const std::string& description, double targetAmount,
                                   const std::string& startDate, const std::string& endDate,
                                   const std::string& status);
    OperationResult deleteCampaign(const std::string& id);

    OperationResult recordDonation(const std::string& donorId, const std::string& campaignId,
                                   double amount, const std::string& date,
                                   const std::string& paymentMethod, const std::string& note);
    OperationResult allocateFunds(const std::string& beneficiaryId, const std::string& campaignId,
                                  double amount, const std::string& date,
                                  const std::string& purpose, const std::string& approvedBy);
    OperationResult generateMonthlyReport(const std::string& month);
    OperationResult generateCampaignReport(const std::string& campaignId);

    SystemSummary summary() const;
    std::string currentDate() const;

    const std::vector<Donor>& getDonors() const;
    const std::vector<Beneficiary>& getBeneficiaries() const;
    const std::vector<Campaign>& getCampaigns() const;
    const std::vector<Donation>& getDonations() const;
    const std::vector<FundAllocation>& getAllocations() const;
    const std::vector<Report>& getReports() const;
};

#endif
