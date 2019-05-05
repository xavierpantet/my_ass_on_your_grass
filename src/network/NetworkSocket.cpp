/**
 * NetworkSocket.cpp
 * Implementation of the NetworkSocket.h file.
 * Superclass implementing common methods for both client and server sockets.
 *
 * @author Alexandre Chambet
 */

#include "../../include/NetworkSocket.h"

using namespace std;

NetworkSocket::NetworkSocket(unsigned int port) {
    // Assigns the port
    this->port = port;
}

void NetworkSocket::closeConnection() {
    close(this->sock);
}

bool NetworkSocket::isSocketInitiated() {
    return this->sock > 0;
}

int NetworkSocket::getSocket() {
    return this->sock;
}

void NetworkSocket::sendTo(int socket, string msg) {
    // Sends the data to the socket
    if (send(socket, msg.data(), msg.size(), 0) <= 0) {
        // Throw a write error
        throw Exception(ERR_NETWORK_WRITE_SOCKET);
    }
}

void NetworkSocket::commonInitiateConnection() {
    // Create the socket
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw Exception(ERR_NETWORK_CREATE_SOCKET);
    }

    // Configuration of the socket
    (this->address).sin_family = AF_INET;
    (this->address).sin_port = htons(this->port);
}

string NetworkSocket::getServerDir() {
    Configuration conf = Configuration("grass.conf");
    return conf.getBase();
}

string NetworkSocket::getClientDir() {
    return NULL;
}

string NetworkSocket::getDirOnServer(unsigned int userSocket) {
    Configuration conf = Configuration("grass.conf");
    return conf.getBase() + to_string(userSocket) + "/";
}
