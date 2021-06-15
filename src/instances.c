/*
 * Copyright 2019, 2021 Denis Salem
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
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>

#include <sys/stat.h>
#include <unistd.h>

#include "audio_api.h"
#include "common.h"
#include "fileutils.h"
#include "info_bar.h"
#include "instances.h"
#include "ui.h"
#include "buttons.h"
#include "text_pointer.h"
#include "window_management.h"

static struct stat st = {0};

InstanceContext * g_current_active_instance = 0; 

Instances g_instances = {0};

UIInteractiveList g_ui_instances = {0};

static char * s_instances_directory = 0;
static FILE * s_instance_fd = 0;
static int s_fd = 0;
static InstanceContext * s_saved_contexts = 0;
static struct inotify_event * s_event = 0;
static struct pollfd s_fds = {0};

FILE * add_instance_file_descriptor() {
    uint_fast32_t count = 0;
    uint_fast32_t last_id = 0;
    char * instance_filename_buffer = dstudio_alloc(
        sizeof(char) * 128,
        DSTUDIO_FAILURE_IS_FATAL
    ); 
    char string_representation_of_integer[4] = {0};

    count_instances(s_instances_directory, &count, &last_id);
    strcat(instance_filename_buffer, s_instances_directory);
    strcat(instance_filename_buffer, "/");
    sprintf(string_representation_of_integer,"%" PRIuFAST32, last_id+1);
    strcat(instance_filename_buffer, string_representation_of_integer);
    return fopen(instance_filename_buffer, "w+");
}

uint_fast32_t _rename_active_context_audio_port(uint_fast32_t index) {
    (void) index;
    dstudio_rename_active_context_audio_port();
    return 0;
}

void init_instances_interactive_list(
    UIElements * ui_elements,
    uint_fast32_t lines_number,
    uint_fast32_t string_size,
    GLfloat item_offset_y
) {
    init_interactive_list(
        &g_ui_instances,
        ui_elements,
        lines_number,
        string_size,
        sizeof(InstanceContext),
        &g_instances.count,
        (char*) g_instances.contexts,
        select_instance_from_list,
        _rename_active_context_audio_port,
        1,
        item_offset_y
    );
    #ifdef DSTUDIO_DEBUG
        strcat(&g_ui_instances.trace[0], "g_ui_instances");
    #endif
}

void init_instance_management_backend() {
    int wd = 0;

    s_fd = inotify_init();
    if (errno != 0 && s_fd == -1) {
        DSTUDIO_TRACE_STR("inotify_init(): failed");
        exit(-1);
    }
    
    wd = inotify_add_watch(s_fd, s_instances_directory, IN_CREATE | IN_DELETE);

    if (errno != 0 && wd == 0) {
        DSTUDIO_TRACE_STR("inotify_add_watch(): failed");
        exit(-1);
    }

	s_event = dstudio_alloc(
        sizeof(struct inotify_event) + 16 + 1,
        DSTUDIO_FAILURE_IS_FATAL
    );
    
    s_fds.fd = s_fd;
    s_fds.events = POLLIN;
}

void instances_management() {
    int poll_result = poll(&s_fds, 1, 0);
    
    if (poll_result <= 0) {
        if (errno != EAGAIN && errno != EINVAL) {
            DSTUDIO_TRACE_ARGS("poll_result <=0 : %s", strerror(errno));
        }
        return;
    }
    
    if(read(s_fd, s_event, sizeof(struct inotify_event) + 16 + 1) < 0 && errno != 0) {
        return;
    }
    
    if (s_event->mask == IN_CREATE) {
        clear_text_pointer();
        g_instances.count++;
        s_saved_contexts = g_instances.contexts;
        g_instances.contexts = dstudio_realloc(g_instances.contexts, sizeof(InstanceContext) * g_instances.count);
        if (g_instances.contexts == NULL) {
            g_instances.contexts = s_saved_contexts;
            g_instances.count--;
            dstudio_update_info_text("New instance creation has failed.");
            return;
        }
        
        explicit_bzero(&g_instances.contexts[g_instances.count-1], sizeof(InstanceContext));
        g_current_active_instance = &g_instances.contexts[g_instances.count-1];
        g_instances.index = g_instances.count - 1;
        g_current_active_instance->identifier = 1;
        strcat((char*) g_current_active_instance->name, "Instance ");
        strcat((char*) g_current_active_instance->name, s_event->name);
        if (g_instances.count > g_ui_instances.lines_number) {
            g_ui_instances.window_offset = g_instances.count - g_ui_instances.lines_number;
            g_ui_instances.update_index = -1;
        }
        else {
            g_ui_instances.update_index = g_instances.index;
        }
        select_item(
            &g_ui_instances.lines[g_instances.index-g_ui_instances.window_offset],
            DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
        );
        
        new_voice();

        g_ui_instances.update_request = 1;

        #ifdef DSTUDIO_DEBUG
        printf("Create instance with id=%s. Allocated memory is now %ld.\n", s_event->name, sizeof(InstanceContext) * g_instances.count);
        printf("Currents instances:\n");
        for(uint_fast32_t i = 0; i < g_instances.count; i++) {
            printf("\t%s\n", g_instances.contexts[i].name);
        }
        #endif
    }
    else if (s_event->mask == IN_DELETE) {
        g_instances.count--;
        g_instances.index--;
        // TODO: Move context data in memory and then realloc
        #ifdef DSTUDIO_DEBUG
        printf("Remove instance with id=%s\n", s_event->name);
        #endif
    }
    char * fd_path = dstudio_alloc(
        strlen(s_instances_directory) +
        strlen(s_event->name) + 
        2, // slash + null byte
        DSTUDIO_FAILURE_IS_FATAL
    );
    
    /* We finally write in the related file descriptor that the
     * current instance has been processed.
     */
    strcat(fd_path, s_instances_directory);
    strcat(fd_path, "/");
    strcat(fd_path, s_event->name);
    s_instance_fd = fopen(fd_path, "w");
    fwrite((char*) g_current_active_instance->name, strlen((char*) g_current_active_instance->name), 1, s_instance_fd);
    fclose(s_instance_fd);
    dstudio_free(fd_path);
}

