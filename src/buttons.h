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

#ifndef DSTUDIO_BUTTONS_H_INCLUDED
#define DSTUDIO_BUTTONS_H_INCLUDED

#include <pthread.h>
#include <semaphore.h>

#include "ui.h"

typedef struct ButtonStates_t {
    GLuint active;
    GLuint release;
    double timestamp;
    void (*application_callback)(void * args);
    unsigned int flags;
} ButtonStates;

typedef struct ButtonsManagement_t {
    ButtonStates * states;
    UICallback * callbacks;
    int cut_thread;
    sem_t mutex;
    pthread_t thread_id;
    int ready;
    unsigned int count;
} ButtonsManagement;

void * buttons_management_thread(void * args);

void check_for_buttons_to_render_n_update(
    ButtonsManagement * buttons_management,
    UICallback * ui_callbacks,
    int mouse_state,
    UIArea * ui_areas);
    
void init_buttons_management(
    ButtonsManagement * buttons_management,
    ButtonStates * button_states_array,
    UICallback * callbacks,
    unsigned int count);

void update_button(
    int index,
    UIElements * buttons_p,
    void * args);

#endif
