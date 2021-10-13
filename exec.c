#include "headers.h"
#include "cmd.h"
#include "exec.h"
#include "config.h"
#include "utils.h"

/*********** JOB ***********/
struct JOB job[JOB_SIZE];
int job_number = 0;

/* Kills all jobs before exiting */
void kill_jobs() {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) continue;

        kill(job[i].pid, SIGKILL);
    }
}

void clear_job(int index) {
    job[index].number = -1;
    job[index].pid = 0;
    job[index].job_type = -1;
    job[index].argv = NULL;
}

void init_job() {
    for(int i = 0; i < JOB_SIZE; i++) 
        clear_job(i);
}

int assign_job(pid_t pid, char** argv, int job_type) {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) {
            job[i].number = ++job_number;
            job[i].pid = pid;
            job[i].job_type = job_type;
            job[i].argv = argv;
            return i;
        }
    }

    fprintf(stderr, "Jobs maximum limit reached\n");
    return -1;
}

/* Returns FG process pid else returns 0 */
int fgpid() {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) continue;

        if(job[i].job_type == FG) return job[i].pid;
    }

    return 0;
}

int delete_job(pid_t pid) {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].pid == pid) {
            clear_job(i);
            return 0;
        }
    }

    return -1; /* Job with passed pid not found */
}

int search_job(pid_t pid) {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) continue;

        if(job[i].pid == pid) return i;
    }

    return -1;
}

// Searches job given the job number
int search_job_number(int jobno) {
    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) continue;

        if(job[i].number == jobno) return i;
    }

    return -1;
}

// For qsort function
int compare_jobs(const void* _a, const void* _b) {
    const struct JOB *a  = _a;
    const struct JOB *b  = _b;
    if(a->argv == NULL && b->argv != NULL)
        return 1;
    if(a->argv != NULL && b->argv == NULL)
        return -1;
    if(a->argv == NULL && b->argv == NULL)
        return 0;
    
    int diff = strcmp(a->argv[0], b->argv[0]);
    if(diff == 0) return a->number - b->number; /* Sorts in ascending order of job number if command name is same */
    
    return diff;
}

void display_args(char** argv) {
    for(int i = 0; argv[i] != NULL; i++)
        printf("%s ", argv[i]);
}

int display_jobs(int R_FLAG, int S_FLAG) {

    qsort(job, JOB_SIZE, sizeof(struct JOB), compare_jobs);

    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) break;

        char* str = malloc(strlen("/proc//stat") + 1 + 10); // Assuming pid length to be <= 10
        sprintf(str, "/proc/%d/stat", job[i].pid);
        FILE *file = fopen(str, "r");
        free(str);

        if(file == NULL) { /* Delete this job */
            delete_job(job[i].pid);
            continue;
        }

        char c; int count = 0;

        do {
            c = getc(file);
            if(c == ' ') count++;

            if(count == 2) { /* get the next character and break */
                c = getc(file);
                break;
            }
        } while(1);

        fclose(file);

        if((c == 'R' || c == 'S') && R_FLAG) {
            printf("[%d] ", job[i].number);
            printf("Running ");
            display_args(job[i].argv);
            printf("[%d]\n", job[i].pid);
            continue;
        }

        if(c == 'T' && S_FLAG) {
            printf("[%d] ", job[i].number);
            printf("Stopped ");
            display_args(job[i].argv);
            printf("[%d]\n", job[i].pid);
        }
    }

    return 0;
}

int cmd_jobs(int argc, char** argv) {
    optind = 1; // Reset to 1 for reading

    int opt;
	int R_FLAG, S_FLAG;
    R_FLAG = S_FLAG = 0;

	while((opt = getopt(argc, argv, "rs")) != -1) {
		switch(opt) {
			case 'r': R_FLAG = 1; break;
			case 's': S_FLAG = 1; break;
			case '?':
				// Invalid Option (since our options don't take arguments)
				fprintf(stderr, "jobs: invalid option -- \'%c\'\n", optopt);
				return 0; // Don't proceed further
		}
	}

    if(optind != argc) {
        fprintf(stderr, "jobs: too many arguments\n");
        return 0;
    }

    if(R_FLAG == 0 && S_FLAG == 0)  /* Display all jobs */
        R_FLAG = S_FLAG = 1;
    display_jobs(R_FLAG, S_FLAG);
}

