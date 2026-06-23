#ifndef REPORT_H
#define REPORT_H

#include <string>

using namespace std;

class Report {
private:
    string id;
    string reportType;
    string period;
    double totalDonations;
    double totalAllocations;
    double remainingBalance;
    string generatedDate;
    string remarks;

public:
    Report();
    Report(const string& id, const string& reportType,
           const string& period, double totalDonations,
           double totalAllocations, double remainingBalance,
           const string& generatedDate, const string& remarks);
    Report(const string& period, double totalDonations,
           double totalAllocations);

    string getId() const;
    string getReportType() const;
    string getPeriod() const;
    double getTotalDonations() const;
    double getTotalAllocations() const;
    double getRemainingBalance() const;
    string getGeneratedDate() const;
    string getRemarks() const;

    void setId(const string& id);
    void setReportType(const string& reportType);
    void setPeriod(const string& period);
    void setTotalDonations(double totalDonations);
    void setTotalAllocations(double totalAllocations);
    void setRemainingBalance(double remainingBalance);
    void setGeneratedDate(const string& generatedDate);
    void setRemarks(const string& remarks);

    string displayInfo() const;
    string serialize() const;
    static Report deserialize(const string& line);
    static string header();
    string row() const;

    Report operator+(const Report& other) const;
    bool operator>(const Report& other) const;
};

#endif
