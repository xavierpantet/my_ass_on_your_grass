#include "../../include/StringHelper.h"

using namespace std;

vector<string> StringHelper::split(string str, char delimiter) {
    stringstream ss(str);
    vector<string> strs;
    string tmp;
    while(getline(ss, tmp, delimiter)){
        strs.push_back(tmp);
    }
    return strs;
}

string StringHelper::stringify(vector<string> strs) {
    stringstream str;
    for(const auto& line: strs)
        str << line;
    return str.str();
}
