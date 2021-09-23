#include "headers.h"
#include "config.h"
#include "cmd.h"

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

// read-eval-process-loop for shell
// Returns 1 on failure (terminate shell)
int shell_loop() {

    // Status returned by the commands executed 
    // status = 1 to terminate the shell loop
    int status = 0; 

    do {
        display_prompt();

        // Read
        char* read_buffer; size_t buffer_size = 0;
        
        if(getline(&read_buffer, &buffer_size, stdin) == -1) {
            if(!feof(stdin)) {
                // Error has occurred
                perror("Error");
                free(read_buffer);
                return 1;
            }
        }
        
        // Tokenize the input
        char* token = strtok(read_buffer, ";\n");
        while(token != NULL) {

            // Subtokenize each input command
            char* sub_token = strtok(token, " \t\r\a\n");
            char** argv; // To store each token in the command

            int count = 0; // To count argc
            argv = malloc(TOKEN_BUFFER_SIZE * sizeof(char*));

            while(sub_token != NULL) {
                argv[count] = malloc(strlen(sub_token) + 1);
                strcpy(argv[count], sub_token);
                count++;

                //!!! Realloc Part???

                sub_token = strtok(NULL, " \t\r\a\n");
            }

            // Execute the command
            status = execute(count, argv);

            // Free argv
            while (count-- != 0)
                free(argv[count]);
            free(argv);
            token = strtok(NULL, ";\n");
        }

        free(read_buffer);
        read_buffer = NULL;
    } while(status == 0);

    return status;
}

void main(int argc, char const *argv[])
{
    if(init() || shell_loop())
        terminate();
}