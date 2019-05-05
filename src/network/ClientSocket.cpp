/**
 * ClientSocket.cpp
 * The implementation of the ClientSocket.h file.
 * Manages all the stuff related to the client network.
 *
 * @author Alexandre Chambet
 */

using namespace std;

#include "../../include/ClientSocket.h"


ClientSocket::ClientSocket(string serverIP, unsigned int dstPort) : NetworkSocket(dstPort) {
    this->serverIP = serverIP;
}

void ClientSocket::initiateConnection() {
    // Common settings to create the socket
    this->commonInitiateConnection();

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, this->serverIP.c_str(), &(this->address).sin_addr) <= 0) {
        throw Exception(ERR_NETWORK_BAD_ADDRESS);
    }

    // Tries to connect to the server
    if (connect(this->sock, (struct sockaddr *) &(this->address), sizeof(this->address)) < 0) {
        throw Exception(ERR_NETWORK_CONNECTION_SERVER_FAILED);
    }
}

string ClientSocket::readCommand() {
    // Read the command of the user
    string command = "";

    cout << ">>> ";
    getline(cin, command);
    if (command == "") return str_nodata;
    else if (command == str_nodata) return "invalid";
    else return command;
}

void ClientSocket::sendToServer(string toSend) {
    this->sendTo(this->getSocket(), toSend);
}

string ClientSocket::readFromServer() {
    if (this->isSocketInitiated()) {
        char buffer[ClientSocket::SOCKET_BUFFER_SIZE] = {0};

        // Bytes read
        ssize_t bytes_read;

        // The stuff we read
        string r = "";

        do {
            bytes_read = recv(this->getSocket(), buffer, SOCKET_BUFFER_SIZE, 0);
            if (bytes_read > 0) {
                string line(buffer, bytes_read);
                r += line;
            }
        } while (bytes_read == SOCKET_BUFFER_SIZE);

        if (r == str_nodata) {
            return "";
        }

        return r;
    } else {
        throw Exception(ERR_NETWORK_SOCKET_NOT_CREATED);
    }
}

void ClientSocket::uploadFile(string filename) {
    try {
        FileReader fileReader(filename);

        // We first read the file
        vector <string> vecOfStr;
        fileReader.readFileVector(vecOfStr);

        // Then we send the lines 1 by 1
        vector<string>::iterator it;

        // The big string in which we'll store the file (yes it's not efficient I know)
        string big = "";

        for (it = vecOfStr.begin(); it != vecOfStr.end(); it++) {
            // Mandatory appending \n
            string toSend = *it + "\n";
            big += toSend;
        }

        this->sendToServer(big);

        cout << "File uploaded!" << endl;
    } catch (Exception &e) {
        e.print_error();
        cout << "The upload was stopped due to an error." << endl;
    }
}

void ClientSocket::downloadFile(string filename, unsigned int size) {
    // Create a file writer to write the file
    FileWriter fw(filename);

    // Clear the file in case of all data was there
    fw.clearFile();

    char buf[NetworkSocket::SOCKET_BUFFER_SIZE];

    // Bytes remaining to read
    ssize_t bytes_read;

    // The file
    string big = "";

    unsigned int sum = 0;

    do {
        bytes_read = recv(this->sock, buf, sizeof(buf), 0);
        sum += bytes_read;
        if (bytes_read > 0) {
            string line(buf, bytes_read);
            big += line;
        }
    } while (sum != size && bytes_read > 0);

    // Write it to the file
    fw.writeLine(big, true);

    cout << "File downloaded!" << endl;
}
