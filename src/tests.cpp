#include "../tests/ConfigurationTests.h"
#include "../tests/AuthorizationServiceTests.h"
#include "../tests/AuthenticationServiceTests.h"
#include "../tests/ExceptionsTests.h"
#include "../tests/CommandTests.h"

int main(void) {
    // Tests for Configuration
    testGetBaseShouldReturnCorrectBaseOnStandardConfig();
    testGetBaseShouldReturnErrorOnMissingEntry();
    testGetBaseShouldReturnErrorOnMissingValue();
    testGetPortShouldReturn8888OnStandardConfig();
    testGetPortShouldReturnErrorOnMissingEntry();
    testGetPortShouldReturnErrorOnMissingValue();
    testGetUsersShouldReturnCorrectMapOnStandardConfig();
    testGetUsersShouldReturnEmptyMapOnMissingEntries();
    testGetUsersShouldIgnoreMalformedEntries();

    // Tests for AuthorizationService
    testsForAuthenticatedUser();
    testsForUnauthenticatedUser();

    // Tests for AuthenticationSevice
    testShouldAcceptToRegisterOnNewSocket();
    testShouldRejectToRegisterOnExistingSocket();
    testShouldConnectUserWithCorrectCredentials();
    testShouldNotConnectUserWithIncorrectCredentials();
    testShouldRemoveUserOnLogout();
    testShouldReturnCorrectListOfAuthenticatedUsers();

    //Test for exceptions
    /*
     * Commented out because it prints the exceptions
    testError1();
    testError2();
    */
}
