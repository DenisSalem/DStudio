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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

GLint scissor_x, scissor_y = 0;
GLsizei scissor_width, scissor_height = 0;

static long unsigned int * s_allocation_register = 0;
static unsigned int s_allocation_register_index = 0;
static unsigned int s_allocation_register_size = DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;

void * dstudio_alloc(unsigned int buffer_size, int failure_is_fatal) {
    if (s_allocation_register_index >= s_allocation_register_size) {
        s_allocation_register = realloc(s_allocation_register, sizeof(long unsigned int) * (s_allocation_register_size + DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        explicit_bzero(&s_allocation_register[s_allocation_register_size], sizeof(long unsigned int) * DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE);
        s_allocation_register_size += DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register[s_allocation_register_index] = (long unsigned int) malloc(buffer_size);
    if (s_allocation_register[s_allocation_register_index] == 0) {
        if(failure_is_fatal) {
            // https://stackoverflow.com/questions/3126149/what-operating-systems-wont-free-memory-on-program-exit 
            DSTUDIO_EXIT_IF_NULL(NULL)
        }
        else {
            return NULL;
        }
    }
    explicit_bzero((void *) s_allocation_register[s_allocation_register_index], buffer_size);
    return (void *) s_allocation_register[s_allocation_register_index++];
}

void dstudio_free(void * buffer) {    
    if (buffer == 0) {
        for (unsigned int i = 0; i < s_allocation_register_index; i++) {
            free((void *)s_allocation_register[i]);
        }
        free(s_allocation_register);
        return;
    }
    
    for (unsigned int i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (long unsigned int) buffer) {
            free((void *)s_allocation_register[i]);
            s_allocation_register[i] = 0;
            for (unsigned int j = i+1; j < s_allocation_register_size; j++) {
                if (s_allocation_register[j] == 0) {
                    break;
                }
                s_allocation_register[j-1] = s_allocation_register[j];
            }
            break;
        }
    }
    if (s_allocation_register_index < s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE) {
        s_allocation_register = realloc(s_allocation_register, sizeof(long unsigned int) * (s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        s_allocation_register_size -= DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register_index--;
    return;
}

/* DStudio has it's own memory manager. It's a simple wrapper build around stantard
 * function like malloc, realloc and free. It MUST be the first dstudio call.*/
void dstudio_init_memory_management() {
    s_allocation_register = malloc(sizeof(long unsigned int) * s_allocation_register_size);
    DSTUDIO_EXIT_IF_NULL(s_allocation_register)
    explicit_bzero(s_allocation_register, sizeof(long unsigned int) * s_allocation_register_size);
}

void * dstudio_realloc(void * buffer, unsigned int new_size) {
    if (buffer == NULL) {
        return dstudio_alloc(new_size, DSTUDIO_FAILURE_IS_FATAL);
    }
    void * new_buffer = 0;
    for (unsigned int i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (long unsigned int) buffer) {
            new_buffer = realloc(buffer, new_size);
            if (!new_buffer) {
                return NULL;
            }
            s_allocation_register[i] = (long unsigned int) new_buffer;
            break;
        }
    }
    return new_buffer;
}

double get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    return (double) (timestamp.tv_sec * 1000 + timestamp.tv_nsec / 1000000) / 1000.0;
}
