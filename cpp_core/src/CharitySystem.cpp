#include "CharitySystem.h"
#include "FileManager.h"
#include "Validation.h"

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <sstream>

using std::string;
using std::vector;

CharitySystem::CharitySystem() {
    loadAll();
}

int CharitySystem::findDonorIndex(const string& id) const {
    for (size_t i = 0; i < donors.size(); ++i) {
        if (donors[i].getId() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int CharitySystem::findBeneficiaryIndex(const string& id) const {
    for (size_t i = 0; i < beneficiaries.size(); ++i) {
        if (beneficiaries[i].getId() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int CharitySystem::findCampaignIndex(const string& id) const {
    for (size_t i = 0; i < campaigns.size(); ++i) {
        if (campaigns[i].getId() == id) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool CharitySystem::donorHasDonations(const string& donorId) const {
    for (const Donation& donation : donations) {
        if (donation.getDonorId() == donorId) {
            return true;
        }
    }
    return false;
}

bool CharitySystem::beneficiaryHasAllocations(const string& beneficiaryId) const {
    for (const FundAllocation& allocation : allocations) {
        if (allocation.getBeneficiaryId() == beneficiaryId) {
            return true;
        }
    }
    return false;
}

bool CharitySystem::campaignHasReferences(const string& campaignId) const {
    for (const Donation& donation : donations) {
        if (donation.getCampaignId() == campaignId) {
            return true;
        }
    }
    for (const FundAllocation& allocation : allocations) {
        if (allocation.getCampaignId() == campaignId) {
            return true;
        }
    }
    for (const Beneficiary& beneficiary : beneficiaries) {
        if (beneficiary.getCampaignId() == campaignId) {
            return true;
        }
    }
    return false;
}

string CharitySystem::nextId(const string& prefix, const vector<string>& existingIds) const {
    int maxNumber = 0;
    for (const string& id : existingIds) {
        if (id.rfind(prefix, 0) == 0 && id.size() > prefix.size()) {
            int number = 0;
            if (Validation::safeToInt(id.substr(prefix.size()), number)) {
                maxNumber = std::max(maxNumber, number);
            }
        }
    }

    std::ostringstream out;
    out << prefix << std::setw(3) << std::setfill('0') << (maxNumber + 1);
    return out.str();
}

string CharitySystem::nextDonorId() const {
    vector<string> ids;
    for (const Donor& donor : donors) ids.push_back(donor.getId());
    return nextId("DNR", ids);
}

string CharitySystem::nextBeneficiaryId() const {
    vector<string> ids;
    for (const Beneficiary& beneficiary : beneficiaries) ids.push_back(beneficiary.getId());
    return nextId("BEN", ids);
}

string CharitySystem::nextCampaignId() const {
    vector<string> ids;
    for (const Campaign& campaign : campaigns) ids.push_back(campaign.getId());
    return nextId("CAM", ids);
}

string CharitySystem::nextDonationId() const {
    vector<string> ids;
    for (const Donation& donation : donations) ids.push_back(donation.getId());
    return nextId("DNT", ids);
}

string CharitySystem::nextAllocationId() const {
    vector<string> ids;
    for (const FundAllocation& allocation : allocations) ids.push_back(allocation.getId());
    return nextId("ALC", ids);
}

string CharitySystem::nextReportId() const {
    vector<string> ids;
    for (const Report& report : reports) ids.push_back(report.getId());
    return nextId("RPT", ids);
}

void CharitySystem::recalculateTotals() {
    for (Donor& donor : donors) {
        donor.setTotalDonated(0.0);
    }
    for (Beneficiary& beneficiary : beneficiaries) {
        beneficiary.setTotalReceived(0.0);
    }
    for (Campaign& campaign : campaigns) {
        campaign.setCollectedAmount(0.0);
        campaign.setAllocatedAmount(0.0);
    }

    for (const Donation& donation : donations) {
        int donorIndex = findDonorIndex(donation.getDonorId());
        int campaignIndex = findCampaignIndex(donation.getCampaignId());
        if (donorIndex != -1) {
            donors[donorIndex].addDonationAmount(donation.getAmount());
        }
        if (campaignIndex != -1) {
            campaigns[campaignIndex].addCollectedAmount(donation.getAmount());
        }
    }

    for (const FundAllocation& allocation : allocations) {
        int beneficiaryIndex = findBeneficiaryIndex(allocation.getBeneficiaryId());
        int campaignIndex = findCampaignIndex(allocation.getCampaignId());
        if (beneficiaryIndex != -1) {
            beneficiaries[beneficiaryIndex].addReceivedAmount(allocation.getAmount());
        }
        if (campaignIndex != -1) {
            campaigns[campaignIndex].addAllocatedAmount(allocation.getAmount());
        }
    }
}

bool CharitySystem::loadAll() {
    donors.clear();
    beneficiaries.clear();
    campaigns.clear();
    donations.clear();
    allocations.clear();
    reports.clear();

    for (const string& line : FileManager::readLines("donors.txt")) {
        Donor donor = Donor::deserialize(line);
        if (!donor.getId().empty()) donors.push_back(donor);
    }
    for (const string& line : FileManager::readLines("beneficiaries.txt")) {
        Beneficiary beneficiary = Beneficiary::deserialize(line);
        if (!beneficiary.getId().empty()) beneficiaries.push_back(beneficiary);
    }
    for (const string& line : FileManager::readLines("campaigns.txt")) {
        Campaign campaign = Campaign::deserialize(line);
        if (!campaign.getId().empty()) campaigns.push_back(campaign);
    }
    for (const string& line : FileManager::readLines("donations.txt")) {
        Donation donation = Donation::deserialize(line);
        if (!donation.getId().empty()) donations.push_back(donation);
    }
    for (const string& line : FileManager::readLines("allocations.txt")) {
        FundAllocation allocation = FundAllocation::deserialize(line);
        if (!allocation.getId().empty()) allocations.push_back(allocation);
    }
    for (const string& line : FileManager::readLines("reports.txt")) {
        Report report = Report::deserialize(line);
        if (!report.getId().empty()) reports.push_back(report);
    }

    recalculateTotals();
    return true;
}

bool CharitySystem::saveAll() {
    recalculateTotals();

    vector<string> donorLines;
    for (const Donor& donor : donors) donorLines.push_back(donor.serialize());

    vector<string> beneficiaryLines;
    for (const Beneficiary& beneficiary : beneficiaries) beneficiaryLines.push_back(beneficiary.serialize());

    vector<string> campaignLines;
    for (const Campaign& campaign : campaigns) campaignLines.push_back(campaign.serialize());

    vector<string> donationLines;
    for (const Donation& donation : donations) donationLines.push_back(donation.serialize());

    vector<string> allocationLines;
    for (const FundAllocation& allocation : allocations) allocationLines.push_back(allocation.serialize());

    vector<string> reportLines;
    for (const Report& report : reports) reportLines.push_back(report.serialize());

    return FileManager::writeLines("donors.txt", donorLines) &&
           FileManager::writeLines("beneficiaries.txt", beneficiaryLines) &&
           FileManager::writeLines("campaigns.txt", campaignLines) &&
           FileManager::writeLines("donations.txt", donationLines) &&
           FileManager::writeLines("allocations.txt", allocationLines) &&
           FileManager::writeLines("reports.txt", reportLines);
}

bool CharitySystem::clearAll() {
    donors.clear();
    beneficiaries.clear();
    campaigns.clear();
    donations.clear();
    allocations.clear();
    reports.clear();
    return saveAll();
}

bool CharitySystem::seedDemoData() {
    clearAll();

    OperationResult c1 = addCampaign("Flood Relief 2026", "Emergency food, shelter, and medicines for flood affected families.", 500000.0, "2026-06-01", "2026-07-31", "Active");
    OperationResult c2 = addCampaign("Education Support", "School fees, books, bags, and uniforms for deserving students.", 300000.0, "2026-06-05", "2026-08-30", "Active");
    OperationResult c3 = addCampaign("Medical Aid Drive", "Treatment and medicines for low-income patients.", 400000.0, "2026-06-10", "2026-09-15", "Active");

    OperationResult d1 = addDonor("Ahmed Khan", 35, "+92 300 1111111", "ahmed.khan@example.com", "Lahore", "Individual");
    OperationResult d2 = addDonor("Fatima Foundation", 12, "+92 321 2222222", "contact@fatimafoundation.org", "Karachi", "Organization");
    OperationResult d3 = addDonor("Sara Ali", 28, "+92 333 3333333", "sara.ali@example.com", "Islamabad", "Individual");

    OperationResult b1 = addBeneficiary("Bilal Family", 42, "+92 311 4444444", "Sukkur", 6, "Food and Shelter", c1.id);
    OperationResult b2 = addBeneficiary("Hina Student", 16, "+92 312 5555555", "Lahore", 1, "Education Fee", c2.id);
    OperationResult b3 = addBeneficiary("Zain Patient", 31, "+92 313 6666666", "Karachi", 4, "Medical Treatment", c3.id);

    recordDonation(d1.id, c1.id, 75000.0, "2026-06-12", "Bank Transfer", "Initial flood relief support");
    recordDonation(d2.id, c1.id, 125000.0, "2026-06-14", "Cheque", "Corporate charity donation");
    recordDonation(d2.id, c2.id, 90000.0, "2026-06-15", "Bank Transfer", "Education sponsorship");
    recordDonation(d3.id, c3.id, 55000.0, "2026-06-16", "Cash", "Medical aid donation");
    allocateFunds(b1.id, c1.id, 60000.0, "2026-06-18", "Ration bags and temporary shelter", "Admin");
    allocateFunds(b2.id, c2.id, 35000.0, "2026-06-20", "School fee and books", "Admin");
    allocateFunds(b3.id, c3.id, 30000.0, "2026-06-21", "Medicine purchase", "Admin");
    generateMonthlyReport("2026-06");
    generateCampaignReport(c1.id);

    return true;
}

OperationResult CharitySystem::addDonor(const string& name, int age, const string& contact,
                                        const string& email, const string& address,
                                        const string& donorType) {
    if (!Validation::isNonEmpty(name)) return {false, "Donor name cannot be empty.", ""};
    if (!Validation::isValidAge(age)) return {false, "Age must be between 1 and 120.", ""};
    if (!Validation::isValidContact(contact)) return {false, "Contact must contain 7 to 15 digits and valid phone characters only.", ""};
    if (!Validation::isValidEmail(email)) return {false, "Email format is invalid. Use example@domain.com.", ""};
    if (!Validation::isNonEmpty(address)) return {false, "Address cannot be empty.", ""};
    if (!Validation::isNonEmpty(donorType)) return {false, "Donor type cannot be empty.", ""};

    for (const Donor& donor : donors) {
        if (donor.getEmail() == Validation::sanitizeField(email)) {
            return {false, "A donor with this email already exists.", ""};
        }
    }

    string id = nextDonorId();
    donors.push_back(Donor(id, name, age, contact, email, address, donorType, 0.0));
    saveAll();
    return {true, "Donor added successfully.", id};
}

OperationResult CharitySystem::updateDonor(const string& id, const string& name, int age,
                                           const string& contact, const string& email,
                                           const string& address, const string& donorType) {
    int index = findDonorIndex(id);
    if (index == -1) return {false, "Donor ID not found.", ""};
    if (!Validation::isNonEmpty(name)) return {false, "Donor name cannot be empty.", ""};
    if (!Validation::isValidAge(age)) return {false, "Age must be between 1 and 120.", ""};
    if (!Validation::isValidContact(contact)) return {false, "Contact must contain 7 to 15 digits and valid phone characters only.", ""};
    if (!Validation::isValidEmail(email)) return {false, "Email format is invalid. Use example@domain.com.", ""};
    if (!Validation::isNonEmpty(address)) return {false, "Address cannot be empty.", ""};
    if (!Validation::isNonEmpty(donorType)) return {false, "Donor type cannot be empty.", ""};

    for (const Donor& donor : donors) {
        if (donor.getId() != id && donor.getEmail() == Validation::sanitizeField(email)) {
            return {false, "Another donor already uses this email.", ""};
        }
    }

    double existingTotal = donors[index].getTotalDonated();
    donors[index] = Donor(id, name, age, contact, email, address, donorType, existingTotal);
    saveAll();
    return {true, "Donor updated successfully.", id};
}

OperationResult CharitySystem::deleteDonor(const string& id) {
    int index = findDonorIndex(id);
    if (index == -1) return {false, "Donor ID not found.", ""};
    if (donorHasDonations(id)) return {false, "Cannot delete donor because donation history exists.", ""};

    donors.erase(donors.begin() + index);
    saveAll();
    return {true, "Donor deleted successfully.", id};
}

OperationResult CharitySystem::addBeneficiary(const string& name, int age, const string& contact,
                                              const string& address, int familySize,
                                              const string& needType, const string& campaignId) {
    if (!Validation::isNonEmpty(name)) return {false, "Beneficiary name cannot be empty.", ""};
    if (!Validation::isValidAge(age)) return {false, "Age must be between 1 and 120.", ""};
    if (!Validation::isValidContact(contact)) return {false, "Contact must contain 7 to 15 digits and valid phone characters only.", ""};
    if (!Validation::isNonEmpty(address)) return {false, "Address cannot be empty.", ""};
    if (familySize < 1 || familySize > 50) return {false, "Family size must be between 1 and 50.", ""};
    if (!Validation::isNonEmpty(needType)) return {false, "Need type cannot be empty.", ""};
    if (findCampaignIndex(campaignId) == -1) return {false, "Campaign ID does not exist.", ""};

    string id = nextBeneficiaryId();
    beneficiaries.push_back(Beneficiary(id, name, age, contact, address, familySize, needType, campaignId, 0.0));
    saveAll();
    return {true, "Beneficiary added successfully.", id};
}

OperationResult CharitySystem::updateBeneficiary(const string& id, const string& name, int age,
                                                 const string& contact, const string& address,
                                                 int familySize, const string& needType,
                                                 const string& campaignId) {
    int index = findBeneficiaryIndex(id);
    if (index == -1) return {false, "Beneficiary ID not found.", ""};
    if (!Validation::isNonEmpty(name)) return {false, "Beneficiary name cannot be empty.", ""};
    if (!Validation::isValidAge(age)) return {false, "Age must be between 1 and 120.", ""};
    if (!Validation::isValidContact(contact)) return {false, "Contact must contain 7 to 15 digits and valid phone characters only.", ""};
    if (!Validation::isNonEmpty(address)) return {false, "Address cannot be empty.", ""};
    if (familySize < 1 || familySize > 50) return {false, "Family size must be between 1 and 50.", ""};
    if (!Validation::isNonEmpty(needType)) return {false, "Need type cannot be empty.", ""};
    if (findCampaignIndex(campaignId) == -1) return {false, "Campaign ID does not exist.", ""};

    double existingTotal = beneficiaries[index].getTotalReceived();
    beneficiaries[index] = Beneficiary(id, name, age, contact, address, familySize, needType, campaignId, existingTotal);
    saveAll();
    return {true, "Beneficiary updated successfully.", id};
}

OperationResult CharitySystem::deleteBeneficiary(const string& id) {
    int index = findBeneficiaryIndex(id);
    if (index == -1) return {false, "Beneficiary ID not found.", ""};
    if (beneficiaryHasAllocations(id)) return {false, "Cannot delete beneficiary because aid allocation history exists.", ""};

    beneficiaries.erase(beneficiaries.begin() + index);
    saveAll();
    return {true, "Beneficiary deleted successfully.", id};
}

OperationResult CharitySystem::addCampaign(const string& title, const string& description,
                                           double targetAmount, const string& startDate,
                                           const string& endDate, const string& status) {
    if (!Validation::isNonEmpty(title)) return {false, "Campaign title cannot be empty.", ""};
    if (!Validation::isNonEmpty(description)) return {false, "Campaign description cannot be empty.", ""};
    if (!Validation::isPositiveAmount(targetAmount)) return {false, "Campaign target amount must be greater than zero.", ""};
    if (!Validation::isValidDate(startDate)) return {false, "Start date must be valid and in YYYY-MM-DD format.", ""};
    if (!Validation::isValidDate(endDate)) return {false, "End date must be valid and in YYYY-MM-DD format.", ""};
    if (endDate < startDate) return {false, "End date cannot be earlier than start date.", ""};
    if (!Validation::isNonEmpty(status)) return {false, "Campaign status cannot be empty.", ""};

    string id = nextCampaignId();
    campaigns.push_back(Campaign(id, title, description, targetAmount, 0.0, 0.0, startDate, endDate, status));
    saveAll();
    return {true, "Campaign added successfully.", id};
}

OperationResult CharitySystem::updateCampaign(const string& id, const string& title,
                                              const string& description, double targetAmount,
                                              const string& startDate, const string& endDate,
                                              const string& status) {
    int index = findCampaignIndex(id);
    if (index == -1) return {false, "Campaign ID not found.", ""};
    if (!Validation::isNonEmpty(title)) return {false, "Campaign title cannot be empty.", ""};
    if (!Validation::isNonEmpty(description)) return {false, "Campaign description cannot be empty.", ""};
    if (!Validation::isPositiveAmount(targetAmount)) return {false, "Campaign target amount must be greater than zero.", ""};
    if (targetAmount < campaigns[index].getCollectedAmount()) return {false, "Target cannot be lower than already collected amount.", ""};
    if (!Validation::isValidDate(startDate)) return {false, "Start date must be valid and in YYYY-MM-DD format.", ""};
    if (!Validation::isValidDate(endDate)) return {false, "End date must be valid and in YYYY-MM-DD format.", ""};
    if (endDate < startDate) return {false, "End date cannot be earlier than start date.", ""};
    if (!Validation::isNonEmpty(status)) return {false, "Campaign status cannot be empty.", ""};

    double collected = campaigns[index].getCollectedAmount();
    double allocated = campaigns[index].getAllocatedAmount();
    campaigns[index] = Campaign(id, title, description, targetAmount, collected, allocated, startDate, endDate, status);
    saveAll();
    return {true, "Campaign updated successfully.", id};
}

OperationResult CharitySystem::deleteCampaign(const string& id) {
    int index = findCampaignIndex(id);
    if (index == -1) return {false, "Campaign ID not found.", ""};
    if (campaignHasReferences(id)) return {false, "Cannot delete campaign because it is linked with donors, beneficiaries, donations, or allocations.", ""};

    campaigns.erase(campaigns.begin() + index);
    saveAll();
    return {true, "Campaign deleted successfully.", id};
}

OperationResult CharitySystem::recordDonation(const string& donorId, const string& campaignId,
                                              double amount, const string& date,
                                              const string& paymentMethod, const string& note) {
    if (findDonorIndex(donorId) == -1) return {false, "Donor ID does not exist.", ""};
    if (findCampaignIndex(campaignId) == -1) return {false, "Campaign ID does not exist.", ""};
    if (!Validation::isPositiveAmount(amount)) return {false, "Donation amount must be greater than zero.", ""};
    if (!Validation::isValidDate(date)) return {false, "Donation date must be valid and in YYYY-MM-DD format.", ""};
    if (!Validation::isNonEmpty(paymentMethod)) return {false, "Payment method cannot be empty.", ""};

    string id = nextDonationId();
    donations.push_back(Donation(id, donorId, campaignId, amount, date, paymentMethod, note));
    saveAll();
    return {true, "Donation recorded successfully.", id};
}

OperationResult CharitySystem::allocateFunds(const string& beneficiaryId, const string& campaignId,
                                             double amount, const string& date,
                                             const string& purpose, const string& approvedBy) {
    int beneficiaryIndex = findBeneficiaryIndex(beneficiaryId);
    int campaignIndex = findCampaignIndex(campaignId);

    if (beneficiaryIndex == -1) return {false, "Beneficiary ID does not exist.", ""};
    if (campaignIndex == -1) return {false, "Campaign ID does not exist.", ""};
    if (beneficiaries[beneficiaryIndex].getCampaignId() != campaignId) {
        return {false, "Beneficiary is not linked to this campaign.", ""};
    }
    if (!Validation::isPositiveAmount(amount)) return {false, "Allocation amount must be greater than zero.", ""};
    if (!Validation::isValidDate(date)) return {false, "Allocation date must be valid and in YYYY-MM-DD format.", ""};
    if (!Validation::isNonEmpty(purpose)) return {false, "Allocation purpose cannot be empty.", ""};
    if (!Validation::isNonEmpty(approvedBy)) return {false, "Approved by field cannot be empty.", ""};

    recalculateTotals();
    double available = campaigns[campaignIndex].getAvailableBalance();
    if (amount > available) {
        return {false, "Allocation exceeds available campaign balance. Available: " + Validation::toFixed2(available), ""};
    }

    string id = nextAllocationId();
    allocations.push_back(FundAllocation(id, beneficiaryId, campaignId, amount, date, purpose, approvedBy));
    saveAll();
    return {true, "Funds allocated successfully.", id};
}

OperationResult CharitySystem::generateMonthlyReport(const string& month) {
    if (!Validation::isValidMonth(month)) return {false, "Month must be valid and in YYYY-MM format.", ""};

    double totalDonation = 0.0;
    double totalAllocation = 0.0;
    for (const Donation& donation : donations) {
        if (donation.getDate().rfind(month, 0) == 0) {
            totalDonation += donation.getAmount();
        }
    }
    for (const FundAllocation& allocation : allocations) {
        if (allocation.getDate().rfind(month, 0) == 0) {
            totalAllocation += allocation.getAmount();
        }
    }

    string id = nextReportId();
    Report report(id, "Monthly", month, totalDonation, totalAllocation,
                  totalDonation - totalAllocation, currentDate(),
                  "Generated monthly transparency report.");
    reports.push_back(report);
    saveAll();
    return {true, "Monthly report generated successfully.", id};
}

OperationResult CharitySystem::generateCampaignReport(const string& campaignId) {
    int campaignIndex = findCampaignIndex(campaignId);
    if (campaignIndex == -1) return {false, "Campaign ID does not exist.", ""};

    double totalDonation = 0.0;
    double totalAllocation = 0.0;
    for (const Donation& donation : donations) {
        if (donation.getCampaignId() == campaignId) totalDonation += donation.getAmount();
    }
    for (const FundAllocation& allocation : allocations) {
        if (allocation.getCampaignId() == campaignId) totalAllocation += allocation.getAmount();
    }

    string id = nextReportId();
    string period = campaignId + " - " + campaigns[campaignIndex].getTitle();
    Report report(id, "Campaign", period, totalDonation, totalAllocation,
                  totalDonation - totalAllocation, currentDate(),
                  "Generated campaign-level transparency report.");
    reports.push_back(report);
    saveAll();
    return {true, "Campaign report generated successfully.", id};
}

SystemSummary CharitySystem::summary() const {
    SystemSummary s{};
    s.donorCount = static_cast<int>(donors.size());
    s.beneficiaryCount = static_cast<int>(beneficiaries.size());
    s.campaignCount = static_cast<int>(campaigns.size());
    s.donationCount = static_cast<int>(donations.size());
    s.allocationCount = static_cast<int>(allocations.size());
    s.totalDonations = 0.0;
    s.totalAllocations = 0.0;

    for (const Donation& donation : donations) s.totalDonations += donation.getAmount();
    for (const FundAllocation& allocation : allocations) s.totalAllocations += allocation.getAmount();
    s.remainingBalance = s.totalDonations - s.totalAllocations;
    return s;
}

string CharitySystem::currentDate() const {
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream out;
    out << std::put_time(localTime, "%Y-%m-%d");
    return out.str();
}

const vector<Donor>& CharitySystem::getDonors() const { return donors; }
const vector<Beneficiary>& CharitySystem::getBeneficiaries() const { return beneficiaries; }
const vector<Campaign>& CharitySystem::getCampaigns() const { return campaigns; }
const vector<Donation>& CharitySystem::getDonations() const { return donations; }
const vector<FundAllocation>& CharitySystem::getAllocations() const { return allocations; }
const vector<Report>& CharitySystem::getReports() const { return reports; }
