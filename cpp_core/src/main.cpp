#include "CharitySystem.h"
#include "Validation.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <limits>
#include <set>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

void printHelp() {
    cout << "Charity & Donation Management System - C++ Core\n";
    cout << "Usage:\n";
    cout << "  charity_app                         Start interactive console menu\n";
    cout << "  charity_app seed                    Load demo data\n";
    cout << "  charity_app clear                   Clear all data files\n";
    cout << "  charity_app summary                 Show dashboard summary\n";
    cout << "  charity_app list donors|beneficiaries|campaigns|donations|allocations|reports\n";
    cout << "  charity_app add-donor name age contact email address donorType\n";
    cout << "  charity_app update-donor id name age contact email address donorType\n";
    cout << "  charity_app delete-donor id\n";
    cout << "  charity_app add-beneficiary name age contact address familySize needType campaignId\n";
    cout << "  charity_app update-beneficiary id name age contact address familySize needType campaignId\n";
    cout << "  charity_app delete-beneficiary id\n";
    cout << "  charity_app add-campaign title description target startDate endDate status\n";
    cout << "  charity_app update-campaign id title description target startDate endDate status\n";
    cout << "  charity_app delete-campaign id\n";
    cout << "  charity_app record-donation donorId campaignId amount date paymentMethod note\n";
    cout << "  charity_app allocate-funds beneficiaryId campaignId amount date purpose approvedBy\n";
    cout << "  charity_app generate-report monthly YYYY-MM\n";
    cout << "  charity_app generate-report campaign CAM001\n";
    cout << "  charity_app verify-donor donorId email\n";
    cout << "  charity_app donor-statement donorId email\n";
    cout << "  charity_app donor-donations donorId email\n";
    cout << "  charity_app donor-campaigns donorId email\n";
    cout << "  charity_app donor-allocations donorId email\n";
    cout << "  charity_app validation-demo          Run validation evidence cases\n";
    cout << "  charity_app oop-demo                 Show inheritance/polymorphism/operator overloading evidence\n";
}

void printResult(const OperationResult& result) {
    if (result.success) {
        cout << "SUCCESS|" << result.message;
        if (!result.id.empty()) cout << "|" << result.id;
        cout << endl;
    } else {
        cout << "ERROR|" << result.message << endl;
    }
}

bool parseIntArg(const string& text, int& value) {
    return Validation::safeToInt(text, value);
}

bool parseDoubleArg(const string& text, double& value) {
    return Validation::safeToDouble(text, value);
}

void printSummary(const CharitySystem& system) {
    SystemSummary s = system.summary();
    cout << "metric|value\n";
    cout << "donors|" << s.donorCount << "\n";
    cout << "beneficiaries|" << s.beneficiaryCount << "\n";
    cout << "campaigns|" << s.campaignCount << "\n";
    cout << "donations|" << s.donationCount << "\n";
    cout << "allocations|" << s.allocationCount << "\n";
    cout << "totalDonations|" << Validation::toFixed2(s.totalDonations) << "\n";
    cout << "totalAllocations|" << Validation::toFixed2(s.totalAllocations) << "\n";
    cout << "remainingBalance|" << Validation::toFixed2(s.remainingBalance) << "\n";
}

void listData(const CharitySystem& system, const string& type) {
    if (type == "donors") {
        cout << Donor::header() << "\n";
        for (const Donor& donor : system.getDonors()) cout << donor.row() << "\n";
    } else if (type == "beneficiaries") {
        cout << Beneficiary::header() << "\n";
        for (const Beneficiary& beneficiary : system.getBeneficiaries()) cout << beneficiary.row() << "\n";
    } else if (type == "campaigns") {
        cout << Campaign::header() << "\n";
        for (const Campaign& campaign : system.getCampaigns()) cout << campaign.row() << "\n";
    } else if (type == "donations") {
        cout << Donation::header() << "\n";
        for (const Donation& donation : system.getDonations()) cout << donation.row() << "\n";
    } else if (type == "allocations") {
        cout << FundAllocation::header() << "\n";
        for (const FundAllocation& allocation : system.getAllocations()) cout << allocation.row() << "\n";
    } else if (type == "reports") {
        cout << Report::header() << "\n";
        for (const Report& report : system.getReports()) cout << report.row() << "\n";
    } else {
        cout << "ERROR|Unknown list type.\n";
    }
}

