/**
 * Client.h
 * Header file for Client.cpp.
 *
 * @author Alexandre Chambet
 */

#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <thread>

#include "grass.h"
#include "ClientSocket.h"
#include "FileReader.h"
#include "FileWriter.h"
#include "Exception.h"
#include "Configuration.h"
#include "Commands.h"

using namespace std;

class ClientLauncher {
public:
    /**
     * Downloads a file from the server.
     *
     * @param filename the file to be downloaded
     * @param size the size of the file
     * @param port the port where the file will be sent by the server
     */
    static void downloadFile(string filename, unsigned int size, string serverIP, unsigned int port);

    /**
     * Uploads a file to the server.
     *
     * @param filename the file to be uploaded
     * @param size the size of the file
     * @param port the port where the file should be send
     */
    static void uploadFile(string filename, string serverIP, unsigned int port);

    /**
     * Utils method to manage common stuff from upload and download such as connecting to the server, creating a new socket on the right port and much more.
     *
     * @param client the client socket object which will be used for the transfer
     */
    static void fileTransferConnect(ClientSocket *client);

    /**
     * Utils method that determines whether or not an string IP address is correct.
     *
     * @param ipAddress the string to be checked
     * @return true if the IP is correct, false otherwise
     */
    static bool isValidIpAddress(char *ipAddress);


    /**
     * Starts a new client and connects to the server on the given port.
     * Typically, this will be called in the main function to start the client and run it.
     *
     * @param serverIP the server's IP to connect to
     * @param port the server's port to connect to
     */
    void startClient(string serverIP, unsigned int port);

    /**
     * Starts a new client and connects to the server on the given port.
     * Typically, this will be called in the main function to start the client and run it.
     *
     * @param serverIP the server's IP to connect to
     * @param port the server's port to connect to
     * @param commands the list of commands to run on the client
     * @return a vector of string containing what the server returned
     */
    vector<string> startClientAutomated(string serverIP, unsigned int port, vector<string> commands);

private:
    string processCommand(ClientSocket client, string command, string serverIP);
};
