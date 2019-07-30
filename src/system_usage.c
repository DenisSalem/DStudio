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
#include "system_usage.h"

void init_system_usage_ui(
    UISystemUsage * system_usage,
    const char * texture_system_usage_filename,
    const char * texture_text_filename,
    unsigned int texture_system_usage_width,
    unsigned int texture_system_usage_height,
    unsigned int texture_text_width,
    unsigned int texture_text_height,
    unsigned int viewport_width,
    unsigned int viewport_height
) {  
    system_usage->instance_offsets_buffer.x = 0.0225;
    system_usage->instance_offsets_buffer.y = 0.889583;
    
    init_background_element(
        system_usage->vertex_indexes,
        system_usage->vertexes_attributes,
        &system_usage->index_buffer_object,
        &system_usage->vertex_buffer_object,
        texture_system_usage_filename,
        &system_usage->texture,
        1,
        &system_usage->texture_id,
        &system_usage->vertex_array_object,
        texture_system_usage_width,
        texture_system_usage_height,
        viewport_width,
        viewport_height,
        system_usage->scale_matrix, 
        &system_usage->instance_offsets,
        &system_usage->instance_offsets_buffer,
        1
    );
    
    init_text(
        &system_usage->ui_text_cpu,
        6,
        texture_text_filename,
        texture_text_width,
        texture_text_height,
        viewport_width,
        viewport_height
    );
}

void * update_system_usage(void * args) {
    SystemUsage * system_usage = (SystemUsage *) args;
    while (!system_usage->cut_thread) {
        clock_t cpu_time = clock();
        usleep(250000);
        system_usage->cpu_usage = (((double) (clock() - cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
    }
    return NULL;
}
