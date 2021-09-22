#include "headers.h"
#include "config.h"

// Function to initialize shell (Set all environment variables)
void init() {
	set_username();
	set_hostname();
	set_pwd();
	set_home();
}

// Deallocate Memory (Terminate shell)
void terminate() {
	name_free();
	pwd_free();
}