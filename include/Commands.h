/**
 * Commands.h
 * Handle the sanitization and commands on the server.
 * @author: Guillaume Michel
 */


#ifndef COMMANDS_H
#define COMMANDS_H

#include <ctype.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <string>
#include <sstream>
#include <algorithm>
#include <errno.h>
#include <fcntl.h>
#include <regex>

#include "grass.h"
#include "Exception.h"
#include "Configuration.h"
#include "AuthenticationService.h"
#include "StringHelper.h"

#ifndef AUTHORIZATION_SERVICE
#include "AuthorizationService.h"
#endif

using namespace std;

const string str_login  = "login";
const string str_pass   = "pass";
const string str_ping   = "ping";
const string str_ls     = "ls";
const string str_cd     = "cd";
const string str_mkdir  = "mkdir";
const string str_rm     = "rm";
const string str_get    = "get";
const string str_put    = "put";
const string str_grep   = "grep";
const string str_date   = "date";
const string str_whoami = "whoami";
const string str_w      = "w";
const string str_logout = "logout";
const string str_exit   = "exit";
const string str_pwd    = "pwd";
const string str_bye    = "bye!";
const string str_nodata = "__nodata__";

const string files_dir = "files";

class Commands {
public:
    explicit Commands(const Configuration config);

    /**
     * Get the path to the home file directory of the current user. The path is
     * given from the root of the host machine
     * @method get_files_path
     * @param  config         the config with informations to the files path
     * @return                the path to the home file directory of the given user
     */
    static string get_files_path(const Configuration config);


    /**
     * Sanitize the given command and either execute it and returns the output
     * or prints an error message on the server and returns the same error message
     *
     * @method exec
     * @param  command  The command that will be executed by the server
     * @param  socketID The socket id, which identifies the connection
     * @return          The output of the command, or an error message
     */
    string exec(string command, unsigned int socketID);

    /**
     * Makes a system call, using execve and the given parameters
     *
     * @method call_cmd
     * @param  command  path to the command that will be executed by execve
     * @param  argv     arguments of execve
     * @param  envp     environnment of execve
     * @return          the output of the system call
     */
    static string call_cmd(const char* command, char * const * argv, char * const * envp);

private:

    Configuration conf;
    AuthenticationService auth;
    string path;

    /**
     * Matches the given command string with a command, sanitize it, and executes
     * the command and return its output or return an invalid command error if
     * the command doesn't match any existing command
     * @method sanitize
     * @param  command  the command given by the user to be sanitized
     * @param  socketID the socket identifier that identifies the connection
     * @return          the output of the command or an error
     */
    string sanitize(string command, unsigned int socketID);

    /**
     * Remove the space characters (space, tab, newline, etc.) from the given input
     * @method remove_spaces
     * @param  input         given input
     * @return               the given input without any space character
     */
    static string remove_spaces(string input);

    /**
     * Remove the space characters (space, tab, newline, etc.) that are in front
     * of the given input until the first non space character
     * @method remove_front_spaces
     * @param  input         given input
     * @return               the given input without the space characters in front
     * of the text
     */
    static string remove_front_spaces(string input);

    /**
     * Verifies if the given string is empty, and throws an invalid arguments
     * error if it is empty
     * @method require_parameters
     * @param  param              parameter to be checked
     */
    void require_parameters(string param);

    /**
     * Verifies if the given string is empty, and throws an invalid arguments
     * error if it is not empty
     * @method require_parameters
     * @param  param              parameter to be checked
     */
    void require_no_parameters(string param);

    /**
     * Verify the given hostname, throw an invalid argument exception if it contains
     * characters other that "A-Z", "a-z", "0-9", '-', '.' and '/'.
     *
     * @method check_hostname
     * @param  str            the given hostname
     */
    static void check_hostname(string hostname);

    /**
     * Verify the given filename, throw an invalid argument exception if it contains
     * characters other that "A-Z", "a-z", "0-9", '-', '_' and '.', and if the filname
     * is more 128 characters long.
     * @method check_filename
     * @param  filename       the given filename
     */
    static void check_filename(string filename);

    /**
     * Verify the given path, throw an invalid argument exception if it contains
     * characters other than "A-Z", "a-z", "0-9", '-', '_', '.' and the separator
     * character '/'
     * @method check_path
     * @param  path       the given path
     */
    static void check_path(string path);

    /**
     * Get the current path of the given user. Each user has its current relative
     * path stored and this gives back the path from the root of the host machine
     * @method get_full_path
     * @param  socketID      the socket identifier to identify the user
     * @return               the current path where the given user is
     */
    string get_full_path(unsigned int socketID);

