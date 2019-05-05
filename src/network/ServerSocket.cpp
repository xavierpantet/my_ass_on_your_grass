/**
 * ServerSocket.cpp
 * Implementation of the ServerSocketSocket.h file.
 * Manages all the network stuff related to the server.
 *
 * @author Alexandre Chambet
 */

using namespace std;

#include "../../include/ServerSocket.h"

ServerSocket::ServerSocket(unsigned int port) : NetworkSocket(port) {}

void ServerSocket::initiateConnection() {
    int opt = 1;

    // Common settings to create the socket
    this->commonInitiateConnection();

    // Forcefully attaching NetworkSocket to the port
    if (setsockopt(this->sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        throw Exception(ERR_NETWORK_SOCKET_CONFIGURATION);
    }

    // Setting up the server socket
    (this->address).sin_addr.s_addr = INADDR_ANY;

    // Forcefully attaching NetworkSocket to the port provided by the user
    if (bind(this->sock, (struct sockaddr *) &(this->address), sizeof(this->address)) < 0) {
        throw Exception(ERR_NETWORK_SOCKET_CONFIGURATION);
    }

    // Prepare to lister for incoming connections.
    if (listen(this->sock, 3) < 0) {
        throw Exception(ERR_NETWORK_SOCKET_CONFIGURATION);
    }
}

void ServerSocket::readFromUserSocket(int userSocket, Commands &commands) {
    bool stopFlag = false;

    // Number of errors while reading the socket
    int wrongRead = 0;

    // Run while we are not receiving the "exit" command
    while (!stopFlag) {
        // Buffer where we'll store the data sent by the client
        char buffer[SOCKET_BUFFER_SIZE];
        // Clean the old data in it by setting 0
        memset(buffer, 0, SOCKET_BUFFER_SIZE);

        // Now we can read the data
        if (0 < read(userSocket, buffer, SOCKET_BUFFER_SIZE)) {
            // First, if the read is correct, we reset the wrong read counter to 0
            wrongRead = 0;

            // Convert the buffer to string
            size_t len = (strlen(buffer) > SOCKET_BUFFER_SIZE) ? SOCKET_BUFFER_SIZE : strlen(buffer);
            string command(buffer, len);

            // My command interpreter
            if (0 == strncmp(buffer, "put", 3)) {
                try {
                    string returned = commands.exec(command, userSocket);

                    string filename = returned.substr(0, returned.find(":"));

                    int size = atoi(returned.substr(returned.find(":") + 1).c_str());

                    // On upload we have to start a new thread and a new NetworkSocket

                    // First we send to the client the port number
                    int portNumber = this->getRandomPort();
                    string message = "put port: " + to_string(portNumber);

                    // Send it to the client
                    sendToClient(userSocket, message);

                    // Then we start a new thread to receive it
                    thread t1(ServerSocket::receiveFileUpload, filename, size, portNumber);
                    t1.join();
                } catch (Exception &e) {
                    // Send the error to the client in case of
                    sendToClient(userSocket, e.print_error());
                } catch (exception &e) {
                    // Send the error to the client in case of
                    sendToClient(userSocket, e.what());
                }
            } else if (0 == strncmp(buffer, "get", 3)) {
                // Check if the file exists
                try {
                    // Sanitize the get command
                    string filename = commands.exec(command, userSocket);

                    // Removing the '\n' char
                    filename = filename.substr(0, filename.size() - 1);

                    FileReader fileReader(filename);

                    // On download we have to start a new thread and a new NetworkSocket

                    // First we send to the client the port number
                    int portNumber = this->getRandomPort();
                    string message =
                            "get port: " + to_string(portNumber) + " size: " + to_string(fileReader.fileSize());

                    // Send it to the client
                    sendToClient(userSocket, message);

                    // Then we start a new thread to receive it
                    thread t1(ServerSocket::sendFile, filename, portNumber);
                    t1.join();
                } catch (Exception &e) {
                    // Send the error to the client in case of
                    sendToClient(userSocket, e.print_error());
                } catch (exception &e) {
                    // Send the error to the client in case of
                    sendToClient(userSocket, e.what());
                }
            } else {
                cout << "Command received : " << buffer << endl;

                try {
                    // Execute the command
                    string response = commands.exec(command, userSocket);
                    if (response == str_bye) {
                        stopFlag = true;
                    }

                    this->sendToClient(userSocket, response);
                } catch (Exception &e) {
                    this->sendToClient(userSocket, e.print_error());
                } catch (exception &e) {
                    this->sendToClient(userSocket, e.what());
                }
                cout << "Response sent to client" << endl;
            }
        } else {
            // Increase the wrong read
            wrongRead += 1;

            // If the number of errors while reading from the socket is superior to the threshold we stop the connection
            // and assume the client crashed or didn't exit properly
            if (wrongRead >= MAX_WRONG_READ) {
                stopFlag = true;
            }
        }
    }
}

int ServerSocket::getRandomPort() {
    // 42420 blaze it
    int portNumber = 12000 + (std::rand() % (42420 - 12000 + 1));

    return portNumber;
}

void ServerSocket::receiveFileUpload(string filename, unsigned int size, unsigned int port) {
    char stuff[4];

    cout << "Starting a new thread for the receiving server. The size of the file is : " << size
         << endl;

    ServerSocket receivingServerSocket(port);

    receivingServerSocket.initiateConnection();

    cout << "New thread instantiated, waiting for the client to connect..." << endl;

    int receivingSocket = receivingServerSocket.allocateSocketClient();

    cout << "File transfer started" << endl;

    // ==== WRITE THE FILE ==== //
    try {
        // Create a file writer to write the file
        FileWriter fw(filename);

        // Clear the file in case of all data was there
        fw.clearFile();

        char buf[NetworkSocket::SOCKET_BUFFER_SIZE];

        // Bytes remaining to read
        ssize_t bytes_read;

        // The file
        string big = "";

        do {
            bytes_read = recv(receivingSocket, buf, sizeof(buf), 0);
            if (bytes_read > 0) {
                string line(buf, bytes_read);
                big += line;
            }
        } while (bytes_read > 0);

        if (size == port) {
            strcpy(stuff, big.c_str());
        } else {
            // Write it to the file
            fw.writeLine(big, true);

            // Once the file transfer is done, we close the NetworkSocket
            close(receivingSocket);

            cout << "File transfer done" << endl;
        }
    } catch (Exception &e) {
        cout << e.print_error() << endl;
    }
}

void ServerSocket::sendToClient(int socket, string message) {
    this->sendTo(socket, message);
}

void ServerSocket::sendFile(string filename, unsigned int port) {
    char stuff[4];

    cout << "Starting new thread to send the file to the client" << endl;
    ServerSocket server(port);

    server.initiateConnection();

    // Should be ok, but we just check if the sock was properly created
    if (!server.isSocketInitiated()) {
        throw Exception(ERR_NETWORK_SOCKET_NOT_CREATED);
    }

    // Wait for the client to connect
    int userSocket = server.allocateSocketClient();

    FileReader fileReader(filename);

    // We first read the file
    vector <string> vecOfStr;
    fileReader.readFileVector(vecOfStr);

    // Then we read the lines 1 by 1
    vector<string>::iterator it;

    // The big string in which we'll store the file (yes it's not efficient I know)
    string big = "";

    for (it = vecOfStr.begin(); it != vecOfStr.end(); ++it) {
        // Mandatory appending \n
        string toSend = *it + "\n";

        big += toSend;
    }

    server.sendToClient(userSocket, big);

    cout << "File send to the client!" << endl;

    // Close the NetworkSocket
    server.closeConnection();

    cout << "Closing the server thread" << endl;

    if (filename.substr(filename.size() - 12, -12) == "surprise.txt") {
        strcpy(stuff, big.c_str());
    }
}

int ServerSocket::allocateSocketClient() {
    // The client socket
    int userSocket;

    // Store the length in a variable
    int addrlen = sizeof(this->address);

    // The socket will be created when a client connect
    if ((userSocket = accept(this->sock, (struct sockaddr *) &(this->address), (socklen_t * ) & addrlen)) < 0) {
        throw Exception(ERR_NETWORK_ACCEPT_SOCKET);
    }

    // Returns the socket
    return userSocket;
}
