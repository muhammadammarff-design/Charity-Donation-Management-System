#ifndef REPORT_H
#define REPORT_H

#include <string>

class Report {
private:
    std::string id;
    std::string reportType;
    std::string period;
    double totalDonations;
    double totalAllocations;
    double remainingBalance;
    std::string generatedDate;
    std::string remarks;

public:
    Report();
    Report(const std::string& id, const std::string& reportType,
           const std::string& period, double totalDonations,
           double totalAllocations, double remainingBalance,
           const std::string& generatedDate, const std::string& remarks);
    Report(const std::string& period, double totalDonations,
           double totalAllocations);

    std::string getId() const;
    std::string getReportType() const;
    std::string getPeriod() const;
    double getTotalDonations() const;
    double getTotalAllocations() const;
    double getRemainingBalance() const;
    std::string getGeneratedDate() const;
    std::string getRemarks() const;

    void setId(const std::string& id);
    void setReportType(const std::string& reportType);
    void setPeriod(const std::string& period);
    void setTotalDonations(double totalDonations);
    void setTotalAllocations(double totalAllocations);
    void setRemainingBalance(double remainingBalance);
    void setGeneratedDate(const std::string& generatedDate);
    void setRemarks(const std::string& remarks);

    std::string displayInfo() const;
    std::string serialize() const;
    static Report deserialize(const std::string& line);
    static std::string header();
    std::string row() const;

    Report operator+(const Report& other) const;
    bool operator>(const Report& other) const;
};

#endif
