#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "fileutils.h"
#include "instances.h"

static struct stat st = {0};

void new_instance(const char * given_directory) {
    char * directory = 0;
    expand_user(&directory, given_directory);
    
    if (stat(directory, &st) == -1) {
        if (errno == EACCES) { // Permission error
            printf("%s: %s\n", directory, strerror(errno));
            exit(-1);
        }
        recursive_mkdir(directory);
    }
}
