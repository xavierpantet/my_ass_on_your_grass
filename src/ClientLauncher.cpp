/**
 * ClientLauncher.cpp
 * Implementation of the ClientLauncher.h file. Launches and runs the client to connect it to the server. Also does all tasks related to the user interface such as reading commands and also uploading / downloading files.
 *
 * @author Alexandre Chambet
 */

#include "../include/ClientLauncher.h"

using namespace std;

void ClientLauncher::downloadFile(string filename, unsigned int size, string serverIP, unsigned int port) {
    ClientSocket client(serverIP, port);

    ClientLauncher::fileTransferConnect(&client);

    client.downloadFile(filename, size);

    // Close the NetworkSocket
    client.closeConnection();
}


void ClientLauncher::uploadFile(string filename, string serverIP, unsigned int port) {
    ClientSocket client(serverIP, port);

    ClientLauncher::fileTransferConnect(&client);

    client.uploadFile(filename);

    // Close the NetworkSocket
    client.closeConnection();
}

void ClientLauncher::fileTransferConnect(ClientSocket *client) {
    // Loop while the server is ready
    int maxTries = 10;
    bool connected = false;
    while (maxTries > 0 && !connected) {
        // Tries to connect
        try {
            client->initiateConnection();
            connected = true;
        } catch (exception &e) {
            maxTries--;
            // Wait an exponential amount of time until the server is ready
            usleep(100 * (10 - maxTries));
        }
    }

    if (maxTries == 0) {
        throw Exception(ERR_NETWORK_CONNECTION_SERVER_FAILED);
    }

    // Should be ok, but we just check if the sock was properly created
    if (!(client->isSocketInitiated())) {
        throw Exception(ERR_NETWORK_SOCKET_NOT_CREATED);
    }
}

bool ClientLauncher::isValidIpAddress(char *ipAddress) {
    struct sockaddr_in sa;

    // Uses the inet_pton function to check if the IP is correct
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}


void ClientLauncher::startClient(string serverIP, unsigned int serverPort) {
    // Instantiate a new client
    ClientSocket client(serverIP, serverPort);

    client.initiateConnection();

    string command;
    string returned = "";

    // Loop while command is not exit
    do {
        // Read the command
        command = client.readCommand();

        try {
            // Tries to process it
            returned = processCommand(client, command, serverIP);

            // Print the result to the client
            if (returned != "") {
                cout << returned << endl;
            }
        } catch (Exception &e) {
            e.print_error();
        } catch (exception &e) {
            cout << "Error: invalid arguments." << endl;
        }

    } while (returned.compare(str_bye));
}


vector <string>
ClientLauncher::startClientAutomated(string serverIP, unsigned int serverPort, vector <string> commands) {
    cout << "Starting the client in the automated mode" << endl;

    // Instantiate a new client
    ClientSocket client(serverIP, serverPort);

    client.initiateConnection();

    // Iterator for the commands
    vector<string>::iterator it;

    // The string returned from the server
    vector <string> returned;

    for (it = commands.begin(); it != commands.end(); ++it) {
        // Get the command
        string command = *it;

        try {
            // Process the command
            string fromServer = processCommand(client, command, serverIP);

            // Append it the vector
            if (fromServer != "") {
                returned.push_back(std::move(fromServer));
            }
        } catch (Exception &e) {
            string ex = e.print_error();
            // Remove the \n
            ex = ex.substr(0, ex.size() - 1);
            returned.push_back(std::move(ex));
        } catch (exception &e) {
            cout << "Error: invalid arguments." << endl;
        }
    }

    cout << "Exiting the automated mode" << endl;

    return returned;
}

