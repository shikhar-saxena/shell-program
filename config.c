#include "headers.h"
#include "config.h"

int shell_pid;

// Function to initialize shell (Set all environment variables)
int init() {
	shell_pid = getpid();
	return set_username() || set_hostname() || set_pwd() || set_home();
}

// Deallocate Memory and Terminate shell
void terminate() {
	name_free();
	pwd_free();
	exit(0);
}