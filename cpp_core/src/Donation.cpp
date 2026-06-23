#include "Donation.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Donation::Donation()
    : id(""), donorId(""), campaignId(""), amount(0.0), date(""),
      paymentMethod("Cash"), note("") {}

Donation::Donation(const string& id, const string& donorId,
                   const string& campaignId, double amount,
                   const string& date, const string& paymentMethod,
                   const string& note)
    : id(Validation::sanitizeField(id)),
      donorId(Validation::sanitizeField(donorId)),
      campaignId(Validation::sanitizeField(campaignId)),
      amount(amount),
      date(Validation::sanitizeField(date)),
      paymentMethod(Validation::sanitizeField(paymentMethod)),
      note(Validation::sanitizeField(note)) {}

Donation::Donation(double amount, const string& date)
    : id(""), donorId(""), campaignId(""), amount(amount),
      date(Validation::sanitizeField(date)), paymentMethod("Cash"), note("") {}

string Donation::getId() const { return id; }
string Donation::getDonorId() const { return donorId; }
string Donation::getCampaignId() const { return campaignId; }
double Donation::getAmount() const { return amount; }
string Donation::getDate() const { return date; }
string Donation::getPaymentMethod() const { return paymentMethod; }
string Donation::getNote() const { return note; }

void Donation::setId(const string& id) { this->id = Validation::sanitizeField(id); }
void Donation::setDonorId(const string& donorId) { this->donorId = Validation::sanitizeField(donorId); }
void Donation::setCampaignId(const string& campaignId) { this->campaignId = Validation::sanitizeField(campaignId); }
void Donation::setAmount(double amount) { this->amount = amount; }
void Donation::setDate(const string& date) { this->date = Validation::sanitizeField(date); }
void Donation::setPaymentMethod(const string& paymentMethod) { this->paymentMethod = Validation::sanitizeField(paymentMethod); }
void Donation::setNote(const string& note) { this->note = Validation::sanitizeField(note); }

string Donation::displayInfo() const {
    ostringstream out;
    out << "Donation => ID: " << id
        << ", Donor: " << donorId
        << ", Campaign: " << campaignId
        << ", Amount: " << Validation::toFixed2(amount)
        << ", Date: " << date
        << ", Method: " << paymentMethod;
    return out.str();
}

string Donation::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(donorId) << '|'
        << Validation::sanitizeField(campaignId) << '|'
        << Validation::toFixed2(amount) << '|'
        << Validation::sanitizeField(date) << '|'
        << Validation::sanitizeField(paymentMethod) << '|'
        << Validation::sanitizeField(note);
    return out.str();
}

Donation Donation::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 7) {
        return Donation();
    }

    double parsedAmount = 0.0;
    Validation::safeToDouble(p[3], parsedAmount);

    return Donation(p[0], p[1], p[2], parsedAmount, p[4], p[5], p[6]);
}

string Donation::header() {
    return "id|donorId|campaignId|amount|date|paymentMethod|note";
}

string Donation::row() const {
    return serialize();
}

double Donation::operator+(const Donation& other) const {
    return amount + other.amount;
}

bool Donation::operator>(const Donation& other) const {
    return amount > other.amount;
}
