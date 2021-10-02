#include "headers.h"
#include "config.h"
#include "exec.h"

#define TOKEN_BUFFER_SIZE 64 // Number of arguments read in each line

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

char** parse(char* line) {

    const char* delim = " \t\r\a\n"; char* save_ptr;
    char* token = strtok_r(line, delim, &save_ptr);
    char** argv; // To store each token in the line

    int count = 0; // To count argc
    argv = malloc(TOKEN_BUFFER_SIZE * sizeof(char*));

    while(token != NULL) {
        argv[count] = malloc(strlen(token) + 1);
        strcpy(argv[count], token);

        //!!! Realloc Part???

        count++;
        token = strtok_r(NULL, delim, &save_ptr);
    }

    argv[count] = NULL;
    return argv;
}

int args_count(char** argv) {

    int count = 0;
    while(argv[count] != NULL) count++;

    return count;
}

// read-eval-process-loop for shell
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
            if(!feof(stdin)) {
                perror("Error");
                free(read_buffer);
                return 1;
            }
        }
        
        // Tokenize the input into lines
        char* save_ptr;
        char* token = strtok_r(read_buffer, ";\n", &save_ptr);
        while(token != NULL) {

            char** argv = parse(token); // Call parse function

            // Execute the command
            int count = args_count(argv);
            status = execute(count, argv);

            // Free argv
            while (count-- != 0) 
                free(argv[count]);
            free(argv);

            token = strtok_r(NULL, ";\n", &save_ptr);
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