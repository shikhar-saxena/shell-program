#include "headers.h"
#include "config.h"

// Make absolute path for given relative path 'path'
// Relative path with respect to home directory
// Returns the new path
char* make_path(char* path) {
    
    // path will begin with '~'
    size_t str_size = home_size + strlen(path);
    char* str = malloc(str_size);
    if(str == NULL) {
        perror("Allocation Error");
        exit(EXIT_FAILURE);
    }

    size_t i;
    for(i = 0; i < home_size; i++) 
        str[i] = home[i];

    for(; i < str_size; i++) {
        str[i] = path[i - home_size + 1];
    }

    return str;
    // str will be freed after use (by calling utils_free function)
}

// Deallocates memory
void str_free(char* str) {
    free(str);
}