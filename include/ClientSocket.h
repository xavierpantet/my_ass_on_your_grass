/**
 * ClientSocket.h
 * Header file for ClientSocket.cpp
 *
 * @author Alexandre Chambet
 */

#ifndef ASS_ON_THE_GRASS_CLIENT_H
#define ASS_ON_THE_GRASS_CLIENT_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>

#include "grass.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "NetworkSocket.h"
#include "Exception.h"
#include "Commands.h"

using namespace std;

class ClientSocket : public NetworkSocket {
public:
    /**
     * Constructor.
     *
     * @param dstPort the port where the client should connect to.
     */
    explicit ClientSocket(string serverIP, unsigned int dstPort);

    /**
     * Read from the keyboard a command and returns it.
     *
     * @return string the command
     */
    string readCommand();

    /**
     * Sends a string to the server.
     *
     * @param toSend the string
     */
    void sendToServer(string toSend);

    /**
     * Creates the connection the server.
     */
    void initiateConnection();

    /**
     * Reads data from the server and returns it.
     *
     * @return string the data read
     */
    string readFromServer();

    static const string EXIT_CMD;

    /**
     * Uploads a file to the server.
     *
     * @param filename the file to be uploaded
     */
    void uploadFile(string filename);

    /**
     * Downloads a file from the server.
     *
     * @param filename the file to be downloaded
     * @param size the size of the file
     */
    void downloadFile(string filename, unsigned int size);

private:
    string serverIP;
};


#endif //ASS_ON_THE_GRASS_CLIENT_H
