#ifndef __EXEC_H
#define __EXEC_H

#define TOKEN_BUFFER_SIZE 128 // Default Number of tokens per line of input
#define CMD_SIZE 10 // Number of user-defined commands 
#define JOB_SIZE 20 // Maximum Number of Jobs

#define FG 0
#define BG 1
#define ST 2

struct JOB {
    int number;
    int job_type; /* Running (FG or BG) or ST (stopped) or -1 (default) */
    char** argv;
    pid_t pid;
};

extern struct JOB job[];
extern int job_number;

void kill_jobs();
void clear_job(int);
void init_job();
int assign_job(pid_t, char**, int);
int fgpid();
int delete_job(pid_t);
int search_job(pid_t);
int search_job_number(int);

int compare_jobs(const void*, const void*);

void display_args(char**);
int display_jobs(int, int);

int cmd_jobs(int, char**);
int cmd_sig(int, char**);
int cmd_fg(int, char**);
int cmd_bg(int, char**);

// Array containing user-defined command names
extern const char* CMDS[];

// Array of pointers to functions (builtin commands)
extern int (*func[])(int, char**);

char** parse(char** line); // To parse each line into words
int args_count(char** argv); /* Finds count of arguments */

void rearrange(int, char**);

/* EXECUTING FUNCTIONS */

/* Function executes given command */
int launch(char** argv, int);

int execute(char**);

void display_operator_error(const char*);

int check_pipe(char**);

int open_file(char*, int, mode_t);
void display_open_file_error(char*);

struct REDIRECTION {
    char* in_file;
    char* out_file;
    char* append_file;
    enum write_to_file {
        UNDEF, /* Initial case */
        OUT, 
        APPEND 
    } write_file; /* Will handle the case when both ">>" and ">" operators are passed in input */
};

int redirect(char**, struct REDIRECTION*);
int check_redirection(char**, struct REDIRECTION*);
int process_redirection(char**);

int pipeline(int, char**);

int check_background(char**);
int handle_bg(int, char**);

int execute_line(char**);

/* Other builtin commands */
int repeat(int argc, char** argv);

#endif