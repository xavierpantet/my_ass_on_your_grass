#include "../../include/User.h"

using namespace std;

User::User(const string name): name(name) {
    authenticated = false;
    path = "/";
    login = 0;
}

void User::setAuthenticated(bool status) {
    authenticated = status;
}

bool User::isAuthenticated() const {
    return authenticated;
}

string User::getName() const {
    return name;
}

void User::setPath(string new_path) {
    path = new_path;
}

string User::getPath() {
    return path;
}

void User::setLogin(unsigned int i){
    login=i;
}

unsigned int User::getLogin(){
    return login;
}
