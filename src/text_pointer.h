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

#ifndef DSTUDIO_TEXT_POINTER_H_INCLUDED
#define DSTUDIO_TEXT_POINTER_H_INCLUDED

#include <semaphore.h>

#include "ui.h"
#include "interactive_list.h"

typedef struct UITextPointerContext_t {
    UIElements *    ui_text;
    char *          string_buffer;
    unsigned int    buffer_size;
    GLint           scissor_x;
    GLint           scissor_y;
    GLsizei         scissor_width;
    GLsizei         scissor_height;
    unsigned int    update;
    unsigned int    active;
    unsigned int    render_flag;
    // Optional, only required for interactive list
    unsigned int    index;
    unsigned int *  lines_count;
    unsigned int    insert_char_index;
    pthread_t       blink_thread_id;
    sem_t           mutex;
} UITextPointerContext;

typedef union TextPointerContextPayload_t {
    InteractiveListContext * interactive_list;
} TextPointerContextPayload;

void clear_text_pointer();

void compute_text_pointer_coordinates(unsigned int index, int offset);

void * text_pointer_blink_thread(void * args);

void update_text_pointer();

void update_text_box(unsigned int keycode);

void update_text_pointer_context(
    unsigned int type,
    unsigned int index,
    TextPointerContextPayload context
);

extern UIElements g_text_pointer;
extern UITextPointerContext g_text_pointer_context;
extern unsigned int g_text_pointer_height;
extern unsigned int g_text_pointer_char_width;
#endif
