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
//~ #include "voices.h"
#include "buttons.h"
//~ #include "text_pointer.h"

static struct stat st = {0};

InstanceContext * g_current_active_instance = 0; 

Instances g_instances = {0};

UIInteractiveList g_ui_instances = {0};

//~ InteractiveListContext g_instances_list_context = {
    //~ select_instance_from_list,
    //~ get_instance_target_name,
    //~ update_instances_text,
    //~ &g_ui_instances,
    //~ DSTUDIO_RENDER_INSTANCES
//~ };

static char * s_instances_directory = 0;

void exit_instances_management_thread() {
    sem_wait(&g_instances.thread_control.mutex);
    char instance_path[128] = {0};
    sprintf(instance_path, "%s/%d", s_instances_directory, g_current_active_instance->identifier);
    unlink(instance_path);
    sem_post(&g_instances.thread_control.mutex);
}


//~ char * get_instance_target_name(unsigned int index) {
    //~ return g_instances.contexts[g_ui_instances.window_offset+index].name;
//~ }

void init_instances_management_thread(
    UIElements * lines,
    unsigned int lines_number,
    unsigned int string_size
) {
    init_interactive_list(
        &g_ui_instances,
        lines,
        lines_number,
        string_size,
        sizeof(InstanceContext),
        &g_instances.count,
        (char **) &g_instances.contexts,
        &g_instances.thread_control
    );
    sem_init(&g_instances.thread_control.mutex, 0, 1);
    g_instances.thread_control.ready = 1;
}

void * instances_management_thread(void * args) {
    (void) args;
    int fd = 0;
    int wd = 0;
    InstanceContext * saved_contexts = 0;
    
    while(!g_instances.thread_control.ready) {
        usleep(1000);
    }

    fd = inotify_init();
    if (errno != 0 && fd == -1) {
        printf("inotify_init(): failed\n");
        exit(-1);
    }
    
    wd = inotify_add_watch(fd, s_instances_directory, IN_CREATE | IN_DELETE);
    if (errno != 0 && wd == 0) {
        printf("inotify_add_watch(): failed\n");
        exit(-1);
    }

	struct inotify_event * event = dstudio_alloc(sizeof(struct inotify_event) + 16 + 1);

    while(1) {
        if(read(fd, event, sizeof(struct inotify_event) + 16 + 1) < 0 && errno != 0) {
            continue;
        }
        sem_wait(&g_instances.thread_control.mutex);
        if (g_instances.thread_control.cut_thread) {
            sem_post(&g_instances.thread_control.mutex);
            break;
        }
        
		if (event->mask == IN_CREATE) {
            //clear_text_pointer();
            g_instances.count++;
            saved_contexts = g_instances.contexts;
            g_instances.contexts = dstudio_realloc(g_instances.contexts, sizeof(InstanceContext) * g_instances.count);
            if (g_instances.contexts == NULL) {
                g_instances.contexts = saved_contexts;
                g_instances.count--;
                sem_post(&g_instances.thread_control.mutex);
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
                g_ui_instances.update_request = -1;
            }
            else {
                g_ui_instances.update_request = g_instances.count - 1;
            }
            //new_voice();
            g_instances.thread_control.update = 1;
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
        sem_post(&g_instances.thread_control.mutex);
    }
    dstudio_free(event);
    return NULL;
}

void new_instance(const char * given_directory, const char * process_name) {
    unsigned int count = 0;
    unsigned int last_id = 0;

    DIR * dr = 0;
    struct dirent *de;

    int processes_count = count_process(process_name);
    expand_user(&s_instances_directory, given_directory);
    char * instance_filename_buffer = dstudio_alloc(sizeof(char) * 128); 
    char string_representation_of_integer[4] = {0};
    FILE * new_instance = 0;
    if (stat(s_instances_directory, &st) == -1) {
        // Permission error
        if (errno == EACCES) {
            printf("%s: %s\n", s_instances_directory, strerror(errno));
            exit(-1);
        }
        recursive_mkdir(s_instances_directory);
    }
    if (processes_count > 1) {
        count_instances(s_instances_directory, &count, &last_id);
        strcat(instance_filename_buffer, s_instances_directory);
        strcat(instance_filename_buffer, "/");
        sprintf(string_representation_of_integer,"%d", last_id+1);
        strcat(instance_filename_buffer, string_representation_of_integer);
        new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        exit(0);
    }
    else {
        // Clear instances caches
        dr = opendir(s_instances_directory);
        while ((de = readdir(dr)) != NULL) {
            strcat(instance_filename_buffer, s_instances_directory);
            strcat(instance_filename_buffer, "/");
            strcat(instance_filename_buffer, de->d_name);
            if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
                remove(instance_filename_buffer);
            }
            explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
        }
        closedir(dr);
        strcat(instance_filename_buffer, s_instances_directory);
        strcat(instance_filename_buffer, "/1");
        FILE * new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        g_instances.contexts = dstudio_alloc( sizeof(InstanceContext) );
        g_instances.count +=1;
        g_instances.contexts[0].identifier = 1;
        g_current_active_instance = &g_instances.contexts[0];
        strcpy(g_current_active_instance->name, "Instance 1");
        g_instances.thread_control.update = 1;

        //new_voice();
    }
    dstudio_free(instance_filename_buffer);
}

//~ void scroll_instances(void * args) {
    //~ unsigned int flags = *((unsigned int *) args);
    //~ sem_wait(&g_ui_instances.mutex);
    //~ if (g_ui_instances.window_offset > 0 && flags && DSTUDIO_BUTTON_ACTION_LIST_BACKWARD) {
        //~ g_ui_instances.window_offset--;
        //~ g_ui_instances.update = 1;
    //~ }
    //~ else if (g_ui_instances.window_offset + g_ui_instances.max_lines_number < g_instances.count && !(flags & DSTUDIO_BUTTON_ACTION_LIST_BACKWARD)) {
        //~ g_ui_instances.window_offset++;
        //~ g_ui_instances.update = 1;
    //~ }
    //~ sem_post(&g_ui_instances.mutex);
//~ }

//~ void select_instance_from_list(
    //~ unsigned int index
//~ ) {
    //~ index += g_ui_instances.window_offset; 
    //~ if (index != g_instances.index && index < g_instances.count) {
        //~ update_current_instance(index);
        //~ update_insteractive_list_shadow(
            //~ DSTUDIO_CONTEXT_INSTANCES,
            //~ &g_ui_instances
        //~ );
    //~ }
//~ }

//~ void update_current_instance(unsigned int index) {
    //~ sem_wait(&g_ui_instances.mutex);
    //~ g_instances.index = index;
    //~ g_current_active_instance = &g_instances.contexts[index];
    //~ sem_post(&g_ui_instances.mutex);
//~ }

//~ void update_instances_text() {
    //~ update_insteractive_list_shadow(
        //~ DSTUDIO_CONTEXT_INSTANCES,
        //~ &g_ui_instances
    //~ );
    //~ update_insteractive_list(
        //~ DSTUDIO_CONTEXT_INSTANCES,
        //~ g_ui_instances.window_offset,
        //~ g_ui_instances.max_lines_number,
        //~ g_instances.count,
        //~ g_ui_instances.lines,
        //~ g_ui_instances.string_size, 
        //~ &g_instances.contexts[0]
    //~ );
//~ }

void update_instances_ui_list() {
    update_insteractive_list(&g_ui_instances);
}
