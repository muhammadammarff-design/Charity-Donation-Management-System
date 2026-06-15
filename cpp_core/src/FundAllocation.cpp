#include "FundAllocation.h"
#include "Validation.h"

#include <sstream>

FundAllocation::FundAllocation()
    : id(""), beneficiaryId(""), campaignId(""), amount(0.0), date(""),
      purpose(""), approvedBy("") {}

FundAllocation::FundAllocation(const std::string& id, const std::string& beneficiaryId,
                               const std::string& campaignId, double amount,
                               const std::string& date, const std::string& purpose,
                               const std::string& approvedBy)
    : id(Validation::sanitizeField(id)),
      beneficiaryId(Validation::sanitizeField(beneficiaryId)),
      campaignId(Validation::sanitizeField(campaignId)),
      amount(amount),
      date(Validation::sanitizeField(date)),
      purpose(Validation::sanitizeField(purpose)),
      approvedBy(Validation::sanitizeField(approvedBy)) {}

std::string FundAllocation::getId() const { return id; }
std::string FundAllocation::getBeneficiaryId() const { return beneficiaryId; }
std::string FundAllocation::getCampaignId() const { return campaignId; }
double FundAllocation::getAmount() const { return amount; }
std::string FundAllocation::getDate() const { return date; }
std::string FundAllocation::getPurpose() const { return purpose; }
std::string FundAllocation::getApprovedBy() const { return approvedBy; }

void FundAllocation::setId(const std::string& id) { this->id = Validation::sanitizeField(id); }
void FundAllocation::setBeneficiaryId(const std::string& beneficiaryId) { this->beneficiaryId = Validation::sanitizeField(beneficiaryId); }
void FundAllocation::setCampaignId(const std::string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void FundAllocation::setAmount(double amount) { this->amount = amount; }
void FundAllocation::setDate(const std::string& date) { this->date = Validation::sanitizeField(date); }
void FundAllocation::setPurpose(const std::string& purpose) { this->purpose = Validation::sanitizeField(purpose); }
void FundAllocation::setApprovedBy(const std::string& approvedBy) { this->approvedBy = Validation::sanitizeField(approvedBy); }

std::string FundAllocation::displayInfo() const {
    std::ostringstream out;
    out << "Allocation => ID: " << id
        << ", Beneficiary: " << beneficiaryId
        << ", Campaign: " << campaignId
        << ", Amount: " << Validation::toFixed2(amount)
        << ", Date: " << date
        << ", Purpose: " << purpose
        << ", Approved By: " << approvedBy;
    return out.str();
}

std::string FundAllocation::serialize() const {
    std::ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(beneficiaryId) << '|'
        << Validation::sanitizeField(campaignId) << '|'
        << Validation::toFixed2(amount) << '|'
        << Validation::sanitizeField(date) << '|'
        << Validation::sanitizeField(purpose) << '|'
        << Validation::sanitizeField(approvedBy);
    return out.str();
}

FundAllocation FundAllocation::deserialize(const std::string& line) {
    std::vector<std::string> p = Validation::split(line);
    if (p.size() < 7) {
        return FundAllocation();
    }

    double parsedAmount = 0.0;
    Validation::safeToDouble(p[3], parsedAmount);

    return FundAllocation(p[0], p[1], p[2], parsedAmount, p[4], p[5], p[6]);
}

std::string FundAllocation::header() {
    return "id|beneficiaryId|campaignId|amount|date|purpose|approvedBy";
}

std::string FundAllocation::row() const {
    return serialize();
}
