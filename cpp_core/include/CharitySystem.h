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

using namespace std;

struct OperationResult {
    bool success;
    string message;
    string id;
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
    vector<Donor> donors;
    vector<Beneficiary> beneficiaries;
    vector<Campaign> campaigns;
    vector<Donation> donations;
    vector<FundAllocation> allocations;
    vector<Report> reports;

    int findDonorIndex(const string& id) const;
    int findBeneficiaryIndex(const string& id) const;
    int findCampaignIndex(const string& id) const;
    bool donorHasDonations(const string& donorId) const;
    bool beneficiaryHasAllocations(const string& beneficiaryId) const;
    bool campaignHasReferences(const string& campaignId) const;
    string nextId(const string& prefix, const vector<string>& existingIds) const;
    string nextDonorId() const;
    string nextBeneficiaryId() const;
    string nextCampaignId() const;
    string nextDonationId() const;
    string nextAllocationId() const;
    string nextReportId() const;
    void recalculateTotals();

public:
    CharitySystem();

    bool loadAll();
    bool saveAll();
    bool clearAll();
    bool seedDemoData();

    OperationResult addDonor(const string& name, int age, const string& contact,
                             const string& email, const string& address,
                             const string& donorType);
    OperationResult updateDonor(const string& id, const string& name, int age,
                                const string& contact, const string& email,
                                const string& address, const string& donorType);
    OperationResult deleteDonor(const string& id);

    OperationResult addBeneficiary(const string& name, int age, const string& contact,
                                   const string& address, int familySize,
                                   const string& needType, const string& campaignId);
    OperationResult updateBeneficiary(const string& id, const string& name, int age,
                                      const string& contact, const string& address,
                                      int familySize, const string& needType,
                                      const string& campaignId);
    OperationResult deleteBeneficiary(const string& id);

    OperationResult addCampaign(const string& title, const string& description,
                                double targetAmount, const string& startDate,
                                const string& endDate, const string& status);
    OperationResult updateCampaign(const string& id, const string& title,
                                   const string& description, double targetAmount,
                                   const string& startDate, const string& endDate,
                                   const string& status);
    OperationResult deleteCampaign(const string& id);

    OperationResult recordDonation(const string& donorId, const string& campaignId,
                                   double amount, const string& date,
                                   const string& paymentMethod, const string& note);
    OperationResult allocateFunds(const string& beneficiaryId, const string& campaignId,
                                  double amount, const string& date,
                                  const string& purpose, const string& approvedBy);
    OperationResult generateMonthlyReport(const string& month);
    OperationResult generateCampaignReport(const string& campaignId);

    SystemSummary summary() const;
    string currentDate() const;

    const vector<Donor>& getDonors() const;
    const vector<Beneficiary>& getBeneficiaries() const;
    const vector<Campaign>& getCampaigns() const;
    const vector<Donation>& getDonations() const;
    const vector<FundAllocation>& getAllocations() const;
    const vector<Report>& getReports() const;
};

#endif
