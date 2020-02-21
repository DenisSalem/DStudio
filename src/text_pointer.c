/*
 * Copyright 2019, 2020 Denis Salem
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
#include "text.h"
#include "text_pointer.h"

static Vec2 s_text_pointer_4x9_scale_matrix[2] = {0};
static Vec2 s_text_pointer_8x18_scale_matrix[2] = {0};

UITextPointerContext g_text_pointer_context = {0}; 

void clear_text_pointer() {
    sem_wait(&g_text_pointer_context.thread_control.mutex);

    if (g_text_pointer_context.active) {
        g_text_pointer_context.active = 0;
        *g_text_pointer_context.text_pointer->instance_alphas_buffer = 0.0;
        g_text_pointer_context.thread_control.update = 1;
        if (g_text_pointer_context.blink_thread_id != 0) {
            sem_post(&g_text_pointer_context.thread_control.mutex);
            DSTUDIO_EXIT_IF_FAILURE(pthread_join(g_text_pointer_context.blink_thread_id, NULL))
            return;
        }
    }
    sem_post(&g_text_pointer_context.thread_control.mutex);

}

void compute_text_pointer_coordinates(unsigned int index) {
    unsigned int half_viewport_width = g_dstudio_viewport_width >> 1;
    unsigned int half_viewport_height = g_dstudio_viewport_height >> 1;
    *g_text_pointer_context.text_pointer->instance_alphas_buffer = 1.0;

    // We need to compute coordinates in such way that the pointer will be perfectly aligned with pixels.
    GLfloat x_inc = 1.0 / (GLfloat) half_viewport_width;
    GLfloat y_inc = 1.0 / (GLfloat) half_viewport_height;
    int x_multiplier = g_text_pointer_context.ui_text->instance_offsets_buffer[index].x / x_inc;
    g_text_pointer_context.text_pointer->instance_offsets_buffer->x = (x_multiplier - (g_text_pointer_context.char_width) - 1) * x_inc;        
    g_text_pointer_context.text_pointer->instance_offsets_buffer->y = g_text_pointer_context.ui_text->instance_offsets_buffer[index].y + y_inc;
    
    // We compute the exact amount of pixel to render.
    g_text_pointer_context.text_pointer->scissor.x = half_viewport_width + half_viewport_width * g_text_pointer_context.text_pointer->instance_offsets_buffer->x - 1;
    g_text_pointer_context.text_pointer->scissor.y = half_viewport_height + half_viewport_height * g_text_pointer_context.text_pointer->instance_offsets_buffer->y - (g_text_pointer_context.text_pointer_height >>1 ) - 1;
    g_text_pointer_context.text_pointer->scissor.width = 1;
    g_text_pointer_context.text_pointer->scissor.height = g_text_pointer_context.text_pointer_height;
    g_text_pointer_context.thread_control.update = 1;
}

void init_text_pointer() {
    sem_init(&g_text_pointer_context.thread_control.mutex, 0, 1);
}

void init_ui_text_pointer(UIElements * text_pointer) {
    s_text_pointer_4x9_scale_matrix[0].x = (DSTUDIO_TEXT_POINTER_WIDTH / (float) g_dstudio_viewport_width);
    s_text_pointer_4x9_scale_matrix[1].y = ((float) DSTUDIO_TEXT_POINTER_1_HEIGHT / (float) g_dstudio_viewport_height);

    s_text_pointer_8x18_scale_matrix[0].x = (DSTUDIO_TEXT_POINTER_WIDTH / (float) g_dstudio_viewport_width);
    s_text_pointer_8x18_scale_matrix[1].y = ((float) DSTUDIO_TEXT_POINTER_2_HEIGHT / (float) g_dstudio_viewport_height);    
    text_pointer->color.r = 1;
    text_pointer->color.g = 0.5;
    text_pointer->color.b = 0;
    text_pointer->color.a = 1.0;

    init_ui_elements(
        text_pointer,
        NULL,
        &s_text_pointer_4x9_scale_matrix[0],
        0,
        0,
        DSTUDIO_TEXT_POINTER_WIDTH,
        DSTUDIO_TEXT_POINTER_1_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );
    text_pointer->request_render = 0;
    
    g_text_pointer_context.text_pointer = text_pointer;
}

void update_text_pointer_context(UIElements * ui_elements) {
    UIInteractiveList * interactive_list = 0;
    sem_wait(&g_text_pointer_context.thread_control.mutex);
    g_text_pointer_context.active = 0;
    sem_post(&g_text_pointer_context.thread_control.mutex);
    if (g_text_pointer_context.blink_thread_id != 0) {
        pthread_join(g_text_pointer_context.blink_thread_id, NULL);
    }

    sem_wait(&g_text_pointer_context.thread_control.mutex);
    switch(ui_elements->type) {
        case DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM:
            interactive_list = ui_elements->interactive_list;
            g_text_pointer_context.ui_text = ui_elements;
            g_text_pointer_context.highlight = interactive_list->highlight;
            for(unsigned int i = 0; i < interactive_list->lines_number; i++) {
                if (&interactive_list->lines[i] == ui_elements) {
                    unsigned int index = i+interactive_list->window_offset;
                    if (index >= *interactive_list->source_data_count) {
                        sem_post(&g_text_pointer_context.thread_control.mutex);
                        return;
                    }
                    ui_elements->request_render = 1;
                    g_text_pointer_context.string_buffer = &interactive_list->source_data[index*interactive_list->stride];
                    g_text_pointer_context.buffer_size = interactive_list->string_size;
                    g_text_pointer_context.text_pointer_height = (g_dstudio_viewport_height) * ui_elements->scale_matrix[1].y;
                    break;
                }
            }
            break;
        case DSTUDIO_UI_ELEMENT_TYPE_BACKGROUND:
        case DSTUDIO_UI_ELEMENT_TYPE_BUTTON:
        case DSTUDIO_UI_ELEMENT_TYPE_BUTTON_REBOUNCE:
        case DSTUDIO_UI_ELEMENT_TYPE_KNOB:
        case DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM:
        case DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE:
        case DSTUDIO_UI_ELEMENT_TYPE_PATTERN:
        case DSTUDIO_UI_ELEMENT_TYPE_SLIDER:
        case DSTUDIO_UI_ELEMENT_TYPE_TEXT:
            sem_post(&g_text_pointer_context.thread_control.mutex);
            return;
    }

    unsigned int last_char_index = strlen(g_text_pointer_context.string_buffer);
    g_text_pointer_context.insert_char_index = last_char_index;

    compute_text_pointer_coordinates(last_char_index);

    if (!g_text_pointer_context.active) {
        g_text_pointer_context.active = 1;
        pthread_create( &g_text_pointer_context.blink_thread_id, NULL, text_pointer_blink_thread, NULL);
    }
    sem_post(&g_text_pointer_context.thread_control.mutex);
}

void * text_pointer_blink_thread(void * args) {
    (void) args;
    GLfloat * text_pointer_alphas_buffer = g_text_pointer_context.text_pointer->instance_alphas_buffer;
    while (1) {
        usleep(125000);
        sem_wait(&g_text_pointer_context.thread_control.mutex);
        if (*text_pointer_alphas_buffer == 1.0) {
            *text_pointer_alphas_buffer = 0.0;
        }
        else {
            if (!g_text_pointer_context.active) {
                g_text_pointer_context.text_pointer->request_render = 1;
                g_text_pointer_context.thread_control.update = 1;
                send_expose_event();
                break;
            }
            *text_pointer_alphas_buffer = 1.0;
        }
        g_text_pointer_context.text_pointer->request_render = 1;
        g_text_pointer_context.thread_control.update = 1;
        send_expose_event();
        sem_post(&g_text_pointer_context.thread_control.mutex);
    }
    sem_post(&g_text_pointer_context.thread_control.mutex);
    return NULL;
}

/* Beware, there is goto in the following. Do not panic or call the
 * police, everything is under control. It's just to avoid calling
 * sem_post multiple times. */
