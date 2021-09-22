# 2021121010 Assignment-2

## Specifications Completed
1, 2, 3 and 7.

## File Structure

### headers.h
Contains C library headers included in files.

### name.h and name.c
Contains definitions and functions on handling username and hostname for the shell.

### pwd.h and pwd.c
Contains definitions and functions relating to environment variables home, pwd and old_pwd.

### config.h and config.c
Contains definitions of name.h and pwd.h and also functions to initiate and terminate the shell.

### utils.h and utils.c
Contains utility functions to convert relative path (with respect to home) to absolute path.

### cmd.h and cmd.c
Contains definitions and functions for shell-builtins and user defined commands. Also contains execute function to call the appropriate commands and some other functions for `ls` command.

### main.c
Contains functions to display prompt, make shell REPL and the main() function.

### Makefile
Makefile to compile and store output in executable `main`.