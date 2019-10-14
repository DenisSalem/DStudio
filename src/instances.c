#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>

#include <sys/stat.h>
#include <unistd.h>

#include "common.h"
#include "fileutils.h"
#include "instances.h"
#include "ui.h"
#include "voices.h"

static struct stat st = {0};
InstanceContext * g_current_active_instance = 0; 
Instances g_instances = {0};
UIInteractiveList g_ui_instances = {0};

char * instances_directory = 0;

void exit_instances_thread() {
    sem_wait(&g_ui_instances.mutex);
    g_ui_instances.cut_thread = 1;    
    char * instance_path = malloc(sizeof(char) * 128);
    explicit_bzero(instance_path, sizeof(char) * 128);
    sprintf(instance_path, "%s/%d", instances_directory, g_current_active_instance->identifier);
    unlink(instance_path);
    sem_post(&g_ui_instances.mutex);

}

void init_instances_ui(UIElements * lines, unsigned int lines_number, unsigned int string_size) {
    init_interactive_list(&g_ui_instances, lines, lines_number, string_size);
}

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
        closedir(dr);
        strcat(instance_filename_buffer, instances_directory);
        strcat(instance_filename_buffer, "/1");
        FILE * new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        g_instances.contexts = malloc( sizeof(InstanceContext) );
        explicit_bzero(g_instances.contexts, sizeof(InstanceContext));
        DSTUDIO_EXIT_IF_NULL(g_instances.contexts)
        
        g_instances.count +=1;
        g_instances.contexts[0].identifier = 1;
        g_current_active_instance = &g_instances.contexts[0];
        strcpy(g_current_active_instance->name, "Instance 1");
        
        new_voice();
    }
    free(instance_filename_buffer);
}

void scroll_instances(unsigned int flags) {
    sem_wait(&g_ui_instances.mutex);
    if (g_ui_instances.window_offset > 0 && flags && DSTUDIO_BUTTON_ACTION_LIST_BACKWARD) {
        g_ui_instances.window_offset--;
        g_ui_instances.update = 1;
    }
    else if (g_ui_instances.window_offset + g_ui_instances.lines_number < g_instances.count && !(flags & DSTUDIO_BUTTON_ACTION_LIST_BACKWARD)) {
        g_ui_instances.window_offset++;
        g_ui_instances.update = 1;
    }
    sem_post(&g_ui_instances.mutex);
}

void * update_instances(void * args) {
    (void) args;
    int fd = 0;
    int wd = 0;
    InstanceContext * saved_contexts = 0;
    while(!g_ui_instances.ready) {
        usleep(1000);
    }
    
    fd = inotify_init();
    if (errno != 0 && fd == -1) {
        printf("inotify_init(): failed\n");
        exit(-1);
    }
    wd = inotify_add_watch(fd, instances_directory, IN_CREATE | IN_DELETE);
    if (errno != 0 && wd == 0) {
        printf("inotify_add_watch(): failed\n");
        exit(-1);
    }

	struct inotify_event * event = malloc(sizeof(struct inotify_event) + 16 + 1);

    while(1) {
        if(read(fd, event, sizeof(struct inotify_event) + 16 + 1) < 0 && errno != 0) {
            continue;
        }
        
        sem_wait(&g_ui_instances.mutex);
        if (g_ui_instances.cut_thread) {
            sem_post(&g_ui_instances.mutex);
            break;
        }
        
		if (event->mask == IN_CREATE) {
            g_instances.count++;
            saved_contexts = g_instances.contexts;
            g_instances.contexts = realloc(g_instances.contexts, sizeof(InstanceContext) * g_instances.count);
            
            if (g_instances.contexts == NULL) {
                g_instances.contexts = saved_contexts;
                g_instances.count--;
                sem_post(&g_ui_instances.mutex);
                // TODO SEND LOG TO GUI
                printf("New instance creation has failed.\n");
                continue;
            }
            
            explicit_bzero(&g_instances.contexts[g_instances.count-1], sizeof(InstanceContext));
            g_current_active_instance = &g_instances.contexts[g_instances.count-1];
            g_instances.index = g_instances.count - 1;
            g_current_active_instance->identifier = 1;
            strcat(g_current_active_instance->name, "Instance ");
            strcat(g_current_active_instance->name, event->name);
            if (g_instances.count > g_ui_instances.lines_number) {
                g_ui_instances.window_offset++;
            }
            new_voice();
            g_ui_instances.update = 1;
            
            send_expose_event();
            
            #ifdef DSTUDIO_DEBUG
			printf("Create instance with id=%s. Allocated memory is now %ld.\n", event->name, sizeof(InstanceContext) * g_instances.count);
            printf("Currents instances:\n");
            for(unsigned int i = 0; i < g_instances.count; i++) {
                printf("\t%s\n", g_instances.contexts[i].name);
            }
            #endif
        }
		
		else if (event->mask == IN_DELETE) {
            g_instances.count--;
            g_instances.index--;
            // TODO: Move context data in memory and then realloc
            #ifdef DSTUDIO_DEBUG
			printf("Remove instance with id=%s\n", event->name);
            #endif
        }
        sem_post(&g_ui_instances.mutex);
    }
    return NULL;
}

void update_instances_text() {
    update_insteractive_list(
        DSTUDIO_CONTEXT_INSTANCES,
        g_ui_instances.window_offset,
        g_ui_instances.lines_number,
        g_instances.count,
        g_ui_instances.lines,
        g_ui_instances.string_size, 
        &g_instances.contexts[0]
    );
}
