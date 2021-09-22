#ifndef __PWD_H
#define __PWD_H

extern char* pwd;  		// Stores current directory path
extern char* old_pwd; 	// Stores previous working directory path

extern char* home;		// Stores home directory path
extern size_t home_size;// Size (Length) of home path

void change_old_pwd();	// Changes old_pwd (when pwd is changed)
void set_pwd();			// Changes pwd
void set_home();		// Set home and home_size

void pwd_free();		// To deallocate memory

#endif