#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileutils.h"

static struct passwd * pw = 0;
static struct stat st = {0};

void expand_user(char ** dest, const char * directory) {
    if (pw == 0) {
        pw = getpwuid(getuid());
    }
    char * tild_ptr = strrchr(directory, '~') + 1;
    *dest = malloc(sizeof(char) * (strlen(pw->pw_dir) + strlen(tild_ptr))+1);
    strcpy(*dest, pw->pw_dir);
    strcpy(&(*dest)[strlen(pw->pw_dir)], tild_ptr);
}

void recursive_mkdir(char * directory) {
    char * tmp_str = malloc(sizeof(char) * strlen(directory));
    int index = 0;
    int previous_index = 0;
    while (1) {
        for(int i = 0; i < strlen(directory); i++) {
            if (directory[i] == '/' && i > index) {
                strcpy(tmp_str, directory);
                tmp_str[i] = 0;
                index = i;
                break;
            }
        }
        if (stat(tmp_str, &st) == -1 && errno == ENOENT) { // File or directory not found.
                mkdir(tmp_str, 0700);
        }   

        if (previous_index == index) {
            break;
        }
        else {
            previous_index = index;
        }
    }
    free(tmp_str);
}
