#include "headers.h"

char* pwd; char* old_pwd;
char* home; size_t home_size;

//-------------------------------------
// Set Environment Path Variables
//-------------------------------------

// Function to set old_pwd to current pwd
void change_old_pwd() {
	if(old_pwd != NULL) free(old_pwd);

	if((old_pwd = malloc(strlen(pwd))) == NULL) {
		perror("Allocation Error");
		exit(EXIT_FAILURE);
	}

	strcpy(old_pwd, pwd);
}

// Sets pwd and old_pwd
void set_pwd() {

	if(pwd != NULL) { // Change old_pwd
		change_old_pwd();
		free(pwd);
	}

	pwd = getcwd(NULL, 0);

	if(pwd == NULL) {
		perror("pwd");
		exit(EXIT_FAILURE);
	}
}

// Set home
void set_home() {
	// Set home and home_size
	home_size = strlen(pwd);

    if((home = malloc(home_size)) == NULL) {
		perror("Allocation Error");
		exit(EXIT_FAILURE);
	}

	strcpy(home, pwd);
}

// Deallocate memory
void pwd_free() {
	free(pwd);
	if(old_pwd != NULL) free(old_pwd);
	free(home);
}