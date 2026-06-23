#include "Beneficiary.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Beneficiary::Beneficiary()
    : Person(), familySize(0), needType(""), campaignId(""), totalReceived(0.0) {}

Beneficiary::Beneficiary(const string& id, const string& name, int age,
                         const string& contact, const string& address,
                         int familySize, const string& needType,
                         const string& campaignId, double totalReceived)
    : Person(id, name, age, contact, address),
      familySize(familySize),
      needType(Validation::sanitizeField(needType)),
      campaignId(Validation::sanitizeField(campaignId)),
      totalReceived(totalReceived) {}

Beneficiary::Beneficiary(const string& name, const string& needType,
                         const string& campaignId)
    : Person("", name, 0, "", ""),
      familySize(1),
      needType(Validation::sanitizeField(needType)),
      campaignId(Validation::sanitizeField(campaignId)),
      totalReceived(0.0) {}

int Beneficiary::getFamilySize() const { return familySize; }
string Beneficiary::getNeedType() const { return needType; }
string Beneficiary::getCampaignId() const { return campaignId; }
double Beneficiary::getTotalReceived() const { return totalReceived; }

void Beneficiary::setFamilySize(int familySize) { this->familySize = familySize; }
void Beneficiary::setNeedType(const string& needType) { this->needType = Validation::sanitizeField(needType); }
void Beneficiary::setCampaignId(const string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void Beneficiary::setTotalReceived(double totalReceived) { this->totalReceived = totalReceived; }
void Beneficiary::addReceivedAmount(double amount) { totalReceived += amount; }

string Beneficiary::displayInfo() const {
    ostringstream out;
    out << "Beneficiary => " << Person::displayInfo()
        << ", Family Size: " << familySize
        << ", Need: " << needType
        << ", Campaign: " << campaignId
        << ", Total Received: " << Validation::toFixed2(totalReceived);
    return out.str();
}

string Beneficiary::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(name) << '|'
        << age << '|'
        << Validation::sanitizeField(contact) << '|'
        << Validation::sanitizeField(address) << '|'
        << familySize << '|'
        << Validation::sanitizeField(needType) << '|'
        << Validation::sanitizeField(campaignId) << '|'
        << Validation::toFixed2(totalReceived);
    return out.str();
}

Beneficiary Beneficiary::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 9) {
        return Beneficiary();
    }

    int parsedAge = 0;
    int parsedFamilySize = 0;
    double parsedTotal = 0.0;
    Validation::safeToInt(p[2], parsedAge);
    Validation::safeToInt(p[5], parsedFamilySize);
    Validation::safeToDouble(p[8], parsedTotal);

    return Beneficiary(p[0], p[1], parsedAge, p[3], p[4], parsedFamilySize, p[6], p[7], parsedTotal);
}

string Beneficiary::header() {
    return "id|name|age|contact|address|familySize|needType|campaignId|totalReceived";
}

string Beneficiary::row() const {
    return serialize();
}
