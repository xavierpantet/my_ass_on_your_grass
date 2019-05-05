/**
 * Configuration.h
 * Reads and parses configuration file.
 * In particular, the config file contains the port used by the server, the base directory of the service and the list of users with their respective passwords.
 * @author: Xavier Pantet
 */
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>

#include "grass.h"
#include "FileReader.h"
#include "Commands.h"
#include "Exception.h"

#ifndef ASS_ON_THE_GRASS_CONFIGURATION_H
#define ASS_ON_THE_GRASS_CONFIGURATION_H

using namespace std;

class Configuration {
private:
    const string fileName;
    vector<string> getEntriesWithKey(const string key) const;
    string removeKeyInLine(string, string) const;
    string extractStringValue(string) const;
    bool set;

public:
    explicit Configuration(const string);

    /**
     * Get base directory from config file.
     * @return Base directory
     */
    string getBase() const;

    /**
     * Get server port from config file.
     * @return Server port
     */
    unsigned int getPort() const;

    /**
     * Get user-password associations from the config file.
     * @return User-password associations
     */
    map<string, string> getUsers() const;
};

#endif //ASS_ON_THE_GRASS_CONFIGURATION_H
