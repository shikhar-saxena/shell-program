#include "headers.h"

char* username;         // Stores username
struct utsname* buf;    // Struct to store hostname


//-------------------------------------
// Username and Hostname
//-------------------------------------

// Returns 1 on failure
int set_username() {

	struct passwd* pwd; // pointer to store getpwuid output

	// Gets password file entry for userid 'uid' in pwd
    if((pwd = getpwuid(getuid())) == NULL)
    {
        perror("username");
        return 1;
    }

    username = pwd->pw_name;
    return 0;
}

// Returns 1 on failure
int set_hostname() {

    if((buf = malloc(sizeof(struct utsname))) == NULL) {
		perror("Allocation Error");
		return 1;
	}

    // uname gives information about current system
    if(uname(buf) == -1) {
        perror("hostname");
        return 1;
    }

    // buf->nodename contains the hostname
    return 0;
}

// To deallocate memory
void name_free() {
    if(buf != NULL)
        free(buf);
}
