#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <vector>

using namespace std;

class FileManager {
public:
    static string dataDirectory();
    static string pathFor(const string& fileName);
    static void ensureDataDirectory();
    static vector<string> readLines(const string& fileName);
    static bool writeLines(const string& fileName, const vector<string>& lines);
    static bool clearFile(const string& fileName);
};

#endif
