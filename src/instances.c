#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "fileutils.h"
#include "instances.h"

static struct stat st = {0};

size_t get_filesize(char * filename) {
}

void new_instance(const char * given_directory, const char * process_name) {
    char * directory = 0;
    int file_count = 0;
    long long int filename_is_an_instance;
    int max_instance_id = 0;
    DIR * dr = 0;
    struct dirent *de;
    int instances_count = count_process(process_name);
    expand_user(&directory, given_directory);
    char * instance_filename_buffer = malloc(sizeof(char) * 128); 
    explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
    char string_representation_of_integer[4] = {0};
    FILE * new_instance = 0;
    if (stat(directory, &st) == -1) {
        // Permission error
        if (errno == EACCES) {
            printf("%s: %s\n", directory, strerror(errno));
            exit(-1);
        }
        recursive_mkdir(directory);
    }

    dr = opendir(directory);
    if (instances_count > 1) {
        while ((de = readdir(dr)) != NULL) {
            filename_is_an_instance = strtol(de->d_name, NULL, 10);
            if (filename_is_an_instance != 0 && filename_is_an_instance > max_instance_id) {
                max_instance_id = filename_is_an_instance;
            }
        }
        strcat(instance_filename_buffer, directory);
        strcat(instance_filename_buffer, "/");
        sprintf(string_representation_of_integer,"%d", max_instance_id+1);
        strcat(instance_filename_buffer, string_representation_of_integer);
        new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        exit(0);
    }
    else {
        // Clear instances caches
        while ((de = readdir(dr)) != NULL) {
            strcat(instance_filename_buffer, directory);
            strcat(instance_filename_buffer, "/");
            strcat(instance_filename_buffer, de->d_name);
            if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
                remove(instance_filename_buffer);
            }
            explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
        }
        strcat(instance_filename_buffer, directory);
        strcat(instance_filename_buffer, "/1");
        FILE * new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
    }
    free(instance_filename_buffer);
}
