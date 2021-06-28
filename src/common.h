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
#include <stdint.h>

#include "constants.h"
#include "extensions.h"
#include "macros.h"
#include "paths.h"

typedef struct DStudioActiveContext_t {
    void * previous;
    void * current;
} DStudioActiveContext;

typedef struct DStudioContexts_t {
    void * data;
    uint_fast32_t data_type_size;
    uint_fast32_t count;
    uint_fast32_t index;
} DStudioContexts;

typedef struct DStudioWindowScale_t {
    uint_fast32_t width;
    uint_fast32_t height;
} DStudioWindowScale;

typedef struct DStudioMonitorRegister_t {
    void (*callback)();
} DStudioMonitorRegister;

extern const char g_dstudio_application_name[];
extern DStudioActiveContext g_dstudio_active_contexts[3];

/*
 * dstudio_alloc
 * dstudio_free
 * dstudio_init_memory_management
 * dstudio_realloc
 * 
 * Allocate and initialize memory.
 * Not thread safe.
 */
 
void * dstudio_alloc(uint_fast32_t buffer_size, uint_fast32_t failure_is_fatal);
void   dstudio_events_monitor();
void   dstudio_free(void * buffer);
double dstudio_get_timestamp();
void   dstudio_init_events_monitor_register(uint_fast32_t monitor_count);
/* DStudio has it's own memory manager. It's a simple wrapper build around stantard
 * function like malloc, realloc and free. It MUST be the first dstudio call.*/
void   dstudio_init_memory_management();
void * dstudio_realloc(void * buffer, uint_fast32_t new_size);
void   dstudio_register_events_monitor(void (*callback)());
#endif
