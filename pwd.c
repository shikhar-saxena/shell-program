#include "headers.h"

char* pwd; char* old_pwd;
char* home; size_t home_size;

//-------------------------------------
// Set Environment Path Variables
//-------------------------------------

// Function to set old_pwd to current pwd
int change_old_pwd() {
	if(old_pwd != NULL) free(old_pwd);

	if((old_pwd = malloc(strlen(pwd) + 1)) == NULL) {
		perror("Allocation Error");
		return 1;
	}

	strcpy(old_pwd, pwd);
    return 0;
}

// Sets pwd and old_pwd
int set_pwd() {

	if(pwd != NULL) { // Change old_pwd
		change_old_pwd();
		free(pwd);
	}

	pwd = getcwd(NULL, 0);

	if(pwd == NULL) {
		perror("pwd");
		return 1;
	}
    return 0;
}

// Set home
int set_home() {
	// Set home and home_size
	home_size = strlen(pwd);

    if((home = malloc(home_size + 1)) == NULL) {
		perror("Allocation Error");
	    return 1;
	}

	strcpy(home, pwd);
    return 0;
}

// Deallocate memory
void pwd_free() {
	if(pwd != NULL) free(pwd);
	if(old_pwd != NULL) free(old_pwd);
	if(home != NULL) free(home);
}