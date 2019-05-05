#include <assert.h>
#include "../include/FileReader.h"
#include "../include/Configuration.h"
#include "../include/AuthenticationService.h"
#include "AuthenticationServiceTests.h"

void testShouldAcceptToRegisterOnNewSocket() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    //assert(s.registerUser(1, "Xavier"));
    //assert(s.registerUser(2, "Guissou"));
    assert(s.getAuthenticatedUsers().size() == 0);
}

void testShouldRejectToRegisterOnExistingSocket() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    //assert(s.registerUser(1, "Xavier"));
    //assert(!s.registerUser(1, "Xavier"));
    //assert(!s.registerUser(1, "Guissou"));
    assert(s.getAuthenticatedUsers().size() == 0);
}

void testShouldConnectUserWithCorrectCredentials() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    //assert(s.registerUser(1, "Alex"));
    assert(s.login(1, "Acidburn", "CrashOverride"));
    assert(s.getAuthenticatedUsers().size() == 1);
}

void testShouldNotConnectUserWithIncorrectCredentials() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    //assert(s.registerUser(1, "Alex"));
    assert(!s.login(1, "Alex", "ElFuego"));
    assert(s.getAuthenticatedUsers().size() == 0);
}

void testShouldRemoveUserOnLogout() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    //assert(s.registerUser(1, "Alex"));
    s.logout(1);
    assert(s.getAuthenticatedUsers().size() == 0);
    //assert(s.registerUser(1, "Acidburn"));
    assert(s.login(1, "Acidburn", "CrashOverride"));
    assert(s.getAuthenticatedUsers().size() == 1);
    s.logout(1);
    assert(s.getAuthenticatedUsers().size() == 0);
}

void testShouldReturnCorrectListOfAuthenticatedUsers() {
    Configuration conf("grass.conf");
    AuthenticationService s(conf);
    s.registerUser(1, "Acidburn");
    s.login(1, "Acidburn", "CrashOverride");
    assert(s.getAuthenticatedUsers().size() == 1);
    assert(s.getAuthenticatedUsers()[0].getName() == "Acidburn");
    assert(s.getAuthenticatedUsers()[0].isAuthenticated());
}
