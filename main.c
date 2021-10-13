#include "headers.h"
#include "config.h"
#include "exec.h"

//-------------------------------------
// Prompt
//-------------------------------------

// Displays the prompt on stdout
void display_prompt() {

    printf("<%s@%s:", username, buf->nodename);

    // Check if pwd begins with home (relative path prompt)
    if(strncmp(home, pwd, home_size) == 0) {

        printf("~");

        // Print the rest of the path into prompt
        printf("%s>", pwd + home_size);
    }
    else printf("%s>", pwd);
    fflush(stdout);
}

void check_processes() {

    for(int i = 0; i < JOB_SIZE; i++) {
        if(job[i].number == -1) continue;

        int status; pid_t pid;

        pid = waitpid(job[i].pid, &status, WNOHANG | WUNTRACED | WCONTINUED);
        if(!pid) continue;
        

        if(WIFEXITED(status)) { /* Terminated normally */
            if(job[i].job_type == BG)
                fprintf(stderr, "\n%s with pid %d exited normally\n", job[i].argv[0], job[i].pid);
            clear_job(i);
        }

        else if(WIFSIGNALED(status)) { /* Terminated abnormally */
            if(job[i].job_type == BG)
                fprintf(stderr, "\n%s with pid %d exited abnormally\n", job[i].argv[0], job[i].pid);
            clear_job(i);
        }
        
        else if(WIFSTOPPED(status))
            job[i].job_type = ST;

        else if(WIFCONTINUED(status)) /* if bg command was called */
            job[i].job_type = BG;
    }
}

// /* ^C */
void sigint_handler(int signum) {

    signal(SIGINT, sigint_handler);


    pid_t pid;

    if((pid = fgpid())) {
        kill(SIGINT, pid);
        return;
    }

    if(getpid() == shell_pid) {
        printf("\n");
        display_prompt();
        fflush(stdout);
    }
}

// REPL for shell
// Returns 1 on failure (terminate shell)
int shell_loop() {

    // Status returned by the commands executed 
    // status = 1 to terminate the shell loop
    int status = 0; 

    do {
        display_prompt();

        // Read
        char* read_buffer = NULL; size_t buffer_size = 0;
        
        if(getline(&read_buffer, &buffer_size, stdin) == -1) {
            free(read_buffer);
            if(!feof(stdin)) perror("Error");
            else {
                printf("\n"); /* for ^D */
                kill_jobs();
            }
            terminate();
        }
        
        // Tokenize the input into lines
        char* save_ptr = NULL;
        char* token = strtok_r(read_buffer, ";\n", &save_ptr);
               
        for(; token != NULL; token = strtok_r(NULL, ";\n", &save_ptr) ) 
            if(status = execute_line(&token)) 
                break;

        check_processes();

        free(read_buffer);

    } while(status == 0);

    return status;
}

int main(int argc, char const *argv[])
{
    init_job();
    signal(SIGINT, sigint_handler); /* Ignore SIGINT for shell */
    if(init() || shell_loop()) 
        terminate();
    
    return 0;
}