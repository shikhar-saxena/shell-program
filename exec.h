#ifndef __EXEC_H
#define __EXEC_H

#define CMD_SIZE 6 // Number of commands defined

// Array containing user-defined command names
extern const char* CMDS[];

// Array of pointers to functions (builtin commands)
extern int (*func[])(int, char**);

// Function calls the appropriate shell function
// And returns respective status
int execute(int, char**);
int launch_fg(int argc, char** argv);

int repeat(int argc, char** argv);

#endif