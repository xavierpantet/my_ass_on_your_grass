#ifndef ASS_ON_THE_GRASS_AUTHENTICATIONSERVICETESTS_H
#define ASS_ON_THE_GRASS_AUTHENTICATIONSERVICETESTS_H

void testShouldAcceptToRegisterOnNewSocket();
void testShouldRejectToRegisterOnExistingSocket();
void testShouldConnectUserWithCorrectCredentials();
void testShouldNotConnectUserWithIncorrectCredentials();
void testShouldRemoveUserOnLogout();
void testShouldReturnCorrectListOfAuthenticatedUsers();

#endif //ASS_ON_THE_GRASS_AUTHENTICATIONSERVICETESTS_H
