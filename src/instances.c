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
static char * instances_directory = 0;

void new_instance(const char * given_directory, const char * process_name) {
    unsigned int count = 0;
    unsigned int last_id = 0;

    DIR * dr = 0;
    struct dirent *de;

    int processes_count = count_process(process_name);
    expand_user(&instances_directory, given_directory);
    char * instance_filename_buffer = malloc(sizeof(char) * 128); 
    explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
    char string_representation_of_integer[4] = {0};
    FILE * new_instance = 0;
    if (stat(instances_directory, &st) == -1) {
        // Permission error
        if (errno == EACCES) {
            printf("%s: %s\n", instances_directory, strerror(errno));
            exit(-1);
        }
        recursive_mkdir(instances_directory);
    }
    if (processes_count > 1) {
        count_instances(instances_directory, &count, &last_id);
        strcat(instance_filename_buffer, instances_directory);
        strcat(instance_filename_buffer, "/");
        sprintf(string_representation_of_integer,"%d", last_id+1);
        strcat(instance_filename_buffer, string_representation_of_integer);
        new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        exit(0);
    }
    else {
        // Clear instances caches
        dr = opendir(instances_directory);
        while ((de = readdir(dr)) != NULL) {
            strcat(instance_filename_buffer, instances_directory);
            strcat(instance_filename_buffer, "/");
            strcat(instance_filename_buffer, de->d_name);
            if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
                remove(instance_filename_buffer);
            }
            explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
        }
        strcat(instance_filename_buffer, instances_directory);
        strcat(instance_filename_buffer, "/1");
        FILE * new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
    }
    free(instance_filename_buffer);
}

void * update_instances(void * args) {
        dr = opendir(instances_directory);
        while ((de = readdir(dr)) != NULL) {
            printf("%d\n", de->d_name);
        }
}
