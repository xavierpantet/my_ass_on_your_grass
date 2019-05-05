#include <assert.h>
#include "AuthorizationServiceTests.h"
#include "../include/AuthorizationService.h"

void testsForAuthenticatedUser() {
    User u("");
    u.setAuthenticated(true);
    AuthorizationService service(u);
    assert(service.hasAccessTo("login"));
    assert(service.hasAccessTo("pass"));
    assert(service.hasAccessTo("ping"));
    assert(service.hasAccessTo("ls"));
    assert(service.hasAccessTo("cd"));
    assert(service.hasAccessTo("mkdir"));
    assert(service.hasAccessTo("rm"));
    assert(service.hasAccessTo("get"));
    assert(service.hasAccessTo("put"));
    assert(service.hasAccessTo("grep"));
    assert(service.hasAccessTo("date"));
    assert(service.hasAccessTo("whoami"));
    assert(service.hasAccessTo("w"));
    assert(service.hasAccessTo("logout"));
    assert(service.hasAccessTo("exit"));
    assert(!service.hasAccessTo("notACommand"));
}

void testsForUnauthenticatedUser() {
    User u("");
    u.setAuthenticated(false);
    AuthorizationService service(u);
    assert(service.hasAccessTo("login"));
    assert(service.hasAccessTo("pass"));
    assert(service.hasAccessTo("ping"));
    assert(!service.hasAccessTo("ls"));
    assert(!service.hasAccessTo("cd"));
    assert(!service.hasAccessTo("mkdir"));
    assert(!service.hasAccessTo("rm"));
    assert(!service.hasAccessTo("get"));
    assert(!service.hasAccessTo("put"));
    assert(!service.hasAccessTo("grep"));
    assert(!service.hasAccessTo("date"));
    assert(!service.hasAccessTo("whoami"));
    assert(!service.hasAccessTo("w"));
    assert(!service.hasAccessTo("logout"));
    assert(!service.hasAccessTo("exit"));
    assert(!service.hasAccessTo("notACommand"));
}