int cmd_sig(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "sig: invalid number of arguments\n");
        return 0;
    }

    long int number, signum; 
    char* end_ptr;

    number = strtol(argv[1], &end_ptr, 10);
    int job_index;

    if((end_ptr == argv[1]) || (*end_ptr != '\0') || (job_index = search_job_number(number)) < 0 ){ // Invalid
        fprintf(stderr, "sig: invalid job number \'%s\'\n", argv[1]);
        return 0;
    }

    signum = strtol(argv[2], &end_ptr, 10);

    if((end_ptr == argv[2]) || (*end_ptr != '\0')) { // Invalid
        fprintf(stderr, "sig: invalid signal number \'%s\'\n", argv[2]);
        return 0;
    }

    if( kill(job[job_index].pid, signum) < 0)
        perror("sig");

    return 0;
}

int cmd_fg(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "fg: invalid number of arguments\n");
        return 0;
    }

    long int number; 
    char* end_ptr;

    number = strtol(argv[1], &end_ptr, 10);
    int job_index;

    if((end_ptr == argv[1]) || (*end_ptr != '\0') || (job_index = search_job_number(number)) < 0 ){ // Invalid
        fprintf(stderr, "fg: invalid job number \'%s\'\n", argv[1]);
        return 0;
    }

    if(getpgid(job[job_index].pid) == shell_pid) /* Make new process group for this process (if not already) */
        setpgid(job[job_index].pid, 0);
    
    pid_t pid = job[job_index].pid; 
    int status;
    
    job[job_index].job_type = FG;
    
    /* ignore sigttin and sigttou otherwise shell process will be stopped */
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTTOU,SIG_IGN);

    /* give control to process pid */
    tcsetpgrp(STDIN_FILENO, pid);
    kill(pid, SIGCONT);
    
    waitpid(pid, &status, WUNTRACED);

    /* Set control back to shell process */
    tcsetpgrp(STDIN_FILENO, shell_pid);

    /* Set behaviour to default */
    signal(SIGTTIN, SIG_DFL);
    signal(SIGTTOU, SIG_DFL);

    /* Check status */
    if(WIFSTOPPED(status))
        job[job_index].job_type = ST;
    else if(WIFEXITED(status) || WIFSIGNALED(status))
        clear_job(job_index);

    return 0;
}

int cmd_bg(int argc, char** argv) {
    if(argc != 2) {
        fprintf(stderr, "bg: invalid number of arguments\n");
        return 0;
    }

    long int number; 
    char* end_ptr;

    number = strtol(argv[1], &end_ptr, 10);
    int job_index;

    if((end_ptr == argv[1]) || (*end_ptr != '\0') || (job_index = search_job_number(number)) < 0 ){ // Invalid
        fprintf(stderr, "bg: invalid job number \'%s\'\n", argv[1]);
        return 0;
    }

    pid_t pid = job[job_index].pid; 
    job[job_index].job_type = BG;  /* Set to Running as BG */
    kill(pid, SIGCONT);
    return 0;
}

const char* CMDS[CMD_SIZE] = {
    "exit", "pwd", "echo", "cd", "ls", "repeat", "jobs", "sig", "fg", "bg"
};

int (*func[])(int, char**) = {
    &cmd_exit, &cmd_pwd, &cmd_echo, &cmd_cd, &ls,  &repeat, &cmd_jobs, &cmd_sig, &cmd_fg, &cmd_bg
};

// Parses given line into words
char** parse(char** line) {

    const char* delim = " \t\r\a\n"; char* save_ptr;
    int bufsize = TOKEN_BUFFER_SIZE;
    char* token = strtok_r(*line, delim, &save_ptr);
    char** argv; // To store each token in the line

    int count = 0; // To count argc
    argv = malloc(bufsize * sizeof(char*));

    while(token != NULL) {
        argv[count] = malloc(strlen(token) + 1);
        strcpy(argv[count], token);
        count++;

        if(count >= bufsize) {
            bufsize += TOKEN_BUFFER_SIZE;
            argv = realloc(argv, bufsize * sizeof(char*));
            if(argv == NULL) {
                perror("Allocation Error");
                exit(1);
            }
        }
        token = strtok_r(NULL, delim, &save_ptr);
    }

    argv[count] = NULL;
    return argv;
}

// To find the count of arguments
int args_count(char** argv) {

    int count = 0;
    while(argv[count] != NULL) count++;

    return count;
}

// To rearrange arguments (place NULL arguments at the end)
// We know argv[argc] = NULL (final NULL character for parsed command)
void rearrange(int argc, char** argv) {
    for(int i = 0; i < argc - 1; i++)
        if(argv[i] == NULL) {
            int k = i + 1;
            
            while(k < argc && argv[k] == NULL) k++;

            if(k == argc) // All NULL remaining
                return;

            argv[i] = argv[k];
            argv[k] = NULL;
        }
}

