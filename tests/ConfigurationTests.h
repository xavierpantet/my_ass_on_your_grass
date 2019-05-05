#include "../include/Configuration.h"

#ifndef ASS_ON_THE_GRASS_CONFIGURATIONTESTS_H
#define ASS_ON_THE_GRASS_CONFIGURATIONTESTS_H

void testGetBaseShouldReturnCorrectBaseOnStandardConfig();
void testGetBaseShouldReturnErrorOnMissingEntry();
void testGetBaseShouldReturnErrorOnMissingValue();
void testGetPortShouldReturn8888OnStandardConfig();
void testGetPortShouldReturnErrorOnMissingEntry();
void testGetPortShouldReturnErrorOnMissingValue();
void testGetUsersShouldReturnCorrectMapOnStandardConfig();
void testGetUsersShouldReturnEmptyMapOnMissingEntries();
void testGetUsersShouldIgnoreMalformedEntries();

#endif //ASS_ON_THE_GRASS_CONFIGURATIONTESTS_H
