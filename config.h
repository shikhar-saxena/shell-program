#ifndef __CONFIG_H
#define __CONFIG_H

#include "pwd.h" 
#include "name.h"

// Returns 1 (on failure)
int init(); 				// To Initialize the shell


void terminate(); 			// To terminate the shell (deallocate env vars)

#endif