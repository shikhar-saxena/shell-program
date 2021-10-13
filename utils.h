#ifndef __UTILS_H
#define __UTILS_H

// Converts relative path to absolute path
char* make_path(char* abs_path, size_t abs_path_size, char* rel_path, size_t rel_path_size);

char* make_from_home(char* filename);

void path_free(char* str);

#endif