// Calls system command as foreground or background process
// option: FG(0) for fg process, BG(1) for bg process
int launch(char** argv, int option) {

    int status;
    pid_t pid = fork(), w;

    if(pid < 0) { perror("Fork"); return 1;}

    if(pid == 0) {

        if(option) setpgid(0, 0); /* Change group id of the child process (if bg) */
        
        execvp(argv[0], argv);
        fprintf(stderr, "%s: command not found\n", argv[0]);
        _exit(0);
    }
    
    /* Parent */
    int index = assign_job(pid, argv, option);
    
    if(option)     /* Print pid of bg process and assign job */
        printf("%d\n", pid);

    else {
        w = waitpid(pid, &status, WUNTRACED);

        if(WIFSTOPPED(status))
            job[index].job_type = ST;
        else if(WIFEXITED(status) || WIFSIGNALED(status))
            clear_job(index);
    }

    return 0;
}

int execute(char** argv) {
    
    if(argv[0] == NULL) return 0;

    // For builtin (user-defined) commands
    for(int i = 0; i < CMD_SIZE; i++) {
        if (strcmp(CMDS[i], argv[0]) == 0) {
            return (func[i])(args_count(argv), argv);
        }
    }

    // system command
    return launch(argv, FG);
}

void display_operator_error(const char* operator) {
    fprintf(stderr, "syntax error near unexpected token `%s\'\n", operator);
}

// Returns number of pipes found in the input
// Returns -1 for invalid input
int check_pipe(char** argv) {
    int NUMBER_OF_PIPES = 0;

    for(int i = 0; argv[i] != NULL; i++) {
        if(!strcmp(argv[i], "|")) {
            if(i == 0 || argv[i - 1] == NULL ) {
                display_operator_error("|");
                return -1;
            } else if(argv[i + 1] == NULL) {
                display_operator_error("newline");
                return -1;
            }
            
            NUMBER_OF_PIPES++;
            argv[i] = NULL;
        }
    }

    return NUMBER_OF_PIPES;
}

// Opens file `file` and returns file descriptor
// Returns -1 if fails
int open_file(char* file, int flags, mode_t mode) {
    
    char* str = make_from_home(file);
    int fd = open(str, flags, mode);
    
    if(fd == -1) return -1;
    
    if(file[0] == '~') path_free(str); /* frees str */

    return fd;
}

// Displays error if open() returns -1
void display_open_file_error(char* file) {
    perror(file);
}

int redirect(char** argv, struct REDIRECTION* files) {

    int in_fd, out_fd, append_fd, write_fd;

    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);
    
    if(files->in_file) {
        in_fd = open_file(files->in_file, O_RDONLY, 0);
        if(in_fd == -1) {
            display_open_file_error(files->in_file);
            return 0;
        }

        dup2(in_fd, STDIN_FILENO);
        close(in_fd);
    }

    if(files->out_file) {
        out_fd = open_file(files->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if(out_fd == -1) {
            display_open_file_error(files->out_file);
            return 0;
        }

        if(files->write_file == OUT) dup2(out_fd, STDOUT_FILENO);
        
        close(out_fd);
    }

    if(files->append_file) {
        append_fd = open_file(files->append_file, O_WRONLY | O_APPEND, 0);
        if(append_fd == -1) {
            // Open in create mode
            append_fd = open_file(files->append_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if(append_fd == -1) {
                display_open_file_error(files->append_file);
                return 0;
            }
        }

        if(files->write_file == APPEND) dup2(append_fd, STDOUT_FILENO);

        close(append_fd);
    }

    int status = execute(argv); /* execute command in argv */

    dup2(save_stdin, STDIN_FILENO); close(save_stdin);
    dup2(save_stdout, STDOUT_FILENO); close(save_stdout);

    return status;
}

// Returns -1 for invalid Input
// Returns 0 for correct input (and rearranges arguments in argv)
// Files (for redirection) stored in `files` REDIRECTION struct
int check_redirection(char** argv, struct REDIRECTION* files) {

    int i; // Will store the count of arguments (to rearrange arguments)

    for(i = 0; argv[i] != NULL; i++) {

        if( !strcmp(argv[i], "<") || !strcmp(argv[i], ">") || !strcmp(argv[i], ">>") ) {
            
            if(argv[i + 1] == NULL) {
                display_operator_error("newline");
                return -1;
            }
            
            // Store argv[i + 1] in the redirection struct
            if(argv[i][0] == '<') files->in_file = argv[i + 1];
            
            else if (!strcmp(argv[i], ">")) {
                files->out_file = argv[i + 1];
                files->write_file = OUT;
            }
            else {
                files->append_file = argv[i + 1];
                files->write_file = APPEND;
            }
            argv[i] = NULL;
            argv[i + 1] = NULL;

            i++;
        }
    }
    
    rearrange(i, argv);
    return 0;
}

// Process redirection (Checks if redirection is required and executes it if yes)
// Returns 1 stop shell
// 0 continue shell
// -1 for redirection not required
int process_redirection(char** argv) {
    
    /* Check redirection */
    struct REDIRECTION files;
    files.in_file = files.out_file = files.append_file = NULL;
    files.write_file = UNDEF;

    int redirection_flag = check_redirection(argv, &files);
    if(redirection_flag == -1) return 0;

    if(files.in_file || files.out_file || files.append_file)
        return redirect(argv, &files); // Function to execute redirection
    
    return -1; // Redirection not required
}

// Executes pipelining for given input
// there will be number_of_pipes intermidate NULLs in argv separating the piped commands
int pipeline(int number_of_pipes, char** argv) {
    
    int i = number_of_pipes + 1;

    int save_stdin = dup(STDIN_FILENO);
    int save_stdout = dup(STDOUT_FILENO);

    int arg_index = 0; int status = 0;

    while(i != 0) {

        int pipefd[2];

        if(pipe(pipefd) < 0 || status) {
            perror("Pipe");
            dup2(save_stdout, STDOUT_FILENO);
            close(save_stdout);
            dup2(save_stdin, STDIN_FILENO);
            close(save_stdin);
            return 1;
        }

        if(i == 1) { /* Last Pipeline */
            dup2(save_stdout, STDOUT_FILENO);
            close(save_stdout);
        } else {
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[1]);
        }

        // Execute the command in argv
        int check_redirection = process_redirection(argv + arg_index);
        if(check_redirection < 0) /* Execute basic command */
            status |= execute(argv + arg_index);
        else 
            status |= check_redirection;

        if(i != 1) { // Move to next command
            while(argv[arg_index] != NULL) arg_index++;
            while(argv[arg_index] == NULL) arg_index++;
        }

        if(i == 1) { /* Last Pipeline */
            dup2(save_stdin, STDIN_FILENO);
            close(save_stdin);
        } else {
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
        }

        i--;
    }

    return status;
}

