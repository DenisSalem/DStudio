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

#include "interactive_list.h"
#include "instances.h"

void configure_ui_interactive_list(
    UIElements * ui_elements,
    void * params
) {
    ((Vec4 *) ui_elements->instance_offsets_buffer)[0].z = 0.0;
    for (unsigned int i = 1; i < ui_elements->count; i++ ){
        ((Vec4 *) ui_elements->instance_offsets_buffer)[i].z = 0.25;
    }
    configure_ui_element(ui_elements, params);
}

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * lines,
    unsigned int lines_number,
    unsigned int string_size
) {
    interactive_list->lines = lines;
    interactive_list->lines_number = lines_number;
    interactive_list->string_size = string_size;
    interactive_list->update = 1;
    send_expose_event();
    sem_init(&interactive_list->mutex, 0, 1);
    interactive_list->ready = 1;
}

void * update_insteractive_list(
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
    return NULL;
}
