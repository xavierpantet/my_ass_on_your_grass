/**
 * AuthorizationPolicy.h
 * Describes all possible access policies to all resources of the system.
 * @author: Xavier Pantet
 */
#ifndef ASS_ON_THE_GRASS_AUTHORIZATIONPOLICY_H
#define ASS_ON_THE_GRASS_AUTHORIZATIONPOLICY_H

/**
 * Access policies to system resources.
 *
 * All resources are assigned a policy level that user must fulfill in order to be allowed to access the desired resource.
 */
enum AuthorizationPolicy {
    // Publicly available
    anonymous,
    // (Logged in) users only
    user
};

#endif