void update_text_box(unsigned int keycode) {
    sem_t * mutex = 0;
    if (g_text_pointer_context.ui_text->type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM) {
        mutex = g_text_pointer_context.ui_text->interactive_list->thread_bound_control->shared_mutex ? g_text_pointer_context.ui_text->interactive_list->thread_bound_control->shared_mutex : &g_text_pointer_context.ui_text->interactive_list->thread_bound_control->mutex;
    }
    
    if(mutex) {
        sem_wait(mutex);
    }
    
    unsigned int string_size = strlen(g_text_pointer_context.string_buffer);
    char * string_buffer = g_text_pointer_context.string_buffer;
    
    if (keycode == DSTUDIO_KEY_CODE_ERASEBACK) {
        if (g_text_pointer_context.insert_char_index == 0) {
            goto release_mutex;
        }
        
        if (g_text_pointer_context.insert_char_index == string_size) {
            string_buffer[--g_text_pointer_context.insert_char_index] = 0;
        }
        else {
            for (unsigned int i = g_text_pointer_context.insert_char_index; i < string_size; i++) {
                string_buffer[i-1] = string_buffer[i];
            }
            string_buffer[string_size-1] = 0;
            g_text_pointer_context.insert_char_index--;
        }
    }
    else if (keycode == DSTUDIO_KEY_LEFT_ARROW){
        if (g_text_pointer_context.insert_char_index <= 0) {
            goto release_mutex;
        }
        g_text_pointer_context.insert_char_index--;
    }
    else if (keycode == DSTUDIO_KEY_RIGHT_ARROW){
        if (g_text_pointer_context.insert_char_index >= string_size) {
            goto release_mutex;
        }
        g_text_pointer_context.insert_char_index++;
    }
    else if (keycode >= 32 && keycode <= 126) {
        if (string_size + 1 >= g_text_pointer_context.buffer_size) {
            goto release_mutex;
        }
        if (string_size > 0) {
            for (unsigned int i = string_size; i > g_text_pointer_context.insert_char_index; i--) {
                string_buffer[i] = string_buffer[i-1];
            }
        }
        string_buffer[g_text_pointer_context.insert_char_index++] = (char) keycode;
    }
    else {
        goto release_mutex;
    }
    compute_text_pointer_coordinates(g_text_pointer_context.insert_char_index);
    update_text(g_text_pointer_context.ui_text, string_buffer, g_text_pointer_context.buffer_size);
    
    // See? Everything is fine. Shhh, no more tears. Only dreams now.
    release_mutex:
    if(mutex) {
        sem_post(mutex);
    }
    
}

void update_text_pointer() {    
    glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer_context.text_pointer->instance_offsets);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vec4), g_text_pointer_context.text_pointer->instance_offsets_buffer);
    
    glBindBuffer(GL_ARRAY_BUFFER, g_text_pointer_context.text_pointer->instance_alphas);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat), g_text_pointer_context.text_pointer->instance_alphas_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    if (g_text_pointer_context.ui_text->type == DSTUDIO_UI_ELEMENT_TYPE_EDITABLE_LIST_ITEM) {
        g_text_pointer_context.highlight->request_render = 1;
    }
    g_text_pointer_context.ui_text->request_render = 1;
}
