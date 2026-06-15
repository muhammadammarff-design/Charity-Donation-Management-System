#include "Donation.h"
#include "Validation.h"

#include <sstream>

Donation::Donation()
    : id(""), donorId(""), campaignId(""), amount(0.0), date(""),
      paymentMethod("Cash"), note("") {}

Donation::Donation(const std::string& id, const std::string& donorId,
                   const std::string& campaignId, double amount,
                   const std::string& date, const std::string& paymentMethod,
                   const std::string& note)
    : id(Validation::sanitizeField(id)),
      donorId(Validation::sanitizeField(donorId)),
      campaignId(Validation::sanitizeField(campaignId)),
      amount(amount),
      date(Validation::sanitizeField(date)),
      paymentMethod(Validation::sanitizeField(paymentMethod)),
      note(Validation::sanitizeField(note)) {}

Donation::Donation(double amount, const std::string& date)
    : id(""), donorId(""), campaignId(""), amount(amount),
      date(Validation::sanitizeField(date)), paymentMethod("Cash"), note("") {}

std::string Donation::getId() const { return id; }
std::string Donation::getDonorId() const { return donorId; }
std::string Donation::getCampaignId() const { return campaignId; }
double Donation::getAmount() const { return amount; }
std::string Donation::getDate() const { return date; }
std::string Donation::getPaymentMethod() const { return paymentMethod; }
std::string Donation::getNote() const { return note; }

void Donation::setId(const std::string& id) { this->id = Validation::sanitizeField(id); }
void Donation::setDonorId(const std::string& donorId) { this->donorId = Validation::sanitizeField(donorId); }
void Donation::setCampaignId(const std::string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void Donation::setAmount(double amount) { this->amount = amount; }
void Donation::setDate(const std::string& date) { this->date = Validation::sanitizeField(date); }
void Donation::setPaymentMethod(const std::string& paymentMethod) { this->paymentMethod = Validation::sanitizeField(paymentMethod); }
void Donation::setNote(const std::string& note) { this->note = Validation::sanitizeField(note); }

std::string Donation::displayInfo() const {
    std::ostringstream out;
    out << "Donation => ID: " << id
        << ", Donor: " << donorId
        << ", Campaign: " << campaignId
        << ", Amount: " << Validation::toFixed2(amount)
        << ", Date: " << date
        << ", Method: " << paymentMethod;
    return out.str();
}

std::string Donation::serialize() const {
    std::ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(donorId) << '|'
        << Validation::sanitizeField(campaignId) << '|'
        << Validation::toFixed2(amount) << '|'
        << Validation::sanitizeField(date) << '|'
        << Validation::sanitizeField(paymentMethod) << '|'
        << Validation::sanitizeField(note);
    return out.str();
}

Donation Donation::deserialize(const std::string& line) {
    std::vector<std::string> p = Validation::split(line);
    if (p.size() < 7) {
        return Donation();
    }

    double parsedAmount = 0.0;
    Validation::safeToDouble(p[3], parsedAmount);

    return Donation(p[0], p[1], p[2], parsedAmount, p[4], p[5], p[6]);
}

std::string Donation::header() {
    return "id|donorId|campaignId|amount|date|paymentMethod|note";
}

std::string Donation::row() const {
    return serialize();
}

double Donation::operator+(const Donation& other) const {
    return amount + other.amount;
}

bool Donation::operator>(const Donation& other) const {
    return amount > other.amount;
}
