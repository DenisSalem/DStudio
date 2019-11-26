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

#ifndef DSTUDIO_SYSTEM_USAGE_H_INCLUDED
#define DSTUDIO_SYSTEM_USAGE_H_INCLUDED

#include <semaphore.h>
#include "ui.h"
#include "text.h"

typedef struct UISystemUsage_t {
    unsigned int    string_size;
    char            cpu_string_buffer[7];
    char            mem_string_buffer[7];
    int             ready;
    unsigned int    update;
    sem_t           mutex;
    int             cut_thread;
} UISystemUsage;

extern UISystemUsage g_ui_system_usage;
extern UIElements g_cpu_usage;
extern UIElements g_mem_usage;

void init_system_usage_ui(unsigned int string_size);

void * update_system_usage(void * args);

void update_ui_system_usage();
#endif
