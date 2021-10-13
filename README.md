# 2021121010 Assignment-3

## Assignment 2 specifications
Except pinfo all specifications have been completed.

## Assignment 3 specifications
Except (^Z) handling all specifications have been completed.

## File Structure

### headers.h
Contains C library headers included in other files.

### name.h and name.c
Contains definitions and functions on handling username and hostname for the shell.

### pwd.h and pwd.c
Contains definitions and functions relating to environment variables home, pwd and old_pwd.

### config.h and config.c
Contains definitions of name.h and pwd.h and also functions to initiate and terminate the shell. Also contains variable `shell_pid` which stores the process id of the shell process.

### utils.h and utils.c
Contains utility functions to convert relative path (with respect to home) to absolute path.

### cmd.h and cmd.c
Contains definitions and functions for handling shell-builtins. These include functionality for exit, cd, pwd, echo and ls.

### exec.c and exec.h
Contains all functionality related to process heirarchy. We call `execute_line` function after getting input which parses the given input, then checks for pipeline (and executes it if present), then checks for redirection (and executes it if present), then checks for background processing else executes the simple command passed by the user.

A valid input is one where all operators are space-separated.


Therefore `<file, |cat, cat|` etc. will not work for the shell.

I have assumed that pipelining and redirection will not be done as background processes.

For redirection, multiple directives of same type are assumed to be invalid.

Example: `cat < file1.txt < file2.txt`
will only open file descriptor for file2.txt (two input direction files). Hence such inputs are considered invalid.


For redirection, I have made a struct that points to the in_file, out_file or append_file depending on the input.

`launch` function forks the main process and makes a bg or fg process as is required.

For job handling we have assumed types FG, BG and ST as running(foreground or background) or stopped respectively.

While displaying jobs it is assumed if -r and -s both are passed then all bg jobs will be displayed.

R and S status from proc file are considered as running and T as stopped.

There cannot be more than 20 jobs.

### main.c
Contains functions to display prompt, shell REPL and the main() function. Also contains signal handlers for ^C and ^Z (^D handled in getline).

### Makefile
Makefile to compile and store output in executable `main`.