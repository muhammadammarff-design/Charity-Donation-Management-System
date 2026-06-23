#include "FileManager.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

using namespace std;

string FileManager::dataDirectory() {
    const char* envDir = getenv("CHARITY_DATA_DIR");
    if (envDir != nullptr && string(envDir).size() > 0) {
        return string(envDir);
    }
    return "data";
}

string FileManager::pathFor(const string& fileName) {
    filesystem::path dir(dataDirectory());
    return (dir / fileName).string();
}

void FileManager::ensureDataDirectory() {
    filesystem::create_directories(dataDirectory());
}

vector<string> FileManager::readLines(const string& fileName) {
    ensureDataDirectory();
    vector<string> lines;
    ifstream input(pathFor(fileName));

    if (!input.is_open()) {
        return lines;
    }

    string line;
    while (getline(input, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    return lines;
}

bool FileManager::writeLines(const string& fileName, const vector<string>& lines) {
    ensureDataDirectory();
    ofstream output(pathFor(fileName), ios::trunc);

    if (!output.is_open()) {
        return false;
    }

    for (const string& line : lines) {
        output << line << '\n';
    }

    return true;
}

bool FileManager::clearFile(const string& fileName) {
    ensureDataDirectory();
    ofstream output(pathFor(fileName), ios::trunc);
    return output.is_open();
}
