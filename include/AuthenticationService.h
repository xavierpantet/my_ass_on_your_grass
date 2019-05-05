/**
 * AuthenticationService.h
 * Handles user authentication from a Configuration which contains user definitions.
 * @author: Xavier Pantet
 */

#include <map>
#include <string>
#include <mutex>

#include "grass.h"
#include "Configuration.h"
#include "Exception.h"

#ifndef USER
#include "User.h"
#endif

#ifndef ASS_ON_THE_GRASS_AUTHENTICATIONSERVICE_H
#define ASS_ON_THE_GRASS_AUTHENTICATIONSERVICE_H

class AuthenticationService {
private:
    const Configuration& config;
    map<unsigned int, User> users;

public:

    explicit AuthenticationService(const Configuration&);

    /**
     * Registers a user to the authentication service.
     * This prepares the service to accept a user with the given name for the given socket.
     * Useful for login command.
     * @param socketID A unique identifier for the socket
     * @param name The username of the desired user
     */
    void registerUser(const unsigned int socketID, const string name);

    /**
     * Actually logs in the user that has been previously registered with the given name and socket.
     * @param socketID The unique socket identifier used to register the user
     * @param name The username used to register the user
     * @param pass The password of the user
     * @return true iff login is successful (ie name and pass correspond to a valid user in the configuration)
     */
    bool login(const unsigned int socketID, const string name, const string pass);

    /**
     * Logs out a user and removes its entry from the authentication service.
     * @param socketID The socket identifier to be cleaned
     */
    void logout(const int socketID);

    /**
     * Returns the User instance of the user connected to the given socket identifier.
     * @param socketID The socket identifier we want to retrieve the user from
     * @return The User connected to the given socket
     */
    User getUser(const unsigned int socketID);

    /**
     * Updates the user at the given socketID.
     * @param socketID The socket identifier we want to update the user to
     * @param user The new user
     */
    void setUser(const unsigned int socketID, User user);

    /**
     * Returns all connected users.
     * This means all users that have been registered to the authentication service and that have been successfully logged in.
     * @return The list of all logged in users
     */
    vector<User> getAuthenticatedUsers();
};

#endif //ASS_ON_THE_GRASS_AUTHENTICATIONSERVICE_H
