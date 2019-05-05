#include "../../../include/AuthorizationService.h"

using namespace std;

map<string, AuthorizationPolicy> init_policies() {
    map<string, AuthorizationPolicy> m;
    m["login"] = anonymous;
    m["pass"] = anonymous;
    m["ping"] = anonymous;
    m["ls"] = user;
    m["cd"] = user;
    m["mkdir"] = user;
    m["rm"] = user;
    m["get"] = user;
    m["put"] = user;
    m["grep"] = user;
    m["date"] = user;
    m["whoami"] = user;
    m["w"] = user;
    m["logout"] = user;
    m["exit"] = anonymous;
    return m;
}

AuthorizationService::AuthorizationService(const User& user): user(user) {}

map<string, AuthorizationPolicy> AuthorizationService::policies = init_policies();

bool AuthorizationService::hasAccessTo(const string command) {
    if(policies.find(command) != policies.end()) {
        return policies.at(command) == anonymous || (policies.at(command) == AuthorizationPolicy::user && user.isAuthenticated());
    }
    return false;
}
