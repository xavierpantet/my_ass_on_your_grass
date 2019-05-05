/**
 * StringHelper.h
 * Helper methods for strings.
 * @author Xavier Pantet
 */
#include <string>
#include <vector>
#include <sstream>

#include "grass.h"

#ifndef ASS_ON_THE_GRASS_STRINGHELPER_H
#define ASS_ON_THE_GRASS_STRINGHELPER_H

using namespace std;

class StringHelper {
public:

    /**
     * Splits a strings according to a delimiter
     * @param str The string to be splitted
     * @param delimiter The delimiter
     * @return str splitted according to delimiter as a vector<string>
     */
    static vector<string> split(string str, char delimiter);

    /**
     * Makes a string out of a vector of strings
     * @param strs The strings we want to concatenate
     * @return The string representation of strs
     */
    static string stringify(vector<string> strs);
};

#endif //ASS_ON_THE_GRASS_STRINGHELPER_H
