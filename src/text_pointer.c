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

#include <pthread.h>
#include <signal.h>
#include <unistd.h>

#include "common.h"
#include "extensions.h"
#include "text_pointer.h"

UITextPointerContext g_text_pointer_context = {0}; 
UIElements g_text_pointer = {0};

unsigned int g_text_pointer_height = 0;
unsigned int g_text_pointer_char_width = 0;
static unsigned int render_flag = 0;

void clear_text_pointer() {
    sem_wait(&g_text_pointer_context.mutex);
    g_text_pointer_context.active = 0;
    g_text_pointer_context.render_flag = render_flag;
    ((Vec4 *) g_text_pointer.instance_offsets_buffer)->opacity = 0.0;
    g_text_pointer_context.update = 1;
    sem_post(&g_text_pointer_context.mutex);
}

void update_text_pointer_context(
    unsigned int type,
    unsigned int index,
    TextPointerContextPayload context
) {
    sem_wait(&g_text_pointer_context.mutex);
    g_text_pointer_context.active = 0;
    sem_post(&g_text_pointer_context.mutex);
    if (g_text_pointer_context.blink_thread_id != 0) {
        pthread_join(g_text_pointer_context.blink_thread_id, NULL);
    }

    sem_wait(&g_text_pointer_context.mutex);
    switch(type) {
        case DSTUDIO_BUTTON_TYPE_LIST_ITEM:
            g_text_pointer_context.ui_text = &context.interactive_list->related_list->lines[index];
            g_text_pointer_context.string_buffer = context.interactive_list->get_item_name_callback(index);
            g_text_pointer_context.buffer_size = g_text_pointer_context.ui_text->count;
            g_text_pointer_context.index = index;
            g_text_pointer_context.render_flag = context.interactive_list->render_flag;
            break;
        #ifdef DSTUDIO_DEBUG
        default:
            // TODO: Set something more helpful
            exit(-1);
        #endif
    }
    unsigned int last_char_index = strlen(g_text_pointer_context.string_buffer) - 1;
    Vec4 * text_pointer_offsets_buffer = ((Vec4 *) g_text_pointer.instance_offsets_buffer);
    text_pointer_offsets_buffer->opacity = 1.0;
    
    // We need to compute coordinates in such way that the pointer will be perfectly aligned with pixels.
    GLfloat x_inc = 1.0 / (GLfloat) (DSTUDIO_VIEWPORT_WIDTH >> 1);
    GLfloat y_inc = 1.0 / (GLfloat) (DSTUDIO_VIEWPORT_HEIGHT >> 1);
    int x_multiplier = ((Vec4 *) g_text_pointer_context.ui_text->instance_offsets_buffer)[last_char_index].x / x_inc;
    int y_multiplier = ((Vec4 *) g_text_pointer_context.ui_text->instance_offsets_buffer)[last_char_index].y / y_inc;
    text_pointer_offsets_buffer->x = (x_multiplier * x_inc) + ((g_text_pointer_char_width >> 1) * x_inc) + x_inc - (4 * g_text_pointer_char_width * x_inc);
        
    text_pointer_offsets_buffer->y = y_multiplier * y_inc;
    text_pointer_offsets_buffer->y += 2 * y_inc; // offset of three pixels to the to

    // We compute the exact amount of pixel to render.
    g_text_pointer_context.scissor_x = 400 + 400 * ((Vec4 *) g_text_pointer.instance_offsets_buffer)->x - 1;
    g_text_pointer_context.scissor_y = 240 + 240 * ((Vec4 *) g_text_pointer.instance_offsets_buffer)->y - (g_text_pointer_height >> 1) - 1;
    g_text_pointer_context.scissor_width = 1;
    g_text_pointer_context.scissor_height = g_text_pointer_height;
    g_text_pointer_context.update = 1;
    if (!g_text_pointer_context.active) {
        g_text_pointer_context.active = 1;
        pthread_create( &g_text_pointer_context.blink_thread_id, NULL, text_pointer_blink_thread, NULL);
    }
    sem_post(&g_text_pointer_context.mutex);
}

void * text_pointer_blink_thread(void * args) {
    (void) args;
    Vec4 * text_pointer_offsets_buffer = ((Vec4 *) g_text_pointer.instance_offsets_buffer);
    render_flag = g_text_pointer_context.render_flag | DSTUDIO_RENDER_TEXT_POINTER;
    while (1) {
        usleep(125000);
        sem_wait(&g_text_pointer_context.mutex);
        if (text_pointer_offsets_buffer->opacity == 1.0) {
            text_pointer_offsets_buffer->opacity = 0.0;
            g_text_pointer_context.render_flag = render_flag;
        }
        else {
            if (!g_text_pointer_context.active) {
                g_text_pointer_context.update = 1;
                send_expose_event();
                break;
            }
            text_pointer_offsets_buffer->opacity = 1.0;
            g_text_pointer_context.render_flag = DSTUDIO_RENDER_TEXT_POINTER;
        }
        g_text_pointer_context.update = 1;
        send_expose_event();
        sem_post(&g_text_pointer_context.mutex);
    }
    sem_post(&g_text_pointer_context.mutex);
    return NULL;
}

void update_text_box() {
}

void update_text_pointer() {
    glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer.instance_offsets);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4), g_text_pointer.instance_offsets_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_text_pointer_context.update = 1;
}
