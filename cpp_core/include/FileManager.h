#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

class FileManager {
public:
    static std::string dataDirectory();
    static std::string pathFor(const std::string& fileName);
    static void ensureDataDirectory();
    static std::vector<std::string> readLines(const std::string& fileName);
    static bool writeLines(const std::string& fileName, const std::vector<std::string>& lines);
    static bool clearFile(const std::string& fileName);
};

#endif
