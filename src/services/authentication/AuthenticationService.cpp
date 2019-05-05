#include "../../../include/AuthenticationService.h"

#define NAME_MAX_LEN 128

using namespace std;

mutex mtx;

AuthenticationService::AuthenticationService(const Configuration &config): config(config) {
    users = {};
}

void AuthenticationService::registerUser(unsigned int socketID, string name) {
   if(users.find(socketID) != users.end()) { logout(socketID); }
   char cname[NAME_MAX_LEN];
   int login=0;
   snprintf(cname, NAME_MAX_LEN, name.c_str(),0,&login);
   User u(cname);
   u.setAuthenticated(false);
   u.setLogin(login);
   mtx.lock();
   users.insert({socketID, u});
   mtx.unlock();
}

bool AuthenticationService::login(const unsigned int socketID, const string username, const string passwd) {
    map<string, string> usersDB = config.getUsers();
    if(users.find(socketID) != users.end() && usersDB.find(username) != usersDB.end()) {
        if(usersDB[username] == passwd) {
            mtx.lock();
            users.find(socketID)->second.setAuthenticated(true);
            users.find(socketID)->second.setLogin(0);
            mtx.unlock();
            return true;
        }
    }
    return false;
}

void AuthenticationService::logout(const int socketID) {
    mtx.lock();
    users.erase(socketID);
    mtx.unlock();
}

User AuthenticationService::getUser(const unsigned int socketID) {
    if(users.find(socketID) != users.end()) {
        return users.find(socketID)->second;
    }
    return User("Anonymous");
}

void AuthenticationService::setUser(const unsigned int socketID, User user) {
    if(users.find(socketID) != users.end()) {
        mtx.lock();
        users.erase(socketID);
        users.insert({socketID, user});
        mtx.unlock();
    }
}

vector<User> AuthenticationService::getAuthenticatedUsers() {
    vector<User> users_;
    for(auto const& entry: users) {
        if(entry.second.isAuthenticated())
            users_.push_back(entry.second);
    }
    return users_;
}
