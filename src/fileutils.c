#include <dirent.h>
#include <errno.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "fileutils.h"

//~ static struct passwd * pw = 0;
//~ static struct stat st = {0};
static double physical_memory_kib = 0;

//~ void count_instances(const char * directory, unsigned int * count, unsigned int * last_id) {
    //~ DIR * dr = 0;
    //~ struct dirent *de;
    //~ long long int filename_is_an_instance;
    
    //~ *last_id = 0;
    //~ *count = 0;
    //~ dr = opendir(directory);
    
    //~ while ((de = readdir(dr)) != NULL) {
        //~ filename_is_an_instance = strtol(de->d_name, NULL, 10);
        //~ if (filename_is_an_instance != 0) {
            //~ *count += 1;
            //~ if(filename_is_an_instance > *last_id) {
                //~ *last_id = filename_is_an_instance;
            //~ }
        //~ }
    //~ }
    //~ closedir(dr);
//~ }

//~ unsigned int count_process(const char * process_name) {
    //~ DIR * dr = 0;
    //~ struct dirent *de;
    //~ char * processus_status_path = malloc(sizeof(char) * 128);
    //~ char * line_buffer = 0;
    //~ size_t line_buffer_size = 0;
    //~ explicit_bzero(processus_status_path, sizeof(char) * 128);
    //~ int count = 0;
    //~ long long int process_id;
    //~ dr = opendir("/proc");
    //~ FILE * processus_status = 0;
    
    //~ int process_name_match = 0;
    //~ int process_uid_match = 0;
    
    //~ while ((de = readdir(dr)) != NULL) {
        //~ process_id = strtol(de->d_name, NULL, 10);
        //~ if (process_id != 0) {
            //~ strcat(processus_status_path, "/proc/");
            //~ strcat(processus_status_path, de->d_name);
            //~ strcat(processus_status_path, "/status");
            //~ processus_status = fopen(processus_status_path, "r");
            //~ if (0 != errno && processus_status == NULL) {
                //~ #ifdef DSTUDIO_DEBUG
                //~ printf("count_process(): %s: %s", processus_status_path, strerror(errno));
                //~ #endif
                //~ continue;
            //~ }
            //~ while(getline(&line_buffer, &line_buffer_size, processus_status) > 0) {
                //~ if (strncmp(line_buffer, "Name:", 5) == 0 ) {
                    //~ process_name_match = strncmp(process_name, line_buffer+6, strlen(process_name));
                //~ }
                //~ if (strncmp(line_buffer, "Uid:", 4) == 0 ) {
                    //~ process_uid_match = getuid() == (unsigned int) atoi(line_buffer+5);
                    //~ break;
                //~ }
            //~ }

            //~ if (process_name_match == 0 && process_uid_match) {
                //~ count+=1;
            //~ }

            //~ explicit_bzero(processus_status_path, sizeof(char) * 64);
            //~ fclose(processus_status);
        //~ }
        
    //~ }
    //~ closedir(dr);
    //~ free(processus_status_path);
    //~ free(line_buffer);
    //~ return count;
//~ }

//~ void expand_user(char ** dest, const char * directory) {
    //~ if (pw == 0) {
        //~ pw = getpwuid(getuid());
    //~ }
    //~ char * tild_ptr = strrchr(directory, '~') + 1;
    //~ *dest = malloc(sizeof(char) * (strlen(pw->pw_dir) + strlen(tild_ptr))+1);
    //~ strcpy(*dest, pw->pw_dir);
    //~ strcpy(&(*dest)[strlen(pw->pw_dir)], tild_ptr);
//~ }

double get_proc_memory_usage() {
    char processus_id[8] = {0};
    char processus_status_path[32] = {0};
    char * line_buffer = 0;
    size_t line_buffer_size = 0;
    FILE * processus_status = 0;
    
    sprintf(processus_id, "%d", getpid());
    strcat(processus_status_path, "/proc/");
    strcat(processus_status_path, processus_id);
    strcat(processus_status_path, "/status");
    
    processus_status = fopen(processus_status_path, "r");
    if (0 != errno && processus_status == NULL) {
        #ifdef DSTUDIO_DEBUG
        printf("get_proc_memory_usage(): %s: %s\n", processus_status_path, strerror(errno));
        #endif
        
        return -1;
    }
    while(getline(&line_buffer, &line_buffer_size, processus_status) > 0) {
        if (strncmp(line_buffer, "VmRSS:", 6) == 0 ) {
            strrchr(line_buffer, ' ')[0] = 0;
            fclose(processus_status);
            return atof(strpbrk(line_buffer, " ")) / physical_memory_kib * 100;
        }
    }
    fclose(processus_status);
    return -1;
}

//~ void recursive_mkdir(char * directory) {
    //~ char * tmp_str = malloc(sizeof(char) * strlen(directory));
    //~ int index = 0;
    //~ int previous_index = 0;
    //~ while (1) {
        //~ for(int i = 0; i < (int) strlen(directory); i++) {
            //~ if (directory[i] == '/' && i > index) {
                //~ strcpy(tmp_str, directory);
                //~ tmp_str[i] = 0;
                //~ index = i;
                //~ break;
            //~ }
        //~ }
        //~ if (stat(tmp_str, &st) == -1 && errno == ENOENT) { // File or directory not found.
                //~ mkdir(tmp_str, 0700);
        //~ }   

        //~ if (previous_index == index) {
            //~ mkdir(directory, 0700);
            //~ break;
        //~ }
        //~ else {
            //~ previous_index = index;
        //~ }
    //~ }
    //~ free(tmp_str);
//~ }

int set_physical_memory() {
    char * line_buffer = 0;
    size_t line_buffer_size = 0;
    FILE * meminfo_fd = 0;
    meminfo_fd = fopen("/proc/meminfo", "r");
    if (0 != errno && meminfo_fd == NULL) {
        #ifdef DSTUDIO_DEBUG
        printf("set_physical_memory(): /proc/meminfo: %s\n", strerror(errno));
        #endif
        return -1;
    }
    while(getline(&line_buffer, &line_buffer_size, meminfo_fd) > 0) {
        if (strncmp(line_buffer, "MemTotal:", 9) == 0 ) {
            strrchr(line_buffer, ' ')[0] = 0;
            physical_memory_kib = atof(strpbrk(line_buffer, " "));
            fclose(meminfo_fd);
            return 0;
        }
    }
    #ifdef DSTUDIO_DEBUG
    printf("set_physical_memory(): There no line matching with 'MemTotal:'.\n");
    #endif
    return -1;
}