string toLowerCopy(string value) {
    std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return value;
}

const Donor* findVerifiedDonor(const CharitySystem& system, const string& donorId, const string& email) {
    string cleanedEmail = toLowerCopy(Validation::trim(email));
    for (const Donor& donor : system.getDonors()) {
        if (donor.getId() == Validation::trim(donorId) &&
            toLowerCopy(donor.getEmail()) == cleanedEmail) {
            return &donor;
        }
    }
    return nullptr;
}

std::set<string> campaignsSupportedByDonor(const CharitySystem& system, const string& donorId) {
    std::set<string> campaignIds;
    for (const Donation& donation : system.getDonations()) {
        if (donation.getDonorId() == donorId) {
            campaignIds.insert(donation.getCampaignId());
        }
    }
    return campaignIds;
}

void printDonorStatement(const CharitySystem& system, const string& donorId, const string& email) {
    const Donor* donor = findVerifiedDonor(system, donorId, email);
    if (donor == nullptr) {
        cout << "ERROR|Donor verification failed. Please check donor ID and email.\n";
        return;
    }

    int donationCount = 0;
    double total = 0.0;
    for (const Donation& donation : system.getDonations()) {
        if (donation.getDonorId() == donor->getId()) {
            donationCount++;
            total += donation.getAmount();
        }
    }

    std::set<string> campaignIds = campaignsSupportedByDonor(system, donor->getId());
    cout << "metric|value\n";
    cout << "donorId|" << donor->getId() << "\n";
    cout << "donorName|" << donor->getName() << "\n";
    cout << "email|" << donor->getEmail() << "\n";
    cout << "donorType|" << donor->getDonorType() << "\n";
    cout << "totalDonated|" << Validation::toFixed2(total) << "\n";
    cout << "donationCount|" << donationCount << "\n";
    cout << "campaignsSupported|" << campaignIds.size() << "\n";
}

void printDonorDonations(const CharitySystem& system, const string& donorId, const string& email) {
    const Donor* donor = findVerifiedDonor(system, donorId, email);
    if (donor == nullptr) {
        cout << "ERROR|Donor verification failed. Please check donor ID and email.\n";
        return;
    }

    cout << Donation::header() << "\n";
    for (const Donation& donation : system.getDonations()) {
        if (donation.getDonorId() == donor->getId()) {
            cout << donation.row() << "\n";
        }
    }
}

void printDonorCampaigns(const CharitySystem& system, const string& donorId, const string& email) {
    const Donor* donor = findVerifiedDonor(system, donorId, email);
    if (donor == nullptr) {
        cout << "ERROR|Donor verification failed. Please check donor ID and email.\n";
        return;
    }

    std::set<string> campaignIds = campaignsSupportedByDonor(system, donor->getId());
    cout << Campaign::header() << "\n";
    for (const Campaign& campaign : system.getCampaigns()) {
        if (campaignIds.count(campaign.getId()) > 0) {
            cout << campaign.row() << "\n";
        }
    }
}

void printDonorAllocations(const CharitySystem& system, const string& donorId, const string& email) {
    const Donor* donor = findVerifiedDonor(system, donorId, email);
    if (donor == nullptr) {
        cout << "ERROR|Donor verification failed. Please check donor ID and email.\n";
        return;
    }

    std::set<string> campaignIds = campaignsSupportedByDonor(system, donor->getId());
    cout << FundAllocation::header() << "\n";
    for (const FundAllocation& allocation : system.getAllocations()) {
        if (campaignIds.count(allocation.getCampaignId()) > 0) {
            cout << allocation.row() << "\n";
        }
    }
}

