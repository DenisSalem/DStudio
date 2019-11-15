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

void update_text_pointer_context(unsigned int type, unsigned int index, TextPointerContextPayload context) {
    if (type == DSTUDIO_BUTTON_TYPE_LIST_ITEM) {
        g_text_pointer_context.ui_text = &context.interactive_list->related_list->lines[index];
        g_text_pointer_context.string_buffer = context.interactive_list->get_item_name_callback(index);
        g_text_pointer_context.buffer_size = g_text_pointer_context.ui_text->count;
    }
    printf("lines: %d, text: %s\n", index, g_text_pointer_context.string_buffer);
    ((Vec4 *) g_text_pointer.instance_offsets_buffer)->z = 1.0;
}

void update_text_pointer() {
    glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer.instance_offsets);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4), g_text_pointer.instance_offsets_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    g_text_pointer_context.update = 1;
}
