/**
 * FileReader.h
 * Header file for FileReader.cpp.
 *
 * @author Alexandre Chambet
 */

#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <iterator>

#include "grass.h"
#include "File.h"
#include "Exception.h"

#ifndef ASS_ON_THE_GRASS_FILEREADER_H
#define ASS_ON_THE_GRASS_FILEREADER_H

using namespace std;

class FileReader: public File {
public:
    /**
     * Constructor for the class.
     *
     * @param filename the name of the file to be read
     */
    explicit FileReader(string filename);

    /**
     * Reads a file and stores its content into buffer.
     *
     * @param buffer where the content will be stored.
     */
    void readFile(char * buffer) const;

    /**
     * Reads a file and stores its content into a vector of string.
     *
     * @param file, the vector where the content will be stored.
     */
    void readFileVector(vector<string> & file) const;
};


#endif //ASS_ON_THE_GRASS_FILEREADER_H
