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

#ifndef DSTUDIO_RESSOURCE_USAGE_H_INCLUDED
#define DSTUDIO_RESSOURCE_USAGE_H_INCLUDED

#include <semaphore.h>

#include "common.h"
#include "ui.h"
#include "text.h"

typedef struct RessourceUsage_t {
    unsigned int    string_size;
    char            cpu_string_buffer[7];
    char            mem_string_buffer[7];
    ThreadControl   thread_control;
} RessourceUsage;

extern RessourceUsage g_ressource_usage;

void init_ressource_usage_backend(unsigned int string_size);

void * update_ressource_usage(void * args);

void update_ui_ressource_usage();

#endif
