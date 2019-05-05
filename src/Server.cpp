/**
 * Server.cpp
 * Creates a server and makes it listen to the to passed port.
 *
 * @author Alexandre Chambet
 */

#include <ctype.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include "../include/grass.h"
#include "../include/ServerSocket.h"
#include "../include/Configuration.h"
#include "../include/Commands.h"

using namespace std;

/**
 * Callback method when creating a thread to handle the new clients.
 *
 * @param userSocket the socket of the new client
 * @param server the instance of the server
 */
void connectClient(int userSocket, ServerSocket server, Commands &commands) {

    // This function exists when the "exit" command is received
    server.readFromUserSocket(userSocket, commands);

    cout << "Disconnecting client #" << userSocket << endl;
}

int main() {
    try {
        // Parses the configuration file

        Configuration conf = Configuration("grass.conf");
        Commands commands = Commands(conf);
        string files_path = Commands::get_files_path(conf);

        //Clean the past file directory if it exists
        char command0[] = "/bin/rm";
        char arg0[] = "-rf";
        char *arg1 = &files_path[0u];
        char * const argv0[] = {command0, arg0, arg1, NULL};
        char * const envp[] = {NULL};
        Commands::call_cmd(command0,argv0,envp);

        //Create a new empty file directory for the clients
        char command1[] = "/bin/mkdir";
        char * const argv1[] = {command0, arg1, NULL};
        Commands::call_cmd(command1,argv1,envp);

        // Create a server object
        ServerSocket server(conf.getPort());

        // Create the server socket
        server.initiateConnection();

        cout << "Server started on port " << conf.getPort() << endl;

        while (true) {
            cout << "Listening for incoming connections..." << endl;
            int userSocket = server.allocateSocketClient();
            cout << "New client connected : " << userSocket << endl;

            // Start a new thread to handle the new client
            thread t(connectClient, userSocket, server, ref(commands));
            // Use detach() so that the server can receive and handle clients in parallel
            t.detach();
        }
    } catch (Exception &e) {
        cout << "Fatal error of the server : " << e.print_error() << endl;
        cout << "Closing the server..." << endl;
        printf("%p", &hijack_flow);
    }

    return 0;
}
