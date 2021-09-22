#ifndef __CMD_H
#define __CMD_H

#define CMD_SIZE 6 // Number of commands defined

// Array containing user-defined command names
extern const char* CMDS[CMD_SIZE];

// Function calls the appropriate shell function after parsing input
// And returns respective status
int execute(int, char**);

int cmd_exit(int argc, char** argv);
int cmd_pwd(int argc, char** argv);
int cmd_echo(int argc, char** argv);
int cmd_cd(int argc, char** argv);
int repeat(int argc, char** argv);

// For ls
void display_month(int);
void disp_time(struct timespec*);
void ls_display_verbose(char*, char*);
void ls_file_display(int a_flag, int l_flag, char* path, char* file_name);
void ls_dir(int a_flag, int l_flag, char* path);
int ls(int argc, char** argv);

// Array of pointers to functions (commands)
extern int (*func[])(int, char**);


#endif