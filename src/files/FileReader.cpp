/**
 * FileReader.cpp
 * Implementation of the FileReader.h file. Reads a file and returns it under different forms.
 *
 * @author Alexandre Chambet
 */

#include "../../include/FileReader.h"

using namespace std;

FileReader::FileReader(string filename) {
    // Append the basepath to the filename

    // Check if file exists
    std::ifstream ifile(filename.c_str());
    if (!(bool) ifile) {
        // File doesn't exist, we throw an exception
        throw Exception(ERR_FILE_NOT_FOUND);
    }

    this->filename = std::move(filename);
}

void FileReader::readFileVector(std::vector<std::string> &file) const {
    // Open the file
    std::ifstream in(this->filename);

    // Check if object is valid
    if (!in) {
        throw Exception(ERR_CANNOT_OPEN_FILE);
    }

    std::string str;

    // Read the next line from File until it reaches the end.
    while (std::getline(in, str)) {
        file.push_back(str);
    }

    //Close The File
    in.close();
}
