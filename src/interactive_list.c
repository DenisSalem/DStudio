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
#include "common.h"
#include "extensions.h"
#include "interactive_list.h"
#include "instances.h"

void configure_ui_interactive_list(
    UIElements * ui_elements,
    void * params
) {
    ((Vec4 *) ui_elements->instance_offsets_buffer)[0].opacity = 0.125;
    for (unsigned int i = 1; i < ui_elements->count; i++ ) {
        ((Vec4 *) ui_elements->instance_offsets_buffer)[i].opacity = 0;
    }
    configure_ui_element(ui_elements, params);
}

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * lines,
    unsigned int max_lines_number,
    unsigned int * lines_number,
    unsigned int string_size
) {
    interactive_list->lines = lines;
    interactive_list->max_lines_number = max_lines_number;
    interactive_list->lines_number = lines_number;
    interactive_list->string_size = string_size;
    interactive_list->update = 1;
    interactive_list->shadows = malloc(sizeof(UIElements) * max_lines_number);
    sem_init(&interactive_list->mutex, 0, 1);
    interactive_list->ready = 1;
}

void update_insteractive_list_shadow(
        int context_type,
        UIInteractiveList * interactive_list
) {
    int window_offset = interactive_list->window_offset;
    unsigned int context_index = 0;
    unsigned int * update_p = 0;
    
    switch(context_type) {
        case DSTUDIO_CONTEXT_INSTANCES:
            context_index = g_instances.index;
            update_p = &g_ui_instances.update;
            break;
    }
    
    int highlight = (int) context_index >= window_offset && context_index < window_offset + interactive_list->max_lines_number;
    int highlight_index = context_index - window_offset;
    
    for (unsigned int i = 0; i < interactive_list->max_lines_number; i++) {
        if (highlight_index == (int) i && highlight) {
            ((Vec4 *) interactive_list->shadows->instance_offsets_buffer)[i].opacity = 0.125;
        }
        else {
            ((Vec4 *) interactive_list->shadows->instance_offsets_buffer)[i].opacity = 0;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, interactive_list->shadows->instance_offsets);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * interactive_list->shadows->count, interactive_list->shadows->instance_offsets_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    *update_p = 1;
    send_expose_event();
}

void update_insteractive_list(
    int context_type,
    int offset,
    unsigned int max_lines_number,
    unsigned int lines_count,
    UIElements * lines,
    unsigned int string_size,
    void * contexts
) {
    char * name = 0;
    for(unsigned int i = 0; i < max_lines_number; i++) {
        if (i + offset < lines_count) {
            switch(context_type) {
                case DSTUDIO_CONTEXT_INSTANCES:
                    name = ((InstanceContext *) contexts)[i+offset].name;
                    break;
            }
            update_text(&lines[i], name, string_size);
        }
        else {
            update_text(&lines[i], "", string_size);
        }
    }
}
