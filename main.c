#include "headers.h"
#include "config.h"
#include "exec.h"
#include "utils.h" //!!!!

#define TOKEN_BUFFER_SIZE 100 // Number of arguments read in each line

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
}

// Parses given line into words
char** parse(char** line) {

    const char* delim = " \t\r\a\n"; char* save_ptr;
    char* token = strtok_r(*line, delim, &save_ptr);
    char** argv; // To store each token in the line

    int count = 0; // To count argc
    argv = malloc(TOKEN_BUFFER_SIZE * sizeof(char*));

    while(token != NULL) {
        argv[count] = token;
        count++;
        token = strtok_r(NULL, delim, &save_ptr);
    }

    argv[count] = NULL;
    return argv;
}

//!!!!!!
int args_count(char** argv) {

    int count = 0;
    while(argv[count] != NULL) count++;

    return count;
}

// int redirection(int argc, char** argv) {

//     int status;

//     for(int i = 0; i < argc; i++) {
//         if(!strcmp(argv[i], "<") || !strcmp(argv[i], ">") || !strcmp(argv[i], ">>")) {
            
//             if(i == argc - 1) {
//                 fprintf(stderr, "syntax error near unexpected token `newline\'\n");
//                 return 0;
//             }

//             char* path = (argv[i+1][0] == '~') ? make_path(home, home_size, (argv[i+1] + 1), strlen(argv[i+1]) - 1) : argv[i+1];
//             int fd; 
            
//             int option = 1; // STDOUT_FILENO
            
//             if(argv[i][0] == '<') {
//                 fd = open(path, O_RDONLY);
//                 option = 0;
//             }
//             else if(strcmp(argv[i], ">") == 0)
//                 fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
//             else
//                 fd = open(path, O_WRONLY | O_APPEND);

//             argv[i] = NULL;

//             if(fd == -1)
//                 perror(argv[i+1]);
//             else if(dup2(fd, option) < 0)
//                 perror("Unable to duplicate file descriptor");
//             else {
//                 // Execute commands before redirection character
//                 status = execute(i, argv);
//                 dup2(option, option); // Revert back the file descriptors
//             }

//             if(argv[i+1][0] == '~') path_free(path);
//             return status;
//         }
//     }

//     return execute(argc, argv);
// }

// Separates line into first and second operands (separated by character `character`)
// Returns 1 if operator (character) not present
// Else returns 0
int check_character(char** line, char** first, char** second, const char* character) {

    char* ptr = NULL;
    for(int i = 0; i < 2; i++) {
        ptr = strsep(line, character);

        if(ptr == NULL) break;
        if(i == 0) *first = ptr;
        else *second = ptr;
    }

    if(*second == NULL) return 1; // Operator not found
    else return 0;
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
        
        if(getline(&read_buffer, &buffer_size, stdin) == -1 && !feof(stdin)) {
            perror("Error");
            free(read_buffer);
            return 1;
        }
        
        // Tokenize the input into lines
        char* save_ptr = NULL;
        char* token = strtok_r(read_buffer, ";\n", &save_ptr);
        
        for(;token != NULL; token = strtok_r(NULL, ";\n", &save_ptr)) {

            char* first = NULL;
            char* second = NULL;

            // Pipeline Check
            if(!check_character(&token, &first, &second, "|")) {
                
                if(first == NULL || strlen(first) == 0) {
                    fprintf(stderr, "syntax error near unexpected token `|\'\n");
                    continue;
                }

                int pipefd[2];
                if(pipe(pipefd) < 0) {
                    perror("Pipe");
                    continue;
                }
                    
                // Parse first argument
                char** args1 = parse(&first);
                char** args2 = parse(&second); 
                
                pid_t pid = fork();
                if(pid < 0) {
                    perror("Fork");
                } else if (pid == 0) { // Child writes to pipe
                    close(pipefd[0]);
                    close(STDIN_FILENO); // Don't want to read from stdin
                    dup2(pipefd[1], STDOUT_FILENO); close(pipefd[1]);
                    status = execute(args_count(args1), args1);
                    close(STDOUT_FILENO);
                    _exit(0);
                } else { // Parent reads from pipe

                    close(pipefd[1]);
                    int save_fd = dup(STDIN_FILENO);
                    dup2(pipefd[0], STDIN_FILENO); close(pipefd[0]);
                    status = execute(args_count(args2), args2);
                    close(STDIN_FILENO);
                    dup2(save_fd, STDIN_FILENO); close(save_fd);
                    
                    // wait(NULL);

                    if(wait(&status) == -1) perror("Error");
                    // pid_t pid2 = fork();
                    // if(pid2 < 0) {
                    //     perror("Fork");
                    // } else if (pid2 == 0) { // Child reads from pipe
                    //     close(pipefd[1]);
                    //     dup2(pipefd[0], STDIN_FILENO); close(pipefd[0]);
                    //     status = execute(args_count(args2), args2);
                    //     close(STDIN_FILENO);
                    //     close(STDOUT_FILENO);
                    //     _exit(0);
                    // } else { // Parent waits for both processes
                    //     if(waitpid(pid2, &status, 0) == -1) perror("Error");
                    // }
                }

                free(args1); free(args2);                    
            }

            else {
                // Parse first
                char** args = parse(&first);
                status = execute(args_count(args), args);
                free(args);
            }

            // Redirection Check
            // redirection(token);

            // status = eval(token);
            
            
        }

        free(read_buffer);

    } while(status == 0);

    return status;
}

void main(int argc, char const *argv[])
{
    if(init() || shell_loop())
        terminate();
}