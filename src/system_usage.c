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

#include "system_usage.h"

void init_ui_system_usage_cpu_side(UISystemUsage * system_usage, const char * texture_filename, int texture_width, int texture_height, int viewport_width, int viewport_height) {
    system_usage->texture_width = texture_width;
    system_usage->texture_height = texture_height;

    get_png_pixel(texture_filename, &system_usage->texture, PNG_FORMAT_RGBA);
    GLchar * vertex_indexes = system_usage->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
    Vec2 * scale_matrix = system_usage->scale_matrix;
    scale_matrix[0].x = ((float) texture_width) / ((float) viewport_width);
    scale_matrix[0].y = 0;
    scale_matrix[1].x = 0;
    scale_matrix[1].y = ((float) texture_height) / ((float) viewport_height);
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
