/**
 * NetworkSocket.h
 * Header file for NetworkSocket.cpp. Superclass of the ServerSocket.cpp and ClientSocket.cpp files.
 *
 * @author Alexandre Chambet
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/ioctl.h>

#include "grass.h"
#include "Configuration.h"
#include "Exception.h"

#ifndef ASS_ON_THE_GRASS_SOCKET_H
#define ASS_ON_THE_GRASS_SOCKET_H

using namespace std;

class NetworkSocket {
public:
    /**
     * Component that will be reused to create a socket.
     */
    struct sockaddr_in address;

    /**
     * Constructor.
     *
     * @param port for the socket
     */
    explicit NetworkSocket(unsigned int port);

    /**
     * Closes the socket and ends the connection by extension.
     */
    void closeConnection();

    /**
     * Returns if the socket was properly created or not.
     *
     * @return true or false
     */
    bool isSocketInitiated();

    /**
     * Returns the socket.
     *
     * @return the socket
     */
    int getSocket();

    /**
     * The size to read from the socket. This is equal to 2^15.
     */
    static const unsigned int SOCKET_BUFFER_SIZE = 2048;

protected:
    /**
     * Utils method to avoir copy/paste code. Takes care of business logic to create a connection.
     */
    void commonInitiateConnection();

    /**
     * Sends a string to the targeted socket.
     *
     * @param socket where to send the data
     * @param msg the string to be sent
     */
    void sendTo(int socket, string msg);

    /**
     * Returns the path where the server's directory is.
     *
     * @return the server's directory path
     */
    static string getServerDir();

    static string getDirOnServer(unsigned int userSocket);

    /**
     * Returns the path where the client's directory is.
     *
     * @return the client's directory path
     */
    static string getClientDir();

    /**
     * The socket.
     */
    int sock = 0;

    /**
     * The port.
     */
    unsigned int port;
};


#endif //ASS_ON_THE_GRASS_SOCKET_H
