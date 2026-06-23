#include "Campaign.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Campaign::Campaign()
    : id(""), title(""), description(""), targetAmount(0.0),
      collectedAmount(0.0), allocatedAmount(0.0), startDate(""), endDate(""),
      status("Active") {}

Campaign::Campaign(const string& id, const string& title,
                   const string& description, double targetAmount,
                   double collectedAmount, double allocatedAmount,
                   const string& startDate, const string& endDate,
                   const string& status)
    : id(Validation::sanitizeField(id)),
      title(Validation::sanitizeField(title)),
      description(Validation::sanitizeField(description)),
      targetAmount(targetAmount),
      collectedAmount(collectedAmount),
      allocatedAmount(allocatedAmount),
      startDate(Validation::sanitizeField(startDate)),
      endDate(Validation::sanitizeField(endDate)),
      status(Validation::sanitizeField(status)) {}

Campaign::Campaign(const string& title, double targetAmount)
    : id(""), title(Validation::sanitizeField(title)), description(""),
      targetAmount(targetAmount), collectedAmount(0.0), allocatedAmount(0.0),
      startDate(""), endDate(""), status("Active") {}

string Campaign::getId() const { return id; }
string Campaign::getTitle() const { return title; }
string Campaign::getDescription() const { return description; }
double Campaign::getTargetAmount() const { return targetAmount; }
double Campaign::getCollectedAmount() const { return collectedAmount; }
double Campaign::getAllocatedAmount() const { return allocatedAmount; }
string Campaign::getStartDate() const { return startDate; }
string Campaign::getEndDate() const { return endDate; }
string Campaign::getStatus() const { return status; }

void Campaign::setId(const string& id) { this->id = Validation::sanitizeField(id); }
void Campaign::setTitle(const string& title) { this->title = Validation::sanitizeField(title); }
void Campaign::setDescription(const string& description) { this->description = Validation::sanitizeField(description); }
void Campaign::setTargetAmount(double targetAmount) { this->targetAmount = targetAmount; }
void Campaign::setCollectedAmount(double collectedAmount) { this->collectedAmount = collectedAmount; }
void Campaign::setAllocatedAmount(double allocatedAmount) { this->allocatedAmount = allocatedAmount; }
void Campaign::setStartDate(const string& startDate) { this->startDate = Validation::sanitizeField(startDate); }
void Campaign::setEndDate(const string& endDate) { this->endDate = Validation::sanitizeField(endDate); }
void Campaign::setStatus(const string& status) { this->status = Validation::sanitizeField(status); }
void Campaign::addCollectedAmount(double amount) { collectedAmount += amount; }
void Campaign::addAllocatedAmount(double amount) { allocatedAmount += amount; }

double Campaign::getAvailableBalance() const {
    return collectedAmount - allocatedAmount;
}

double Campaign::getProgressPercent() const {
    if (targetAmount <= 0.0) {
        return 0.0;
    }
    return (collectedAmount / targetAmount) * 100.0;
}

string Campaign::displayInfo() const {
    ostringstream out;
    out << "Campaign => ID: " << id
        << ", Title: " << title
        << ", Target: " << Validation::toFixed2(targetAmount)
        << ", Collected: " << Validation::toFixed2(collectedAmount)
        << ", Allocated: " << Validation::toFixed2(allocatedAmount)
        << ", Available: " << Validation::toFixed2(getAvailableBalance())
        << ", Progress: " << Validation::toFixed2(getProgressPercent()) << "%"
        << ", Status: " << status;
    return out.str();
}

string Campaign::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(title) << '|'
        << Validation::sanitizeField(description) << '|'
        << Validation::toFixed2(targetAmount) << '|'
        << Validation::toFixed2(collectedAmount) << '|'
        << Validation::toFixed2(allocatedAmount) << '|'
        << Validation::sanitizeField(startDate) << '|'
        << Validation::sanitizeField(endDate) << '|'
        << Validation::sanitizeField(status);
    return out.str();
}

Campaign Campaign::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 9) {
        return Campaign();
    }

    double target = 0.0, collected = 0.0, allocated = 0.0;
    Validation::safeToDouble(p[3], target);
    Validation::safeToDouble(p[4], collected);
    Validation::safeToDouble(p[5], allocated);

    return Campaign(p[0], p[1], p[2], target, collected, allocated, p[6], p[7], p[8]);
}

string Campaign::header() {
    return "id|title|description|targetAmount|collectedAmount|allocatedAmount|availableBalance|progressPercent|startDate|endDate|status";
}

string Campaign::row() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(title) << '|'
        << Validation::sanitizeField(description) << '|'
        << Validation::toFixed2(targetAmount) << '|'
        << Validation::toFixed2(collectedAmount) << '|'
        << Validation::toFixed2(allocatedAmount) << '|'
        << Validation::toFixed2(getAvailableBalance()) << '|'
        << Validation::toFixed2(getProgressPercent()) << '|'
        << Validation::sanitizeField(startDate) << '|'
        << Validation::sanitizeField(endDate) << '|'
        << Validation::sanitizeField(status);
    return out.str();
}

double Campaign::operator+(const Campaign& other) const {
    return collectedAmount + other.collectedAmount;
}

bool Campaign::operator<(const Campaign& other) const {
    return collectedAmount < other.collectedAmount;
}

bool Campaign::operator<(double amount) const {
    return collectedAmount < amount;
}
