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

#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#include <semaphore.h>

#include "constants.h"
#include "extensions.h"
#include "macros.h"
#include "paths.h"

typedef struct WindowScale_t {
    unsigned int width;
    unsigned int height;
} WindowScale;

extern const char g_application_name[];

/*
 * Safely allocate and initialize memory.
 */
void * dstudio_alloc(unsigned int buffer_size, int failure_is_fatal);
void   dstudio_free(void * buffer);
void dstudio_init_memory_management();
void * dstudio_realloc(void * buffer, unsigned int new_size);
double get_timestamp();

#endif
