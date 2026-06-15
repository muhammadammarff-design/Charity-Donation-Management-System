#include "Beneficiary.h"
#include "Validation.h"

#include <sstream>

Beneficiary::Beneficiary()
    : Person(), familySize(0), needType(""), campaignId(""), totalReceived(0.0) {}

Beneficiary::Beneficiary(const std::string& id, const std::string& name, int age,
                         const std::string& contact, const std::string& address,
                         int familySize, const std::string& needType,
                         const std::string& campaignId, double totalReceived)
    : Person(id, name, age, contact, address),
      familySize(familySize),
      needType(Validation::sanitizeField(needType)),
      campaignId(Validation::sanitizeField(campaignId)),
      totalReceived(totalReceived) {}

Beneficiary::Beneficiary(const std::string& name, const std::string& needType,
                         const std::string& campaignId)
    : Person("", name, 0, "", ""),
      familySize(1),
      needType(Validation::sanitizeField(needType)),
      campaignId(Validation::sanitizeField(campaignId)),
      totalReceived(0.0) {}

int Beneficiary::getFamilySize() const { return familySize; }
std::string Beneficiary::getNeedType() const { return needType; }
std::string Beneficiary::getCampaignId() const { return campaignId; }
double Beneficiary::getTotalReceived() const { return totalReceived; }

void Beneficiary::setFamilySize(int familySize) { this->familySize = familySize; }
void Beneficiary::setNeedType(const std::string& needType) { this->needType = Validation::sanitizeField(needType); }
void Beneficiary::setCampaignId(const std::string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void Beneficiary::setTotalReceived(double totalReceived) { this->totalReceived = totalReceived; }
void Beneficiary::addReceivedAmount(double amount) { totalReceived += amount; }

std::string Beneficiary::displayInfo() const {
    std::ostringstream out;
    out << "Beneficiary => " << Person::displayInfo()
        << ", Family Size: " << familySize
        << ", Need: " << needType
        << ", Campaign: " << campaignId
        << ", Total Received: " << Validation::toFixed2(totalReceived);
    return out.str();
}

std::string Beneficiary::serialize() const {
    std::ostringstream out;
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

Beneficiary Beneficiary::deserialize(const std::string& line) {
    std::vector<std::string> p = Validation::split(line);
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

std::string Beneficiary::header() {
    return "id|name|age|contact|address|familySize|needType|campaignId|totalReceived";
}

std::string Beneficiary::row() const {
    return serialize();
}
