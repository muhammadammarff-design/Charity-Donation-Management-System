#include "FileManager.h"

#include <cstdlib>
#include <filesystem>
#include <fstream>

std::string FileManager::dataDirectory() {
    const char* envDir = std::getenv("CHARITY_DATA_DIR");
    if (envDir != nullptr && std::string(envDir).size() > 0) {
        return std::string(envDir);
    }
    return "data";
}

std::string FileManager::pathFor(const std::string& fileName) {
    std::filesystem::path dir(dataDirectory());
    return (dir / fileName).string();
}

void FileManager::ensureDataDirectory() {
    std::filesystem::create_directories(dataDirectory());
}

std::vector<std::string> FileManager::readLines(const std::string& fileName) {
    ensureDataDirectory();
    std::vector<std::string> lines;
    std::ifstream input(pathFor(fileName));

    if (!input.is_open()) {
        return lines;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    return lines;
}

bool FileManager::writeLines(const std::string& fileName, const std::vector<std::string>& lines) {
    ensureDataDirectory();
    std::ofstream output(pathFor(fileName), std::ios::trunc);

    if (!output.is_open()) {
        return false;
    }

    for (const std::string& line : lines) {
        output << line << '\n';
    }

    return true;
}

bool FileManager::clearFile(const std::string& fileName) {
    ensureDataDirectory();
    std::ofstream output(pathFor(fileName), std::ios::trunc);
    return output.is_open();
}
