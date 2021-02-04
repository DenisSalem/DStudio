/*
 * Copyright 2019, 2021 Denis Salem
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
    UIElements *    text_pointer;
    UIElements *    ui_text;
    UIElements *    highlight;
    char *          string_buffer;
    unsigned int    buffer_size;
    ThreadControl   thread_control;
    unsigned int    active;
    unsigned int    insert_char_index;
    pthread_t       blink_thread_id;
    unsigned int    text_pointer_height;
    unsigned int    char_width;
} UITextPointerContext;

void clear_text_pointer();

void compute_text_pointer_coordinates(unsigned int index);

void init_text_pointer();
void init_ui_text_pointer(UIElements * text_pointer);

void * text_pointer_blink_thread(void * args);

void update_text_pointer();

void update_text_box(unsigned int keycode);

void update_text_pointer_context(UIElements * ui_elements);

extern UITextPointerContext g_text_pointer_context;
extern unsigned int g_text_pointer_height;
extern unsigned int g_text_pointer_char_width;
#endif
