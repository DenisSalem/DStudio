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
#include "system_usage.h"
#include "text.h"
#include "window_management.h"

UISystemUsage g_ui_system_usage = {0};

UIElements g_cpu_usage = {0};
UIElements g_mem_usage = {0};

void init_system_usage_ui(unsigned int string_size) {
    sem_init(&g_ui_system_usage.mutex, 0, 1);
    g_ui_system_usage.string_size = string_size;
    g_ui_system_usage.ready = 1;
}

void * update_system_usage(void * args) {
    (void) args;
    while(!g_ui_system_usage.ready) {
        usleep(1000);
    }

    while (1) {
        clock_t cpu_time = clock();
        usleep(250000);
        sem_wait(&g_ui_system_usage.mutex);
        
        if (g_ui_system_usage.cut_thread) {
            sem_post(&g_ui_system_usage.mutex);
            break;
        }
        double cpu_usage = (((double) (clock() - cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
        double mem_usage = get_proc_memory_usage();

        if (cpu_usage != -1) {
            explicit_bzero(g_ui_system_usage.cpu_string_buffer, 6);
            sprintf(g_ui_system_usage.cpu_string_buffer, "%0.1f%%", cpu_usage);
        }
        if (mem_usage != -1) {
            explicit_bzero(g_ui_system_usage.mem_string_buffer, 6);
            sprintf(g_ui_system_usage.mem_string_buffer, "%0.1f%%", mem_usage);
        }

        send_expose_event();
        g_ui_system_usage.update = 1;
        sem_post(&g_ui_system_usage.mutex);
        
        get_proc_memory_usage();
    }
    
    return NULL;
}

void update_ui_system_usage() {
    update_text(&g_cpu_usage, g_ui_system_usage.cpu_string_buffer, g_ui_system_usage.string_size);
    update_text(&g_mem_usage, g_ui_system_usage.mem_string_buffer, g_ui_system_usage.string_size);
}