void new_instance(
    const char * given_directory,
    const char * process_name
) {
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);

    DIR * dr = 0;
    struct dirent *de;

    uint_fast32_t processes_count = count_process((char*)process_name);
    dstudio_expand_user(&s_instances_directory, (char*)given_directory);
    char * instance_filename_buffer = dstudio_alloc(
        sizeof(char) * 128,
        DSTUDIO_FAILURE_IS_FATAL
    ); 
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
        g_instances.contexts = dstudio_alloc(
            sizeof(InstanceContext),
            DSTUDIO_FAILURE_IS_FATAL
        );
        g_instances.count +=1;
        g_instances.contexts[0].identifier = 1;
        g_current_active_instance = &g_instances.contexts[0];
        strcpy((char*)g_current_active_instance->name, "Instance 1");
        new_voice();
        g_ui_instances.update_request = 1;
    }
    dstudio_free(instance_filename_buffer);
    dstudio_audio_api_request(DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING);
}

uint_fast32_t select_instance_from_list(
    uint_fast32_t index
) {
    uint_fast32_t voice_index = 0;
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
        bind_sub_context_interactive_list(
            setup_sub_context_interactive_list(),
            DSTUDIO_SELECT_ITEM_WITH_CALLBACK
        );
        return 1;
    }
    return 0;
}

void update_current_instance(uint_fast32_t index) {
    g_instances.index = index;
    g_current_active_instance = &g_instances.contexts[index];
}

void update_ui_instances_list() {
    instances_management();
    if (g_ui_instances.update_request) {
        g_ui_instances.source_data = &g_instances.contexts->name[0];
        update_interactive_list(&g_ui_instances);
    }
}
