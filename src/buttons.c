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
#include <stdio.h>
#include <unistd.h>

#include "common.h"
#include "extensions.h"
#include "buttons.h"

void * buttons_management_thread(void * args) {
    ButtonsManagement * buttons_management = (ButtonsManagement *) args;
    double timestamp = 0;
    double elapsed_time;
    unsigned int count = 0;
    while(!buttons_management->ready) {
        usleep(1000);
    }
    count = buttons_management->count;
    while(!buttons_management->cut_thread) {
        usleep(20000);
        sem_wait(&buttons_management->mutex);
        for (unsigned int i = 0; i < count; i++) {
            timestamp = buttons_management->states[i].timestamp;
            if (timestamp == 0) {
                continue;
            }
            elapsed_time = get_timestamp() - buttons_management->states[i].timestamp;
            
            if (elapsed_time > 1 && buttons_management->states[i].type == DSTUDIO_BUTTON_TYPE_1) {
                send_expose_event();
            }
        }
        sem_post(&buttons_management->mutex);
    }
    return NULL;
}

void check_for_buttons_to_render_n_update(ButtonsManagement * buttons_management, void (*render_viewport)(int)) {
    sem_wait(&buttons_management->mutex);
    unsigned int count = buttons_management->count;
    for (unsigned int i=0; i < count; i++) {
        if (buttons_management->states[i].timestamp != 0) {
            buttons_management->states[i].timestamp = 0;
            glScissor(0, 0, DSTUDIO_VIEWPORT_WIDTH, DSTUDIO_VIEWPORT_HEIGHT);
            render_viewport(DSTUDIO_RENDER_ALL);        }
    }
    sem_post(&buttons_management->mutex);
}

void init_buttons_management(ButtonsManagement * buttons_management, ButtonStates * button_states_array, unsigned int count) {
    buttons_management->states = button_states_array;
    printf("init count = %d\n", count);
    buttons_management->count = count;
    buttons_management->ready = 1;
    sem_init(&buttons_management->mutex, 0, 1);
}
 
void update_button(int index, UIElements * buttons_p, void * args) {
    (void) index;
    ButtonStates * button_states = (ButtonStates *) args;
    button_states->timestamp = get_timestamp();
    if (button_states->active == buttons_p->texture_id) {
        buttons_p->texture_id = button_states->release;
    }
    else {
        buttons_p->texture_id = button_states->active;
    }
}
