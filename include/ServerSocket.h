/**
 * ServerSocket.h
 * Header file for ServerSocket.cpp
 *
 * @author Alexandre Chambet
 */

#ifndef ASS_ON_THE_GRASS_SERVER_H
#define ASS_ON_THE_GRASS_SERVER_H

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>

#include "grass.h"
#include "FileWriter.h"
#include "FileReader.h"
#include "NetworkSocket.h"
#include "Exception.h"
#include "Commands.h"
#include "Configuration.h"

using namespace std;

class ServerSocket : public NetworkSocket {
public:
    /**
     * Constructor for the object.
     *
     * @param port to listen on
     */
    explicit ServerSocket(unsigned int port);

    /**
     * Creates the socket that will be listening in the port passed as argument while constructing the object.
     */
    void initiateConnection();

    /**
     * Reads the commands sent by the user and answers accordingly to the protocol.
     *
     * @param userSocket the socket to read the commands from
     * @param conf the configuration of the server
     */
    void readFromUserSocket(int userSocket, Commands &commands);

    /**
     * Sends a message to a socket.
     *
     * @param socket to send the message to
     * @param message to send
     */
    void sendToClient(int socket, string message);

    /**
     * Accepts a new socket to connect to the server.
     * @return the accepted socket
     */
    int allocateSocketClient();

private:
    /**
     * Takes care of the logic to receive a file from the client.
     *
     * @param filename the file to be uploaded
     * @param size the size of the file
     * @param port the port where to receive the file
     */
    static void receiveFileUpload(string filename, unsigned int size, unsigned int port);

    /**
     * Sends a file to the client.
     *
     * @param filename the file to send
     * @param port the port from where the file will be sent
     */
    static void sendFile(string filename, unsigned int port);

    /**
     * Generates a random port number.
     *
     * @return the port number
     */
    int getRandomPort();

    /**
     * Constant that gives the max consecutive read error from a socket. Once this threshold is exceeded, we close the connection to the client.
     */
    const int MAX_WRONG_READ = 10;
};


#endif //ASS_ON_THE_GRASS_SERVER_H
