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

#include "dstudio.h"

double s_previous_timestamp = 0;
clock_t s_previous_cpu_time = 0;

RessourceUsage g_ressource_usage = {0};

static UIElements *    s_cpu_usage = {0};
static UIElements *    s_mem_usage = {0};
static UIElements *    s_info_text = NULL;
static char       *    s_previous_info_message = NULL;

void dstudio_init_info_text(UIElements * info_text) {
    s_info_text = info_text;
    s_previous_info_message = dstudio_alloc(sizeof(char) * info_text->count, DSTUDIO_FAILURE_IS_FATAL);
}

// TODO : When message is longer than buffer size then animate.
void dstudio_update_info_text(char * message) {
    if (strcmp(s_previous_info_message, message) != 0) {
        strncpy(s_previous_info_message, message, s_info_text->count*sizeof(char));
        printf("%s\n", message);
        if (s_info_text) {
            update_text(s_info_text, message, strlen(message));
        }
    }
}

void dstudio_init_ressource_usage_backend(
    uint_fast32_t string_size,
    UIElements * cpu_usage,
    UIElements * mem_usage
) {
    set_physical_memory();
    g_ressource_usage.string_size = string_size;
    s_cpu_usage = cpu_usage;
    s_mem_usage = mem_usage;
}

void dstudio_update_ui_ressource_usage() {
    double timestamp = dstudio_get_timestamp();

    if (timestamp - s_previous_timestamp < 0.25) {
        return;
    }

    double cpu_usage = (((double) (clock() - s_previous_cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
    explicit_bzero(g_ressource_usage.cpu_string_buffer, g_ressource_usage.string_size);
    sprintf(g_ressource_usage.cpu_string_buffer, "%0.1lf%%", cpu_usage);
    //DSTUDIO_TRACE_ARGS("%s", g_ressource_usage.cpu_string_buffer)
    double mem_usage = get_proc_memory_usage();
    if (mem_usage != -1) {
        explicit_bzero(g_ressource_usage.mem_string_buffer, g_ressource_usage.string_size);
        sprintf(g_ressource_usage.mem_string_buffer, "%0.1lf%%", mem_usage);
    }
    update_text(s_cpu_usage, g_ressource_usage.cpu_string_buffer, g_ressource_usage.string_size);
    update_text(s_mem_usage, g_ressource_usage.mem_string_buffer, g_ressource_usage.string_size);
    
    s_previous_cpu_time = clock();
    s_previous_timestamp = timestamp;
}