void runValidationDemo(CharitySystem& system) {
    cout << "testCase|input|result\n";

    OperationResult r1 = system.addDonor("", 25, "+92 300 1234567", "valid@example.com", "Lahore", "Individual");
    cout << "Empty donor name|name=''|" << (r1.success ? "FAILED - accepted invalid input" : r1.message) << "\n";

    OperationResult r2 = system.addDonor("Invalid Age Donor", -5, "+92 300 1234567", "valid2@example.com", "Lahore", "Individual");
    cout << "Invalid donor age|age=-5|" << (r2.success ? "FAILED - accepted invalid input" : r2.message) << "\n";

    OperationResult r3 = system.addDonor("Invalid Email Donor", 30, "+92 300 1234567", "bad-email", "Lahore", "Individual");
    cout << "Invalid donor email|email='bad-email'|" << (r3.success ? "FAILED - accepted invalid input" : r3.message) << "\n";

    OperationResult r4 = system.addCampaign("Bad Campaign", "Negative target test", -1000, "2026-06-01", "2026-06-30", "Active");
    cout << "Negative campaign target|target=-1000|" << (r4.success ? "FAILED - accepted invalid input" : r4.message) << "\n";

    OperationResult r5 = system.addCampaign("Bad Date Campaign", "End date before start date", 1000, "2026-07-10", "2026-06-10", "Active");
    cout << "Invalid campaign date range|start=2026-07-10 end=2026-06-10|" << (r5.success ? "FAILED - accepted invalid input" : r5.message) << "\n";

    string donorId = system.getDonors().empty() ? "DNR999" : system.getDonors()[0].getId();
    string campaignId = system.getCampaigns().empty() ? "CAM999" : system.getCampaigns()[0].getId();
    OperationResult r6 = system.recordDonation(donorId, campaignId, -50, "2026-06-14", "Cash", "Invalid amount test");
    cout << "Negative donation amount|amount=-50|" << (r6.success ? "FAILED - accepted invalid input" : r6.message) << "\n";

    OperationResult r7 = system.recordDonation(donorId, campaignId, 500, "2026-99-99", "Cash", "Invalid date test");
    cout << "Invalid donation date|date=2026-99-99|" << (r7.success ? "FAILED - accepted invalid input" : r7.message) << "\n";

    if (!system.getBeneficiaries().empty()) {
        string beneficiaryId = system.getBeneficiaries()[0].getId();
        string linkedCampaignId = system.getBeneficiaries()[0].getCampaignId();
        OperationResult r8 = system.allocateFunds(beneficiaryId, linkedCampaignId, 999999999, "2026-06-15", "Over-allocation test", "Admin");
        cout << "Allocation greater than balance|amount=999999999|" << (r8.success ? "FAILED - accepted invalid input" : r8.message) << "\n";
    } else {
        cout << "Allocation greater than balance|requires beneficiary data|SKIPPED - seed demo data first\n";
    }
}

void runOopDemo(const CharitySystem& system) {
    cout << "concept|evidence\n";
    cout << "Inheritance|Donor and Beneficiary inherit from Person.\n";
    cout << "Polymorphism|displayInfo() is virtual in Person and overridden in Donor and Beneficiary.\n";

    if (!system.getDonors().empty()) {
        const Person* personPointer = &system.getDonors()[0];
        cout << "Runtime polymorphism|" << personPointer->displayInfo() << "\n";
    } else {
        cout << "Runtime polymorphism|Seed data first to show real Donor object through Person pointer.\n";
    }

    if (system.getDonors().size() >= 2) {
        double combined = system.getDonors()[0] + system.getDonors()[1];
        bool topCheck = system.getDonors()[0] > system.getDonors()[1];
        cout << "operator+ on Donor|Combined donation total = " << Validation::toFixed2(combined) << "\n";
        cout << "operator> on Donor|First donor greater than second = " << (topCheck ? "true" : "false") << "\n";
    } else {
        cout << "operator overloading on Donor|Need at least two donors.\n";
    }

    if (system.getDonations().size() >= 2) {
        double combinedDonations = system.getDonations()[0] + system.getDonations()[1];
        cout << "operator+ on Donation|Combined donation amount = " << Validation::toFixed2(combinedDonations) << "\n";
    } else {
        cout << "operator+ on Donation|Need at least two donations.\n";
    }

    if (system.getReports().size() >= 2) {
        Report merged = system.getReports()[0] + system.getReports()[1];
        cout << "operator+ on Report|" << merged.displayInfo() << "\n";
    } else {
        cout << "operator+ on Report|Need at least two reports.\n";
    }
}

