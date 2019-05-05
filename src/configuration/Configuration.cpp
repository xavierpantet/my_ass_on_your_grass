#include "../../include/Configuration.h"

using namespace std;

Configuration::Configuration(const string fileName): fileName(fileName) {
    set = false;
}

vector<string> Configuration::getEntriesWithKey(const string key) const {
    vector<string> lines;
    FileReader fr = FileReader(fileName);
    fr.readFileVector(lines);
    vector<string> desiredLines;
    for(auto const& line: lines) {
        if(line.rfind(key, 0) == 0 && line.size() > key.size() + 1 && line.find_first_of(" ") == key.size())
            desiredLines.push_back(line);
    }
    return desiredLines;
}

string Configuration::removeKeyInLine(string key, string line) const {
    if(line.find(" ", 0) == key.size() && line.size() > key.size() + 1) {
        return line.substr(key.size() + 1);
    }
    throw Exception(ERR_INVALID_ARGS);
}

string Configuration::extractStringValue(string key) const {
    vector<string> filtered = getEntriesWithKey(key);
    if(filtered.size() > 0) {
        return removeKeyInLine(key, filtered[0]);
    }
    throw Exception(ERR_INVALID_ARGS);
}

string Configuration::getBase() const {
    return extractStringValue("base");
}

unsigned int Configuration::getPort() const {
    string value = extractStringValue("port");
    return stoul(value, NULL, 10);
}

map<string, string> Configuration::getUsers() const {
    vector<string> filtered = getEntriesWithKey("user");
    map<string, string> users;
    for(auto const& u: filtered) {
        string userString = removeKeyInLine("user", u);
        size_t delimiterPos = userString.find(" ", 0);
        if(delimiterPos != string::npos && delimiterPos < userString.size() - 1) {
            users[userString.substr(0, delimiterPos)] = userString.substr(delimiterPos + 1);
        }
    }
    return users;
}
