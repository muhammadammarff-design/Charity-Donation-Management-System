#include "Report.h"
#include "Validation.h"

#include <sstream>

Report::Report()
    : id(""), reportType("Monthly"), period(""), totalDonations(0.0),
      totalAllocations(0.0), remainingBalance(0.0), generatedDate(""), remarks("") {}

Report::Report(const std::string& id, const std::string& reportType,
               const std::string& period, double totalDonations,
               double totalAllocations, double remainingBalance,
               const std::string& generatedDate, const std::string& remarks)
    : id(Validation::sanitizeField(id)),
      reportType(Validation::sanitizeField(reportType)),
      period(Validation::sanitizeField(period)),
      totalDonations(totalDonations),
      totalAllocations(totalAllocations),
      remainingBalance(remainingBalance),
      generatedDate(Validation::sanitizeField(generatedDate)),
      remarks(Validation::sanitizeField(remarks)) {}

Report::Report(const std::string& period, double totalDonations,
               double totalAllocations)
    : id(""), reportType("Monthly"), period(Validation::sanitizeField(period)),
      totalDonations(totalDonations), totalAllocations(totalAllocations),
      remainingBalance(totalDonations - totalAllocations), generatedDate(""), remarks("") {}

std::string Report::getId() const { return id; }
std::string Report::getReportType() const { return reportType; }
std::string Report::getPeriod() const { return period; }
double Report::getTotalDonations() const { return totalDonations; }
double Report::getTotalAllocations() const { return totalAllocations; }
double Report::getRemainingBalance() const { return remainingBalance; }
std::string Report::getGeneratedDate() const { return generatedDate; }
std::string Report::getRemarks() const { return remarks; }

void Report::setId(const std::string& id) { this->id = Validation::sanitizeField(id); }
void Report::setReportType(const std::string& reportType) { this->reportType = Validation::sanitizeField(reportType); }
void Report::setPeriod(const std::string& period) { this->period = Validation::sanitizeField(period); }
void Report::setTotalDonations(double totalDonations) { this->totalDonations = totalDonations; }
void Report::setTotalAllocations(double totalAllocations) { this->totalAllocations = totalAllocations; }
void Report::setRemainingBalance(double remainingBalance) { this->remainingBalance = remainingBalance; }
void Report::setGeneratedDate(const std::string& generatedDate) { this->generatedDate = Validation::sanitizeField(generatedDate); }
void Report::setRemarks(const std::string& remarks) { this->remarks = Validation::sanitizeField(remarks); }

std::string Report::displayInfo() const {
    std::ostringstream out;
    out << "Report => ID: " << id
        << ", Type: " << reportType
        << ", Period: " << period
        << ", Donations: " << Validation::toFixed2(totalDonations)
        << ", Allocations: " << Validation::toFixed2(totalAllocations)
        << ", Remaining: " << Validation::toFixed2(remainingBalance)
        << ", Generated: " << generatedDate;
    return out.str();
}

std::string Report::serialize() const {
    std::ostringstream out;
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

Report Report::deserialize(const std::string& line) {
    std::vector<std::string> p = Validation::split(line);
    if (p.size() < 8) {
        return Report();
    }

    double donations = 0.0, allocations = 0.0, remaining = 0.0;
    Validation::safeToDouble(p[3], donations);
    Validation::safeToDouble(p[4], allocations);
    Validation::safeToDouble(p[5], remaining);

    return Report(p[0], p[1], p[2], donations, allocations, remaining, p[6], p[7]);
}

std::string Report::header() {
    return "id|reportType|period|totalDonations|totalAllocations|remainingBalance|generatedDate|remarks";
}

std::string Report::row() const {
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
