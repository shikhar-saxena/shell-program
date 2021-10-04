#include "headers.h"
#include "cmd.h"
#include "exec.h"

const char* CMDS[CMD_SIZE] = {
        "exit", "pwd", "echo", "cd", "ls", "repeat"
};

int (*func[])(int, char**) = {&cmd_exit, &cmd_pwd, &cmd_echo, &cmd_cd, &ls, &repeat};

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
        if(execute(argc - 2, (argv + 2)) == 1) return 1;
    }
    return 0;
}

// Launches in foreground
int launch_fg(int argc, char** argv) {
    int status;
    pid_t pid = fork(), w;

    if(pid < 0) { perror("Fork"); return 1;}

    if(pid == 0) { 
        if (execvp(argv[0], argv) == -1)
            perror(argv[0]);
        exit(0);
    }
    else do { 
        w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
        if(w == -1) { perror("waitpid"); exit(1); }

    } while(!WIFEXITED(status) && !WIFSIGNALED(status));

    return 0;
}

int execute(int argc, char** argv) {
    
    if(argc == 0) return 0;

    // For builtin (user-defined) commands
    for(int i = 0; i < CMD_SIZE; i++) {
        if (strcmp(CMDS[i], argv[0]) == 0) {
            return (func[i])(argc, argv);
        }
    }

    // system command
    return launch_fg(argc, argv);
}