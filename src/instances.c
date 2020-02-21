/*
 * Copyright 2019, 2020 Denis Salem
 *
 * This file is part of DStudio.
 *
 * DStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DStudio. If not, see <http://www.gnu.org/licenses/>.
*/

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
#include "text_pointer.h"

static struct stat st = {0};

InstanceContext * g_current_active_instance = 0; 

Instances g_instances = {0};

UIInteractiveList g_ui_instances = {0};

static char * s_instances_directory = 0;

void add_instance_file_descriptor() {
    unsigned int count = 0;
    unsigned int last_id = 0;
    FILE * new_instance = 0;
    char * instance_filename_buffer = dstudio_alloc(sizeof(char) * 128); 
    char string_representation_of_integer[4] = {0};

    count_instances(s_instances_directory, &count, &last_id);
    strcat(instance_filename_buffer, s_instances_directory);
    strcat(instance_filename_buffer, "/");
    sprintf(string_representation_of_integer,"%d", last_id+1);
    strcat(instance_filename_buffer, string_representation_of_integer);
    new_instance = fopen(instance_filename_buffer, "w+");
    fclose(new_instance);
}

void exit_instances_management_thread() {
    sem_wait(&g_instances.thread_control.mutex);
    char instance_path[128] = {0};
    sprintf(instance_path, "%s/%d", s_instances_directory, g_current_active_instance->identifier);
    unlink(instance_path);
    sem_post(&g_instances.thread_control.mutex);
}

void init_instances_management_thread(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
) {
    init_interactive_list(
        &g_ui_instances,
        ui_elements,
        lines_number,
        string_size,
        sizeof(InstanceContext),
        &g_instances.count,
        g_instances.contexts->name,
        &g_instances.thread_control,
        select_instance_from_list,
        1,
        item_offset_y
    );
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
            clear_text_pointer();
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
                g_ui_instances.window_offset = g_instances.count - g_ui_instances.lines_number;
                g_ui_instances.update_request = -1;
            }
            else {
                g_ui_instances.update_request = g_instances.index;
            }
            select_item(
                &g_ui_instances.lines[g_instances.index-g_ui_instances.window_offset],
                DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
            );
            
            new_voice(DSTUDIO_DO_NOT_USE_MUTEX);

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
    sem_init(&g_instances.thread_control.mutex, 0, 1);
    DIR * dr = 0;
    struct dirent *de;

    int processes_count = count_process(process_name);
    expand_user(&s_instances_directory, given_directory);
    char * instance_filename_buffer = dstudio_alloc(sizeof(char) * 128); 
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
        add_instance_file_descriptor();
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
        new_instance = fopen(instance_filename_buffer, "w+");
        fclose(new_instance);
        g_instances.contexts = dstudio_alloc( sizeof(InstanceContext) );
        g_instances.count +=1;
        g_instances.contexts[0].identifier = 1;
        g_current_active_instance = &g_instances.contexts[0];
        strcpy(g_current_active_instance->name, "Instance 1");
        g_ui_voices.thread_bound_control = &g_voices_thread_control;
        g_voices_thread_control.shared_mutex = &g_instances.thread_control.mutex;
        g_instances.thread_control.update = 1;
        new_voice(DSTUDIO_DO_NOT_USE_MUTEX);
    }
    dstudio_free(instance_filename_buffer);
}

unsigned int select_instance_from_list(
    unsigned int index
) {
    unsigned int voice_index = 0;
    UIElements * line = NULL;
    
    if (index != g_instances.index && index < g_instances.count) {
        update_current_instance(index);
        voice_index = g_instances.contexts[index].voices.index;
        if (voice_index < g_ui_voices.lines_number) {
            line = &g_ui_voices.lines[voice_index];
            g_ui_voices.window_offset = 0;
        }
        else {
            line = &g_ui_voices.lines[g_ui_voices.lines_number-1];
            g_ui_voices.window_offset = voice_index - g_ui_voices.lines_number + 1;
        }
        bind_voices_interactive_list(line);
        return 1;
    }
    return 0;
}

void update_current_instance(unsigned int index) {
    g_instances.index = index;
    g_current_active_instance = &g_instances.contexts[index];
}

void update_instances_ui_list() {
    g_ui_instances.source_data = g_instances.contexts->name;
    update_insteractive_list(&g_ui_instances);
}