string promptLine(const string& label) {
    cout << label;
    string value;
    std::getline(cin, value);
    return value;
}

int promptInt(const string& label) {
    while (true) {
        string value = promptLine(label);
        int parsed = 0;
        if (Validation::safeToInt(value, parsed)) return parsed;
        cout << "Invalid integer. Try again.\n";
    }
}

double promptDouble(const string& label) {
    while (true) {
        string value = promptLine(label);
        double parsed = 0.0;
        if (Validation::safeToDouble(value, parsed)) return parsed;
        cout << "Invalid amount. Try again.\n";
    }
}

void interactiveMenu() {
    CharitySystem system;
    while (true) {
        cout << "\n============================================\n";
        cout << " Charity & Donation Management System\n";
        cout << "============================================\n";
        cout << "1. Dashboard Summary\n";
        cout << "2. Add Donor\n";
        cout << "3. Add Campaign\n";
        cout << "4. Add Beneficiary\n";
        cout << "5. Record Donation\n";
        cout << "6. Allocate Funds\n";
        cout << "7. View Donors\n";
        cout << "8. View Campaigns\n";
        cout << "9. View Donations\n";
        cout << "10. Generate Monthly Report\n";
        cout << "11. Seed Demo Data\n";
        cout << "12. Validation Demo\n";
        cout << "0. Exit\n";
        cout << "Choose option: ";

        string choiceText;
        std::getline(cin, choiceText);
        int choice = -1;
        Validation::safeToInt(choiceText, choice);

        if (choice == 0) {
            cout << "Exiting system. Data has been saved in text files.\n";
            break;
        }

        OperationResult result;
        switch (choice) {
            case 1:
                printSummary(system);
                break;
            case 2: {
                string name = promptLine("Name: ");
                int age = promptInt("Age: ");
                string contact = promptLine("Contact: ");
                string email = promptLine("Email: ");
                string address = promptLine("Address: ");
                string donorType = promptLine("Donor Type (Individual/Organization): ");
                printResult(system.addDonor(name, age, contact, email, address, donorType));
                break;
            }
            case 3: {
                string title = promptLine("Title: ");
                string description = promptLine("Description: ");
                double target = promptDouble("Target Amount: ");
                string startDate = promptLine("Start Date (YYYY-MM-DD): ");
                string endDate = promptLine("End Date (YYYY-MM-DD): ");
                string status = promptLine("Status: ");
                printResult(system.addCampaign(title, description, target, startDate, endDate, status));
                break;
            }
            case 4: {
                string name = promptLine("Name: ");
                int age = promptInt("Age: ");
                string contact = promptLine("Contact: ");
                string address = promptLine("Address: ");
                int familySize = promptInt("Family Size: ");
                string needType = promptLine("Need Type: ");
                string campaignId = promptLine("Campaign ID: ");
                printResult(system.addBeneficiary(name, age, contact, address, familySize, needType, campaignId));
                break;
            }
            case 5: {
                string donorId = promptLine("Donor ID: ");
                string campaignId = promptLine("Campaign ID: ");
                double amount = promptDouble("Amount: ");
                string date = promptLine("Date (YYYY-MM-DD): ");
                string method = promptLine("Payment Method: ");
                string note = promptLine("Note: ");
                printResult(system.recordDonation(donorId, campaignId, amount, date, method, note));
                break;
            }
            case 6: {
                string beneficiaryId = promptLine("Beneficiary ID: ");
                string campaignId = promptLine("Campaign ID: ");
                double amount = promptDouble("Amount: ");
                string date = promptLine("Date (YYYY-MM-DD): ");
                string purpose = promptLine("Purpose: ");
                string approvedBy = promptLine("Approved By: ");
                printResult(system.allocateFunds(beneficiaryId, campaignId, amount, date, purpose, approvedBy));
                break;
            }
            case 7:
                listData(system, "donors");
                break;
            case 8:
                listData(system, "campaigns");
                break;
            case 9:
                listData(system, "donations");
                break;
            case 10: {
                string month = promptLine("Month (YYYY-MM): ");
                printResult(system.generateMonthlyReport(month));
                break;
            }
            case 11:
                system.seedDemoData();
                cout << "Demo data loaded successfully.\n";
                break;
            case 12:
                runValidationDemo(system);
                break;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        interactiveMenu();
        return 0;
    }

    string command = argv[1];
    if (command == "help" || command == "--help" || command == "-h") {
        printHelp();
        return 0;
    }

    CharitySystem system;

    if (command == "seed") {
        system.seedDemoData();
        cout << "SUCCESS|Demo data loaded successfully.\n";
    } else if (command == "clear") {
        system.clearAll();
        cout << "SUCCESS|All data cleared successfully.\n";
    } else if (command == "summary") {
        printSummary(system);
    } else if (command == "list") {
        if (argc < 3) { cout << "ERROR|Missing list type.\n"; return 1; }
        listData(system, argv[2]);
    } else if (command == "add-donor") {
        if (argc < 8) { cout << "ERROR|add-donor requires 6 arguments.\n"; return 1; }
        int age = 0;
        if (!parseIntArg(argv[3], age)) { cout << "ERROR|Age must be an integer.\n"; return 1; }
        printResult(system.addDonor(argv[2], age, argv[4], argv[5], argv[6], argv[7]));
    } else if (command == "update-donor") {
        if (argc < 9) { cout << "ERROR|update-donor requires 7 arguments.\n"; return 1; }
        int age = 0;
        if (!parseIntArg(argv[4], age)) { cout << "ERROR|Age must be an integer.\n"; return 1; }
        printResult(system.updateDonor(argv[2], argv[3], age, argv[5], argv[6], argv[7], argv[8]));
    } else if (command == "delete-donor") {
        if (argc < 3) { cout << "ERROR|delete-donor requires donor ID.\n"; return 1; }
        printResult(system.deleteDonor(argv[2]));
    } else if (command == "add-beneficiary") {
        if (argc < 9) { cout << "ERROR|add-beneficiary requires 7 arguments.\n"; return 1; }
        int age = 0, familySize = 0;
        if (!parseIntArg(argv[3], age)) { cout << "ERROR|Age must be an integer.\n"; return 1; }
        if (!parseIntArg(argv[6], familySize)) { cout << "ERROR|Family size must be an integer.\n"; return 1; }
        printResult(system.addBeneficiary(argv[2], age, argv[4], argv[5], familySize, argv[7], argv[8]));
    } else if (command == "update-beneficiary") {
        if (argc < 10) { cout << "ERROR|update-beneficiary requires 8 arguments.\n"; return 1; }
        int age = 0, familySize = 0;
        if (!parseIntArg(argv[4], age)) { cout << "ERROR|Age must be an integer.\n"; return 1; }
        if (!parseIntArg(argv[7], familySize)) { cout << "ERROR|Family size must be an integer.\n"; return 1; }
        printResult(system.updateBeneficiary(argv[2], argv[3], age, argv[5], argv[6], familySize, argv[8], argv[9]));
    } else if (command == "delete-beneficiary") {
        if (argc < 3) { cout << "ERROR|delete-beneficiary requires beneficiary ID.\n"; return 1; }
        printResult(system.deleteBeneficiary(argv[2]));
    } else if (command == "add-campaign") {
        if (argc < 8) { cout << "ERROR|add-campaign requires 6 arguments.\n"; return 1; }
        double target = 0.0;
        if (!parseDoubleArg(argv[4], target)) { cout << "ERROR|Target amount must be numeric.\n"; return 1; }
        printResult(system.addCampaign(argv[2], argv[3], target, argv[5], argv[6], argv[7]));
    } else if (command == "update-campaign") {
        if (argc < 9) { cout << "ERROR|update-campaign requires 7 arguments.\n"; return 1; }
        double target = 0.0;
        if (!parseDoubleArg(argv[5], target)) { cout << "ERROR|Target amount must be numeric.\n"; return 1; }
        printResult(system.updateCampaign(argv[2], argv[3], argv[4], target, argv[6], argv[7], argv[8]));
    } else if (command == "delete-campaign") {
        if (argc < 3) { cout << "ERROR|delete-campaign requires campaign ID.\n"; return 1; }
        printResult(system.deleteCampaign(argv[2]));
    } else if (command == "record-donation") {
        if (argc < 8) { cout << "ERROR|record-donation requires 6 arguments.\n"; return 1; }
        double amount = 0.0;
        if (!parseDoubleArg(argv[4], amount)) { cout << "ERROR|Donation amount must be numeric.\n"; return 1; }
        printResult(system.recordDonation(argv[2], argv[3], amount, argv[5], argv[6], argv[7]));
    } else if (command == "allocate-funds") {
        if (argc < 8) { cout << "ERROR|allocate-funds requires 6 arguments.\n"; return 1; }
        double amount = 0.0;
        if (!parseDoubleArg(argv[4], amount)) { cout << "ERROR|Allocation amount must be numeric.\n"; return 1; }
        printResult(system.allocateFunds(argv[2], argv[3], amount, argv[5], argv[6], argv[7]));
    } else if (command == "generate-report") {
        if (argc < 4) { cout << "ERROR|generate-report requires type and value.\n"; return 1; }
        string type = argv[2];
        if (type == "monthly") {
            printResult(system.generateMonthlyReport(argv[3]));
        } else if (type == "campaign") {
            printResult(system.generateCampaignReport(argv[3]));
        } else {
            cout << "ERROR|Unknown report type. Use monthly or campaign.\n";
            return 1;
        }
    } else if (command == "verify-donor") {
        if (argc < 4) { cout << "ERROR|verify-donor requires donor ID and email.\n"; return 1; }
        const Donor* donor = findVerifiedDonor(system, argv[2], argv[3]);
        if (donor == nullptr) {
            cout << "ERROR|Donor verification failed. Please check donor ID and email.\n";
            return 1;
        }
        cout << "SUCCESS|Donor verified successfully.|" << donor->getId() << "\n";
    } else if (command == "donor-statement") {
        if (argc < 4) { cout << "ERROR|donor-statement requires donor ID and email.\n"; return 1; }
        printDonorStatement(system, argv[2], argv[3]);
    } else if (command == "donor-donations") {
        if (argc < 4) { cout << "ERROR|donor-donations requires donor ID and email.\n"; return 1; }
        printDonorDonations(system, argv[2], argv[3]);
    } else if (command == "donor-campaigns") {
        if (argc < 4) { cout << "ERROR|donor-campaigns requires donor ID and email.\n"; return 1; }
        printDonorCampaigns(system, argv[2], argv[3]);
    } else if (command == "donor-allocations") {
        if (argc < 4) { cout << "ERROR|donor-allocations requires donor ID and email.\n"; return 1; }
        printDonorAllocations(system, argv[2], argv[3]);
    } else if (command == "validation-demo") {
        runValidationDemo(system);
    } else if (command == "oop-demo") {
        runOopDemo(system);
    } else {
        cout << "ERROR|Unknown command. Use help for available commands.\n";
        return 1;
    }

    return 0;
}
