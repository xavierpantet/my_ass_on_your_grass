/**
 * File.cpp
 * Implementation of the File.h file. Super class providing some tools method for FileReader and FileWriter classes.
 *
 * @author Alexandre Chambet
 */

#include "../../include/File.h"

using namespace std;

size_t File::fileSize() const {
    // Open the file
    ifstream file( filename, ios::binary | ios::ate);
    return (size_t) file.tellg();
}