string ClientLauncher::processCommand(ClientSocket client, string command, string serverIP) {
    // The command without arguments
    string cmdWithoutArgs = command.substr(0, command.find(" "));

    // If a file must be upload
    if (cmdWithoutArgs == "put") {
        // Get the args of the command
        string removePut = command.substr(command.find(" ") + 1);

        // If no args, we throw an exception
        if (removePut == "" || removePut == "put") {
            throw Exception(ERR_INVALID_ARGS);
        }

        string filename = removePut.substr(0, removePut.find(" "));
        int size = stoi(removePut.substr(removePut.find(" ") + 1));

        // If size is below 0, then there is for sure a problem
        if (size < 0) {
            throw Exception(ERR_WRONG_FILE_SIZE);
        }

        // Cast into uint to be able to compare them with size_t
        unsigned int size_u = (unsigned int) size;

        // Checks if the size given is the same as the one in the file
        FileReader fr(filename);
        if (fr.fileSize() != size_u) {
            throw Exception(ERR_WRONG_FILE_SIZE);
        }

        // Send the command to the server
        client.sendToServer(command);

        // Wait for the server answer
        string read = client.readFromServer();

        // Check if the receive string is good, otherwise there was an error on the server side
        if (read.substr(0, 3) == "put") {
            // Parse the port number
            int port = stoi(read.substr(read.find(":") + 2));

            // Upload the file
            thread t1(ClientLauncher::uploadFile, filename, serverIP, port);
            t1.join();
        } else {
            // Display the error
            cout << read;
        }

        // Return empty strings for transfer operation
        return "";
    } else if (cmdWithoutArgs == "get") {
        // Get the args of the command
        string removeGet = command.substr(command.find(" ") + 1);

        // If no args, throw an exception
        if (removeGet == "" || removeGet == "get") {
            throw Exception(ERR_INVALID_ARGS);
        }

        string filename = removeGet.substr(0, removeGet.find(" "));

        // Send the command to the server
        client.sendToServer(command);

        // Wait for the server answer
        string read = client.readFromServer();

        // Check if the message has the expected form
        if (read.substr(0, 3) == "get") {
            // Not too much precaution here on the parsing because we assume the server sends the right stuff
            int port = stoi(read.substr(read.find(":") + 2));
            string withoutPort = read.substr(read.find(":") + 1);
            int size = stoi(withoutPort.substr(withoutPort.find(":") + 2));

            // Download the file
            thread t1(ClientLauncher::downloadFile, filename, size, serverIP, port);
            t1.join();
        } else {
            // Display the error
            cout << read;
        }

        // Return empty strings for transfer operation
        return "";
    } else {
        // Send the command to the server
        client.sendToServer(command);

        // Read and print the result from the server
        string fromServer = client.readFromServer();

        // Remove the "\n" (i.e. shitty fix)
        fromServer = fromServer.substr(0, fromServer.size() - 1);

        return fromServer;
    }
}

int main(int argc, char *argv[]) {
    // Checks the number of arguments
    if (argc != 3 && argc != 5) {
        cout << "Number of arguments specified is not correct" << endl;
        return -1;
    }

    // Get the first 2 arguments
    char *ip = argv[1];
    int port = atoi(argv[2]);

    // Checks the port
    if (port <= 0) {
        cout << "Port is not valid" << endl;
        return -1;
    }

    // Casts the port to an unsigned int
    unsigned int serverPort = (unsigned int) port;

    // Checks the IP
    if (!ClientLauncher::isValidIpAddress(ip)) {
        cout << "Server IP provided is not valid" << endl;
        return -1;
    }

    string serverIP(ip);

    // Create the object to launch the client
    ClientLauncher clientLauncher;

    // If the infile and outfile for automation are also specified
    if (argc == 5) {
        // Get the infile and outfile
        string inFile(argv[3]);
        string outFile(argv[4]);

        try {
            // Checks if infile exists
            FileReader fr("./" + inFile);
            FileWriter fw("./" + outFile);

            // Clear the output file
            fw.clearFile();

            // We read all the commands
            vector <string> commands;
            fr.readFileVector(commands);

            // Launches the client in automated mode
            vector <string> output = clientLauncher.startClientAutomated(serverIP, serverPort, commands);

            fw.write(output);

        } catch (Exception &e) {
            e.print_error();
            return -1;
        }
    } else {
        // Launches the client in normal mode
        try {
            clientLauncher.startClient(serverIP, serverPort);
        } catch (Exception &e) {
            e.print_error();
        } catch (exception &e) {
            cout << "An unknown error has happened." << endl;
        }
    }
    return 0;
}
