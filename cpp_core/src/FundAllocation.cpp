#include "FundAllocation.h"
#include "Validation.h"

#include <sstream>

using namespace std;
FundAllocation::FundAllocation()
    : id(""), beneficiaryId(""), campaignId(""), amount(0.0), date(""),
      purpose(""), approvedBy("") {}

FundAllocation::FundAllocation(const string& id, const string& beneficiaryId,
                               const string& campaignId, double amount,
                               const string& date, const string& purpose,
                               const string& approvedBy)
    : id(Validation::sanitizeField(id)),
      beneficiaryId(Validation::sanitizeField(beneficiaryId)),
      campaignId(Validation::sanitizeField(campaignId)),
      amount(amount),
      date(Validation::sanitizeField(date)),
      purpose(Validation::sanitizeField(purpose)),
      approvedBy(Validation::sanitizeField(approvedBy)) {}

string FundAllocation::getId() const { return id; }
string FundAllocation::getBeneficiaryId() const { return beneficiaryId; }
string FundAllocation::getCampaignId() const { return campaignId; }
double FundAllocation::getAmount() const { return amount; }
string FundAllocation::getDate() const { return date; }
string FundAllocation::getPurpose() const { return purpose; }
string FundAllocation::getApprovedBy() const { return approvedBy; }

void FundAllocation::setId(const string& id) { this->id = Validation::sanitizeField(id); }
void FundAllocation::setBeneficiaryId(const string& beneficiaryId) { this->beneficiaryId = Validation::sanitizeField(beneficiaryId); }
void FundAllocation::setCampaignId(const string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void FundAllocation::setAmount(double amount) { this->amount = amount; }
void FundAllocation::setDate(const string& date) { this->date = Validation::sanitizeField(date); }
void FundAllocation::setPurpose(const string& purpose) { this->purpose = Validation::sanitizeField(purpose); }
void FundAllocation::setApprovedBy(const string& approvedBy) { this->approvedBy = Validation::sanitizeField(approvedBy); }

string FundAllocation::displayInfo() const {
    ostringstream out;
    out << "Allocation => ID: " << id
        << ", Beneficiary: " << beneficiaryId
        << ", Campaign: " << campaignId
        << ", Amount: " << Validation::toFixed2(amount)
        << ", Date: " << date
        << ", Purpose: " << purpose
        << ", Approved By: " << approvedBy;
    return out.str();
}

string FundAllocation::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(beneficiaryId) << '|'
        << Validation::sanitizeField(campaignId) << '|'
        << Validation::toFixed2(amount) << '|'
        << Validation::sanitizeField(date) << '|'
        << Validation::sanitizeField(purpose) << '|'
        << Validation::sanitizeField(approvedBy);
    return out.str();
}

FundAllocation FundAllocation::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 7) {
        return FundAllocation();
    }

    double parsedAmount = 0.0;
    Validation::safeToDouble(p[3], parsedAmount);

    return FundAllocation(p[0], p[1], p[2], parsedAmount, p[4], p[5], p[6]);
}

string FundAllocation::header() {
    return "id|beneficiaryId|campaignId|amount|date|purpose|approvedBy";
}

string FundAllocation::row() const {
    return serialize();
}
