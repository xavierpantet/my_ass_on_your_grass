#include <assert.h>
#include <map>
#include <string>
#include <iostream>
#include "ConfigurationTests.h"

void testGetBaseShouldReturnCorrectBaseOnStandardConfig() {
    Configuration conf("tests/testables/grass.conf");
    assert(conf.getBase() == "my_super_base");
}

void testGetBaseShouldReturnErrorOnMissingEntry() {
    Configuration conf("tests/testables/no_base_grass.conf");
    try {
        conf.getBase();
        assert(false);
    } catch(...) {
        assert(true);
    }
}

void testGetBaseShouldReturnErrorOnMissingValue() {
    Configuration conf1("tests/testables/malformed_base_grass_1.conf");
    try {
        conf1.getBase();
        assert(false);
    } catch(...) {
        assert(true);
    }

    Configuration conf2("tests/testables/malformed_base_grass_2.conf");
    try {
        conf2.getBase();
        assert(false);
    } catch(...) {
        assert(true);
    }
}

void testGetPortShouldReturn8888OnStandardConfig() {
    Configuration conf("tests/testables/grass.conf");
    assert(conf.getPort() == (unsigned int) 8888);
}

void testGetPortShouldReturnErrorOnMissingEntry() {
    Configuration conf("tests/testables/no_port_grass.conf");
    try {
        conf.getPort();
        assert(false);
    } catch(...) {
        assert(true);
    }
}

void testGetPortShouldReturnErrorOnMissingValue() {
    Configuration conf1("tests/testables/malformed_port_grass_1.conf");
    try {
        conf1.getPort();
        assert(false);
    } catch(...) {
        assert(true);
    }

    Configuration conf2("tests/testables/malformed_port_grass_2.conf");
    try {
        conf2.getPort();
        assert(false);
    } catch(...) {
        assert(true);
    }

    Configuration conf3("tests/testables/malformed_port_grass_3.conf");
    try {
        conf3.getPort();
        assert(false);
    } catch(...) {
        assert(true);
    }
}

void testGetUsersShouldReturnCorrectMapOnStandardConfig() {
    Configuration conf("tests/testables/grass.conf");
    map<string, string> users = conf.getUsers();
    map<string, string> usersTest = {
            {"XavierP", "TopSecretPasswd"},
            {"GuillaumeMichel", "FreeGuissou"},
            {"Alex", "ElFuego"}
    };

    assert(users.size() == usersTest.size());
    assert(equal(users.begin(), users.end(), usersTest.begin()));
}

void testGetUsersShouldReturnEmptyMapOnMissingEntries() {
    Configuration conf("tests/testables/no_users_grass.conf");
    assert(conf.getUsers().size() == 0);
}

void testGetUsersShouldIgnoreMalformedEntries() {
    Configuration conf("tests/testables/malformed_users_grass.conf");
    map<string, string> users = conf.getUsers();
    map<string, string> usersTest = {
            {"GuillaumeMichel", "FreeGuissou"},
            {"Alex", "ElFuego"}
    };

    assert(users.size() == usersTest.size());
    assert(equal(users.begin(), users.end(), usersTest.begin()));
}