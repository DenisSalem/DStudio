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

void init_interactive_list(
    UIInteractiveList * interactive_list,
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    unsigned int stride,
    unsigned int * source_data_count,
    char * source_data,
    ThreadControl * thread_bound_control,
    unsigned int (*select_callback)(unsigned int index),
    unsigned int editable,
    GLfloat highlight_step
) {
    interactive_list->lines_number = lines_number;
    interactive_list->string_size = string_size;
    interactive_list->highlight = ui_elements;
    interactive_list->lines = &ui_elements[1];
    for (unsigned int i = 0; i < lines_number; i++) {
        interactive_list->lines[i].interactive_list = interactive_list;
    }
    interactive_list->stride = stride;
    interactive_list->source_data_count = source_data_count;
    interactive_list->source_data = source_data;
    interactive_list->thread_bound_control = thread_bound_control;
    interactive_list->select_callback = select_callback;
    interactive_list->editable = editable;
    interactive_list->highlight_step = highlight_step;
    interactive_list->highlight_offset_y = ui_elements->instance_offsets_buffer->y;
}

void scroll_up(UIElements * self) {
    UIInteractiveList * interactive_list = (UIInteractiveList *) self->application_callback_args;
    sem_t * mutex = interactive_list->thread_bound_control->shared_mutex ? interactive_list->thread_bound_control->shared_mutex : &interactive_list->thread_bound_control->mutex;
    sem_wait(mutex);
    if (interactive_list->window_offset > 0) {
        interactive_list->window_offset--;
        interactive_list->update_request= -1;
        interactive_list->thread_bound_control->update = 1;
        if (++interactive_list->index < (int) interactive_list->lines_number && interactive_list->index >= 0) {
            select_item(
                &interactive_list->lines[interactive_list->index],
                DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
            );
            *interactive_list->highlight->instance_alphas_buffer = 1;

        }
        else {
            interactive_list->update_highlight = 1;
            *interactive_list->highlight->instance_alphas_buffer = 0;
        }
        printf("index: %d\n", interactive_list->index);
    }
    sem_post(mutex);
}

void scroll_down(UIElements * self) {
    UIInteractiveList * interactive_list = (UIInteractiveList *) self->application_callback_args;
    sem_t * mutex = interactive_list->thread_bound_control->shared_mutex ? interactive_list->thread_bound_control->shared_mutex : &interactive_list->thread_bound_control->mutex;
    sem_wait(mutex);
    if (interactive_list->window_offset + interactive_list->lines_number < *interactive_list->source_data_count) {
        interactive_list->window_offset++;
        interactive_list->update_request= -1;
        interactive_list->thread_bound_control->update = 1;
        if (--interactive_list->index >= 0 && interactive_list->index < (int) interactive_list->lines_number) {
            select_item(
                &interactive_list->lines[interactive_list->index],
                DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK
            );
        }
        else {
            interactive_list->update_highlight = 1;
            *interactive_list->highlight->instance_alphas_buffer = 0;
        }
        printf("index: %d\n", interactive_list->index);

    }
    sem_post(mutex);
}

void select_item(
    UIElements * self,
    SelectItemOpt do_not_use_callback
) {
    unsigned int lines_number = self->interactive_list->lines_number;
    UIInteractiveList * interactive_list = self->interactive_list;
    UIElements * highlight = interactive_list->highlight;
    sem_t * mutex = interactive_list->thread_bound_control->shared_mutex ? interactive_list->thread_bound_control->shared_mutex : &interactive_list->thread_bound_control->mutex;
    if (!do_not_use_callback) {
        sem_wait(mutex);
    }
    for(unsigned int i = 0; i < lines_number; i++) {
        if (&interactive_list->lines[i] == self) {
            if(do_not_use_callback || interactive_list->select_callback(i+interactive_list->window_offset)) {
                interactive_list->lines[interactive_list->previous_item_index].render = 1;
                interactive_list->lines[i].render = 1;
                interactive_list->thread_bound_control->update = 1;
                *interactive_list->highlight->instance_alphas_buffer = 1;
                interactive_list->update_highlight = 1;
                highlight->instance_offsets_buffer->y = interactive_list->highlight_offset_y + interactive_list->highlight_step * i;
                interactive_list->index = i;
                highlight->scissor.y = (1 + highlight->instance_offsets_buffer->y - highlight->scale_matrix[1].y) * (g_dstudio_viewport_height >> 1);
                interactive_list->previous_item_index = i;
            }
            break;
        }
    }
    if (!do_not_use_callback){
        sem_post(mutex);
    }
}

void update_insteractive_list(
    UIInteractiveList * interactive_list
) {
    unsigned int stride = interactive_list->stride;
    unsigned int window_offset = interactive_list->window_offset;
    unsigned int string_size = interactive_list->string_size;
    unsigned int source_data_count = *interactive_list->source_data_count;
    int index = interactive_list->update_request;
    for (unsigned int i = index < 0 ? 0 : index; i < interactive_list->lines_number; i++) {
        if (i < source_data_count) {
            update_text(
                &interactive_list->lines[i],
                &interactive_list->source_data[stride * (i + window_offset)],
                string_size
            );
        }
        else {
            update_text(
                &interactive_list->lines[i],
                "",
                string_size
            );
        }
    }
    if (interactive_list->update_highlight) {
        glBindBuffer(GL_ARRAY_BUFFER, interactive_list->highlight->instance_offsets);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4) * interactive_list->highlight->count, interactive_list->highlight->instance_offsets_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, interactive_list->highlight->instance_alphas);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * interactive_list->highlight->count, interactive_list->highlight->instance_alphas_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        interactive_list->highlight->render = 1;
        interactive_list->update_highlight = 0;
    }
}
