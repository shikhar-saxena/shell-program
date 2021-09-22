#include "headers.h"

char* username;         // Stores username
struct utsname* buf;    // Struct to store hostname


//-------------------------------------
// Username and Hostname
//-------------------------------------


void set_username() {

	struct passwd* pwd; // pointer to store getpwuid output

	// Gets password file entry for userid 'uid' in pwd
    if((pwd = getpwuid(getuid())) == NULL)
    {
        perror("username");
        exit(EXIT_FAILURE);
    }

    username = pwd->pw_name;
}

void set_hostname() {

    if((buf = malloc(sizeof(struct utsname))) == NULL) {
		perror("Allocation Error");
		exit(EXIT_FAILURE);
	}

    // uname gives information about current system
    if(uname(buf) == -1) {
        perror("hostname");
        exit(EXIT_FAILURE);
    }

    // buf->nodename contains the hostname
}

// To deallocate memory
void name_free() {
    free(buf);
}
