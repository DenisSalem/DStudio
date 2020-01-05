/*
 * Copyright 2019 Denis Salem
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

#include <time.h>
#include <unistd.h>

#include "extensions.h"
#include "fileutils.h"
#include "ressource_usage.h"
#include "text.h"
#include "window_management.h"

RessourceUsage g_ressource_usage = {0};

UIElements g_cpu_usage = {0};
UIElements g_mem_usage = {0};

void init_ressource_usage_backend(unsigned int string_size) {
    sem_init(&g_ressource_usage.thread_control.mutex, 0, 1);
    set_physical_memory();
    g_ressource_usage.string_size = string_size;
    g_ressource_usage.thread_control.ready = 1;
}

void * update_ressource_usage(void * args) {
    (void) args;
    while(!g_ressource_usage.thread_control.ready) {
        usleep(1000);
    }
    while (1) {
        clock_t cpu_time = clock();
        usleep(250000);
        sem_wait(&g_ressource_usage.thread_control.mutex);
        
        if (g_ressource_usage.thread_control.cut_thread) {
            sem_post(&g_ressource_usage.thread_control.mutex);
            break;
        }
        double cpu_usage = (((double) (clock() - cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
        explicit_bzero(g_ressource_usage.cpu_string_buffer, 6);
        sprintf(g_ressource_usage.cpu_string_buffer, "%0.1lf%%", cpu_usage);
        
        double mem_usage = get_proc_memory_usage();
        if (mem_usage != -1) {
            explicit_bzero(g_ressource_usage.mem_string_buffer, 6);
            sprintf(g_ressource_usage.mem_string_buffer, "%0.1lf%%", mem_usage);
        }

        send_expose_event();
        g_ressource_usage.thread_control.update = 1;
        sem_post(&g_ressource_usage.thread_control.mutex);
    }
    return NULL;
}

/*void update_ui_system_usage() {
    update_text(&g_cpu_usage, g_ressource_usage.cpu_string_buffer, g_ressource_usage.string_size);
    update_text(&g_mem_usage, g_ressource_usage.mem_string_buffer, g_ressource_usage.string_size);
}*/
