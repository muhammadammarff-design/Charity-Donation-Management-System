#include "Report.h"
#include "Validation.h"

#include <sstream>

using namespace std;
Report::Report()
    : id(""), reportType("Monthly"), period(""), totalDonations(0.0),
      totalAllocations(0.0), remainingBalance(0.0), generatedDate(""), remarks("") {}

Report::Report(const string& id, const string& reportType,
               const string& period, double totalDonations,
               double totalAllocations, double remainingBalance,
               const string& generatedDate, const string& remarks)
    : id(Validation::sanitizeField(id)),
      reportType(Validation::sanitizeField(reportType)),
      period(Validation::sanitizeField(period)),
      totalDonations(totalDonations),
      totalAllocations(totalAllocations),
      remainingBalance(remainingBalance),
      generatedDate(Validation::sanitizeField(generatedDate)),
      remarks(Validation::sanitizeField(remarks)) {}

Report::Report(const string& period, double totalDonations,
               double totalAllocations)
    : id(""), reportType("Monthly"), period(Validation::sanitizeField(period)),
      totalDonations(totalDonations), totalAllocations(totalAllocations),
      remainingBalance(totalDonations - totalAllocations), generatedDate(""), remarks("") {}

string Report::getId() const { return id; }
string Report::getReportType() const { return reportType; }
string Report::getPeriod() const { return period; }
double Report::getTotalDonations() const { return totalDonations; }
double Report::getTotalAllocations() const { return totalAllocations; }
double Report::getRemainingBalance() const { return remainingBalance; }
string Report::getGeneratedDate() const { return generatedDate; }
string Report::getRemarks() const { return remarks; }

void Report::setId(const string& id) { this->id = Validation::sanitizeField(id); }
void Report::setReportType(const string& reportType) { this->reportType = Validation::sanitizeField(reportType); }
void Report::setPeriod(const string& period) { this->period = Validation::sanitizeField(period); }
void Report::setTotalDonations(double totalDonations) { this->totalDonations = totalDonations; }
void Report::setTotalAllocations(double totalAllocations) { this->totalAllocations = totalAllocations; }
void Report::setRemainingBalance(double remainingBalance) { this->remainingBalance = remainingBalance; }
void Report::setGeneratedDate(const string& generatedDate) { this->generatedDate = Validation::sanitizeField(generatedDate); }
void Report::setRemarks(const string& remarks) { this->remarks = Validation::sanitizeField(remarks); }

string Report::displayInfo() const {
    ostringstream out;
    out << "Report => ID: " << id
        << ", Type: " << reportType
        << ", Period: " << period
        << ", Donations: " << Validation::toFixed2(totalDonations)
        << ", Allocations: " << Validation::toFixed2(totalAllocations)
        << ", Remaining: " << Validation::toFixed2(remainingBalance)
        << ", Generated: " << generatedDate;
    return out.str();
}

string Report::serialize() const {
    ostringstream out;
    out << Validation::sanitizeField(id) << '|'
        << Validation::sanitizeField(reportType) << '|'
        << Validation::sanitizeField(period) << '|'
        << Validation::toFixed2(totalDonations) << '|'
        << Validation::toFixed2(totalAllocations) << '|'
        << Validation::toFixed2(remainingBalance) << '|'
        << Validation::sanitizeField(generatedDate) << '|'
        << Validation::sanitizeField(remarks);
    return out.str();
}

Report Report::deserialize(const string& line) {
    vector<string> p = Validation::split(line);
    if (p.size() < 8) {
        return Report();
    }

    double donations = 0.0, allocations = 0.0, remaining = 0.0;
    Validation::safeToDouble(p[3], donations);
    Validation::safeToDouble(p[4], allocations);
    Validation::safeToDouble(p[5], remaining);

    return Report(p[0], p[1], p[2], donations, allocations, remaining, p[6], p[7]);
}

string Report::header() {
    return "id|reportType|period|totalDonations|totalAllocations|remainingBalance|generatedDate|remarks";
}

string Report::row() const {
    return serialize();
}

Report Report::operator+(const Report& other) const {
    Report combined;
    combined.setId(id + "+" + other.id);
    combined.setReportType("Combined");
    combined.setPeriod(period + " + " + other.period);
    combined.setTotalDonations(totalDonations + other.totalDonations);
    combined.setTotalAllocations(totalAllocations + other.totalAllocations);
    combined.setRemainingBalance(remainingBalance + other.remainingBalance);
    combined.setGeneratedDate(generatedDate);
    combined.setRemarks("Merged using Report operator+.");
    return combined;
}

bool Report::operator>(const Report& other) const {
    return totalDonations > other.totalDonations;
}
