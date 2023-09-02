/*
 * Copyright 2019, 2023 Denis Salem
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
#include <stdint.h>

#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#define DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE 8


typedef enum DStudioFailureMode_t {
    DSTUDIO_FAILURE_IS_FATAL = 1,
    DSTUDIO_FAILURE_IS_NOT_FATAL = 0
} DStudioFailureMode;

extern const char   g_dstudio_application_name[];
 
void * dstudio_alloc(uint_fast32_t buffer_size, DStudioFailureMode failure_is_fatal);
void   dstudio_free(void * buffer);
double dstudio_get_timestamp();

/* DStudio has it's own memory manager. It's a simple wrapper build around stantard
 * function like malloc, realloc and free. It MUST be the first dstudio call.*/
void   dstudio_init_memory_management();
void * dstudio_realloc(void * buffer, uint_fast32_t new_size);

#endif
