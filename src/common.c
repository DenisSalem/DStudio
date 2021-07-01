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
#include "instances.h"

DStudioActiveContext    g_dstudio_active_contexts[3] = {0};
uint_fast32_t           g_dstudio_client_context_size = 0;

static uint_fast64_t * s_allocation_register = 0;
static uint_fast32_t s_allocation_register_index = 0;
static uint_fast32_t s_allocation_register_size = DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;

static DStudioMonitorRegister *    s_monitors_register = 0;
static uint_fast32_t         s_monitors_register_index = 0;

void * dstudio_alloc(uint_fast32_t buffer_size, uint_fast32_t failure_is_fatal) {
    if (s_allocation_register_index >= s_allocation_register_size) {
        s_allocation_register = realloc(s_allocation_register, sizeof(uint_fast64_t) * (s_allocation_register_size + DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        explicit_bzero(&s_allocation_register[s_allocation_register_size], sizeof(uint_fast64_t) * DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE);
        s_allocation_register_size += DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register[s_allocation_register_index] = (uint_fast64_t) malloc(buffer_size);
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

void dstudio_events_monitor() {
    for (uint_fast32_t i = 0; i < s_monitors_register_index; i++) {
        s_monitors_register[i].callback();
    }
}

void dstudio_free(void * buffer) {    
    if (buffer == 0) {
        for (uint_fast32_t i = 0; i < s_allocation_register_index; i++) {
            free((void *)s_allocation_register[i]);
        }
        free(s_allocation_register);
        return;
    }
    
    for (uint_fast32_t i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (uint_fast64_t) buffer) {
            free((void *)s_allocation_register[i]);
            s_allocation_register[i] = 0;
            for (uint_fast32_t j = i+1; j < s_allocation_register_size; j++) {
                if (s_allocation_register[j] == 0) {
                    break;
                }
                s_allocation_register[j-1] = s_allocation_register[j];
            }
            break;
        }
    }
    if (s_allocation_register_index < s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE) {
        s_allocation_register = realloc(s_allocation_register, sizeof(uint_fast64_t) * (s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        s_allocation_register_size -= DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register_index--;
    return;
}

double dstudio_get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    return (double) (timestamp.tv_sec * 1000 + timestamp.tv_nsec / 1000000) / 1000.0;
}

void dstudio_init_events_monitor_register(uint_fast32_t monitor_count) {
    s_monitors_register = dstudio_alloc(
        monitor_count * sizeof(DStudioMonitorRegister),
        DSTUDIO_FAILURE_IS_FATAL
    );
}

void dstudio_init_memory_management() {
    s_allocation_register = malloc(sizeof(uint_fast64_t) * s_allocation_register_size);
    DSTUDIO_EXIT_IF_NULL(s_allocation_register)
    explicit_bzero(s_allocation_register, sizeof(uint_fast64_t) * s_allocation_register_size);
}

void * dstudio_realloc(void * buffer, uint_fast32_t new_size) {
    if (buffer == NULL) {
        return dstudio_alloc(new_size, DSTUDIO_FAILURE_IS_FATAL);
    }
    void * new_buffer = 0;
    for (uint_fast32_t i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (uint_fast64_t) buffer) {
            new_buffer = realloc(buffer, new_size);
            if (!new_buffer) {
                return NULL;
            }
            s_allocation_register[i] = (uint_fast64_t) new_buffer;
            break;
        }
    }
    return new_buffer;
}

void dstudio_register_events_monitor(void (*callback)()) {
    s_monitors_register[s_monitors_register_index++].callback = callback;
}

void dstudio_update_current_context(uint_fast32_t index, uint_fast32_t context_level) { 
        DStudioContexts * parent = 0;
        uint_fast32_t data_type_size = 0;
        switch (context_level) {
            case DSTUDIO_INSTANCE_CONTEXT_LEVEL:
                parent = ((InstanceContext*)g_dstudio_active_contexts[context_level].current)->parent;
                data_type_size = sizeof(InstanceContext);
                break;
            case DSTUDIO_VOICE_CONTEXT_LEVEL:
                parent = ((VoiceContext*)g_dstudio_active_contexts[context_level].current)->parent;
                data_type_size = sizeof(VoiceContext);
                break;
                
            case DSTUDIO_CLIENT_CONTEXT_LEVEL:
                parent = ((VoiceContext*)g_dstudio_active_contexts[context_level].current)->parent;
                data_type_size = g_dstudio_client_context_size;
                break;
                
            default:
                DSTUDIO_TRACE_STR("Invalid context level.");
                exit(-1);
        }
        parent->index = index;
        g_dstudio_active_contexts[context_level].previous = g_dstudio_active_contexts[context_level].current;
        g_dstudio_active_contexts[context_level].current  = parent->data + (index*data_type_size);
}
