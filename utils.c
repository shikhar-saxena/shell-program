#include "headers.h"
#include "config.h"

// Concatenates relative path "rel_path" at the end of absolute path "abs_path"
// Returns the new path respectively
char* make_path(char* abs_path, size_t abs_path_size, char* rel_path, size_t rel_path_size) {

    // Get size for new path (to be made)
    size_t path_size = abs_path_size + rel_path_size + 1; // 1 to accomadate '/' in between the concatenation
    char* path = malloc(path_size + 1); // For '\0'
    if(path == NULL) {
        perror("Allocation Error");
        return NULL; // Failure
    }

    strncpy(path, abs_path, abs_path_size + 1);
    strcat(path, "/");
    strncat(path, rel_path, rel_path_size + 1);

    return path;
    // path will be freed after use (by calling path_free function)
}

// make absolute path from home (for relative path)
char* make_from_home(char* filename) {
    if(filename[0] == '~') 
        return make_path(home, home_size, (filename + 1), strlen(filename) - 1);
    
    else return filename;
}

// Deallocates memory
void path_free(char* str) {
    if(str != NULL) free(str);
}