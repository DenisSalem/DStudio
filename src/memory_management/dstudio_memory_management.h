/*
 * Copyright 2019, 2025 Denis Salem
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

#ifndef DSTUDIO_MEMORY_MANAGEMENT_H_INCLUDED
#define DSTUDIO_MEMORY_MANAGEMENT_H_INCLUDED

#define DSTUDIO_FAILURE_IS_FATAL 1
#define DSTUDIO_FAILURE_IS_NOT_FATAL 0
#define DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE 8

void * dstudio_alloc(uint_fast32_t buffer_size, uint_fast32_t failure_is_fatal);
void   dstudio_free(void * buffer);

void   dstudio_init_memory_management();
void * dstudio_realloc(void * buffer, uint_fast32_t new_size);

#endif