// Return -1 for invalid input
// Else return the number of background processes to run
int check_background(char** argv) {

    int NUMBER_OF_BG_PROCESSES = 0;

    for(int i = 0; argv[i] != NULL; i++) {
        if(!strcmp(argv[i], "&")) {
            if(i == 0 || argv[i - 1] == NULL ) {
                display_operator_error("&");
                return -1;
            }
            argv[i] = NULL;
            NUMBER_OF_BG_PROCESSES++;
        }
    }

    return NUMBER_OF_BG_PROCESSES;
}

// Function to execute background processes and/or simple commands
int handle_bg(int NUMBER_OF_BG_PROCESSES, char** argv) {

    int index = 0, status;
    while(NUMBER_OF_BG_PROCESSES) {
        
        status = launch(argv + index, BG);
        if(status) return 1;

        while(argv[index] != NULL) index++; /* To Point to the next process' arguments */

        // Skip this NULL and decrement number of bg processes
        index += 1; 
        NUMBER_OF_BG_PROCESSES--;
    }

    return execute(argv + index);  /* Execute the simple command */
}

// Executes each line of input
int execute_line(char** line) {

    char** argv =  parse(line); /* parse line */
    
    if(argv[0] == NULL) return 0;
    
    int NUMBER_OF_PIPES = check_pipe(argv);
    if(NUMBER_OF_PIPES == -1) return 0;

    if(NUMBER_OF_PIPES > 0)
        return pipeline(NUMBER_OF_PIPES, argv);
        
    int redirection_output = process_redirection(argv);
    if(redirection_output != -1) return redirection_output;
    
    /* Check for bg process */
    int NUMBER_OF_BG_PROCESSES = check_background(argv);

    if(NUMBER_OF_BG_PROCESSES == -1) return 0;

    return handle_bg(NUMBER_OF_BG_PROCESSES, argv); /* Will take care of simple commands as well */
}

int repeat(int argc, char** argv) {
    if(argc < 3) {
        fprintf(stderr, "repeat: less arguments\n");
        return 0;
    }

    // Convert 2nd argument to numeric type
    long int val; char* end_ptr;
    val = strtol(argv[1], &end_ptr, 10);

    if((end_ptr == argv[1]) || (*end_ptr != '\0')){ // Invalid
        fprintf(stderr, "repeat: invalid argument \'%s\'\n", argv[1]);
        return 0;
    }

    // Execute the given command
    for(long int i = 1; i <= val; i++) {
        if(execute(argv + 2) == 1) return 1;
    }
    return 0;
}