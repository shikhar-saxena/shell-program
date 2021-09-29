#ifndef __CMD_H
#define __CMD_H

int cmd_exit(int argc, char** argv);
int cmd_pwd(int argc, char** argv);
int cmd_echo(int argc, char** argv);
int cmd_cd(int argc, char** argv);

// For ls
void display_month(int);
void disp_time(struct timespec*);
void ls_display_verbose(char*, char*);
void ls_file_display(int a_flag, int l_flag, char* path, char* file_name);
void ls_dir(int a_flag, int l_flag, char* path);
int ls(int argc, char** argv);



#endif