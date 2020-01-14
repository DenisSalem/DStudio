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
#include "text.h"

//~ unsigned char g_group_identifier = 0;

//~ void configure_ui_interactive_list(
    //~ UIElements * ui_elements,
    //~ void * params
//~ ) {
    //~ ((Vec4 *) ui_elements->instance_offsets_buffer)[0].opacity = 0.125;
    //~ for (unsigned int i = 1; i < ui_elements->count; i++ ) {
        //~ ((Vec4 *) ui_elements->instance_offsets_buffer)[i].opacity = 0;
    //~ }
    //~ configure_ui_element(ui_elements, params);
//~ }

void scroll_up(UIElements * self) {
    UIInteractiveList * interactive_list = (UIInteractiveList *) self->application_callback_args;
    sem_wait(&interactive_list->thread_bound_control->mutex);
    if (interactive_list->window_offset > 0) {
        interactive_list->window_offset--;
        interactive_list->update_request= -1;
        interactive_list->thread_bound_control->update = 1;
    }
    sem_post(&interactive_list->thread_bound_control->mutex);
}

void scroll_down(UIElements * self) {
    UIInteractiveList * interactive_list = (UIInteractiveList *) self->application_callback_args;
    sem_wait(&interactive_list->thread_bound_control->mutex);
    if (interactive_list->window_offset + interactive_list->lines_number < *interactive_list->source_data_count) {
        interactive_list->window_offset++;
        interactive_list->update_request= -1;
        interactive_list->thread_bound_control->update = 1;
    }
    sem_post(&interactive_list->thread_bound_control->mutex);
}

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * lines,
    unsigned int lines_number,
    unsigned int string_size,
    unsigned int stride,
    unsigned int * source_data_count,
    char ** source_data,
    ThreadControl * thread_bound_control
) {
    interactive_list->lines_number = lines_number;
    interactive_list->string_size = string_size;
    interactive_list->lines = lines;
    for (unsigned int i = 0; i < lines_number; i++) {
        interactive_list->lines[i].interactive_list = interactive_list;
    }
    interactive_list->stride = stride;
    interactive_list->source_data_count = source_data_count;
    interactive_list->source_data = source_data;
    interactive_list->thread_bound_control = thread_bound_control;
}

//~ void update_insteractive_list_shadow(
        //~ int context_type,
        //~ UIInteractiveList * interactive_list
//~ ) {
    //~ int window_offset = interactive_list->window_offset;
    //~ unsigned int context_index = 0;
    //~ unsigned int * update_p = 0;
    
    //~ switch(context_type) {
        //~ case DSTUDIO_CONTEXT_INSTANCES:
            //~ context_index = g_instances.index;
            //~ update_p = &g_ui_instances.update;
            //~ break;
    //~ }
    
    //~ int highlight = (int) context_index >= window_offset && context_index < window_offset + interactive_list->max_lines_number;
    //~ int highlight_index = context_index - window_offset;
    
    //~ for (unsigned int i = 0; i < interactive_list->max_lines_number; i++) {
        //~ if (highlight_index == (int) i && highlight) {
            //~ ((Vec4 *) interactive_list->shadows->instance_offsets_buffer)[i].opacity = 0.125;
        //~ }
        //~ else {
            //~ ((Vec4 *) interactive_list->shadows->instance_offsets_buffer)[i].opacity = 0;
        //~ }
    //~ }
    //~ glBindBuffer(GL_ARRAY_BUFFER, interactive_list->shadows->instance_offsets);
        //~ glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * interactive_list->shadows->count, interactive_list->shadows->instance_offsets_buffer);
    //~ glBindBuffer(GL_ARRAY_BUFFER, 0);
    //~ *update_p = 1;
    //~ send_expose_event();
//~ }

void update_insteractive_list(
    UIInteractiveList * interactive_list
) {
    unsigned int stride = interactive_list->stride;
    unsigned int window_offset = interactive_list->window_offset;
    unsigned int string_size = interactive_list->string_size;
    unsigned int source_data_count = *interactive_list->source_data_count;
    int index = interactive_list->update_request; 
    for (unsigned int i = index < 0 ? 0 : index; i < interactive_list->lines_number; i++) {
        if (i >= source_data_count) {
            break;
        }
        update_text(
            &interactive_list->lines[i],
            &(*interactive_list->source_data)[stride * (i + window_offset)],
            string_size
        );
        if (index >= 0) {
            break;
        }
    }
}
