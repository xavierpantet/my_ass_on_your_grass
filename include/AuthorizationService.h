/**
 * AuthorizationService.h
 * Determines which resources a given user is allowed to access to.
 * See AuthorizationPolicies.h for details.
 * @author: Xavier Pantet
 */

#include <string>
#include <map>

#include "AuthorizationPolicy.h"
#include "grass.h"

#ifndef USER
#include "User.h"
#endif

using namespace std;

#ifndef AUTHORIZATION_SERVICE
#define AUTHORIZATION_SERVICE

/**
 * Creates a map that associates a policy level to every resource.
 * @return Policy level - Resource association for every resource of the system.
 */
map<string, AuthorizationPolicy> init_policies();

class AuthorizationService {
    private:
        const User& user;

    public:

        explicit AuthorizationService(const User&);

        /**
         * Policy level required to access a given resource for any resource.
         */
        static map<string, AuthorizationPolicy> policies;

        /**
         * Determines whether the user has access to the given resource.
         * @return true iff the user has access to the given resource
         */
        bool hasAccessTo(const string resource);
};
#endif
