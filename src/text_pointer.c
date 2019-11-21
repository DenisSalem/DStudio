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

#include "extensions.h"
#include "text_pointer.h"

UITextPointerContext g_text_pointer_context = {0}; 
UIElements g_text_pointer = {0};

unsigned int g_text_pointer_height = 0;
unsigned int g_text_pointer_char_width = 0;

void update_text_pointer_context(
    unsigned int type,
    unsigned int index,
    TextPointerContextPayload context
) {
    sem_wait(&g_text_pointer_context.mutex);
    switch(type) {
        case DSTUDIO_BUTTON_TYPE_LIST_ITEM:
            g_text_pointer_context.ui_text = &context.interactive_list->related_list->lines[index];
            g_text_pointer_context.string_buffer = context.interactive_list->get_item_name_callback(index);
            g_text_pointer_context.buffer_size = g_text_pointer_context.ui_text->count;
            
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
    text_pointer_offsets_buffer->x = (x_multiplier * x_inc) + ((g_text_pointer_char_width >> 1) * x_inc) + x_inc;
        
    text_pointer_offsets_buffer->y = y_multiplier * y_inc;
    text_pointer_offsets_buffer->y += 2 * y_inc; // offset of three pixels to the to

    // We compute the exact amount of pixel to render.
    g_text_pointer_context.scissor_x = 400 + 400 * ((Vec4 *) g_text_pointer.instance_offsets_buffer)->x - 1;
    g_text_pointer_context.scissor_y = 240 + 240 * ((Vec4 *) g_text_pointer.instance_offsets_buffer)->y - (g_text_pointer_height >> 1) - 1;
    g_text_pointer_context.scissor_width = 1;
    g_text_pointer_context.scissor_height = g_text_pointer_height;
    g_text_pointer_context.update = 1;
    sem_post(&g_text_pointer_context.mutex);
}

void update_text_pointer() {
    glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer.instance_offsets);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4), g_text_pointer.instance_offsets_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_text_pointer_context.update = 1;
}
