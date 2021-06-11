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

#ifndef DSTUDIO_INFO_BAR_H_INCLUDED
#define DSTUDIO_INFO_BAR_H_INCLUDED

#include "common.h"
#include "ui.h"
#include "text.h"

typedef struct RessourceUsage_t {
    uint_fast32_t   string_size;
    char            cpu_string_buffer[7];
    char            mem_string_buffer[7];
} RessourceUsage;

extern RessourceUsage g_ressource_usage;

void * update_ressource_usage_thread(void * args);

void dstudio_init_info_text(UIElements * info_text);
void dstudio_init_ressource_usage_backend(
    uint_fast32_t string_size,
    UIElements * cpu_usage,
    UIElements * mem_usage
);
void dstudio_update_info_text(char * message);
void dstudio_update_ui_ressource_usage();

#endif