    /**
     * Set the current user's path to a new value.
     * @method set_user_path
     * @param  string        the new path to update to old one
     * @param  socketID      the socket identifier to identify the user
     */
    void set_user_path(string new_path, unsigned int socketID);

    /**
     * Return the current path of the user relative to the user's files directory
     * @method get_relative_path
     * @param  socketID          the socket identifier to identify the user
     * @return                   the relative path of the current location of the user
     */
    string get_relative_path(unsigned int socketID);

    /**
     * Check if the user is already logged in, and logout her if it is the case
     * @method user_already_logged
     * @param  socketID            socket identifier of the user
     */
    void user_already_logged(unsigned int socketID);

    /**
     * Check if the directory given by name exists in the directory given by dir,
     * return an exception with message if the directory doesn't exists
     * @method dir_exists
     * @param  dir        the directory that will be inspected
     * @param  name       the target directory in that we want to check in dir
     * @param  message    message containing the full path of the directory that
     * the user wants to access to throw the exception
     */
    static void dir_exists(string dir, string name, string message);

    /**
     * Execute the command pwd to get the path to the location where the program
     * is being run
     * @method  cmd_pwd
     * @return  the output of the command pwd
     */
    static string cmd_pwd();

    /**
     * Executes the login command
     * @method cmd_login
     * @param  param     username
     * @param  socketID  socket id of the current user
     * @return           "Ok... go on"
     */
    string cmd_login(string param, unsigned int socketID);

    /**
     * pass command, takes the password for the previous username and logs the user in
     * @method cmd_pass
     * @param  param    password
     * @param  socketID socket id of the current user
     * @return          success or failure
     */
    string cmd_pass(string param, unsigned int socketID);

    /**
     * ping command
     * @method cmd_ping
     * @param  param    host to ping
     * @param  int      not used
     * @return          the output of the ping command
     */
    string cmd_ping(string param, unsigned int);

    /**
     * ls command, list show with parameters -l
     * @method cmd_ls
     * @param  string   not used
     * @param  socketID socket id of the current user
     * @return          the output of the ls -l command
     */
    string cmd_ls(string, unsigned int socketID);

    /**
     * cd command, change directory
     * @method cmd_cd
     * @param  param    the target directory
     * @param  socketID socket id of the current user
     * @return          nothing on success or an error on failure
     */
    string cmd_cd(string param, unsigned int socketID);

    /**
     * mkdir command, create a directory
     * @method cmd_mkdir
     * @param  param     name of the directory to create
     * @param  socketID  socket id of the current user
     * @return           nothing on success or an error on failure
     */
    string cmd_mkdir(string param, unsigned int socketID);

    /**
     * rm command, removes a file or directory with -r argument
     * @method cmd_rm
     * @param  param    the file or directory to remove
     * @param  socketID socket id of the current user
     * @return          nothing on success or an error on failure
     */
    string cmd_rm(string param, unsigned int socketID);

    /**
     * Get command, download a file from the server
     * @method cmd_get
     * @param  param    filename
     * @param  socketID socket through which the file is sent
     * @return          the filename
     */
    string cmd_get(string param, unsigned int socketID);

    /**
     * Put command, upload a file to the server
     * @method cmd_put
     * @param  param    filename and size
     * @param  socketID socket through which the file is sent
     * @return          the filename along with the size
     */
    string cmd_put(string param, unsigned int socketID);

    /**
     * grep command
     * @method cmd_grep
     * @param  param    parameter of the grep command
     * @param  int      not used
     * @return          output of the grep command
     */
    string cmd_grep(string param, unsigned int);

    /**
     * date command
     * @method cmd_date
     * @param  strings  not used
     * @param  int      not used
     * @return          the date
     */
    string cmd_date(string, unsigned int);

    /**
     * Return the identity of the user
     * @method cmd_whoami
     * @param  string     not used
     * @param  socketID   used to get the identity of the user
     * @return            the identity of the user
     */
    string cmd_whoami(string, unsigned int socketID);

    /**
     * w command, list all the connected and logged in clients
     * @method cmd_w
     * @param  string   not used
     * @param  int not used
     * @return          the list of connected users
     */
    string cmd_w(string, unsigned int);

    /**
     * Logout command
     * @method cmd_logout
     * @param  string     not used
     * @param  socketID   socket id of the user to logout
     * @return            logout success message on success
     */
    string cmd_logout(string, unsigned int socketID);

    /**
     * Exit command, end the client session
     * @method cmd_exit
     * @param  string   not used
     * @param  int      not used
     * @return          goodbye message
     */
    string cmd_exit(string, unsigned int);

};

#endif
