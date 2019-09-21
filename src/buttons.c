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

#include "extensions.h"
#include "buttons.h"

void check_for_buttons_to_render_n_update(ButtonStates * button_states, int count, void (*render)(int)) {
    (void) button_states;
    (void) render;
    for (int i = 0; i < count; i++) {
        printf("active button: %ld\n", button_states[i].timestamp);
    }           
    printf("\n"); 
}

void update_button(int index, UIElements * buttons_p, void * args) {
    (void) index;
    ButtonStates * button_states = (ButtonStates *) args;
    button_states->timestamp = clock();
    if (button_states->active == buttons_p->texture_id) {
        buttons_p->texture_id = button_states->release;
    }
    else {
        buttons_p->texture_id = button_states->active;
    }
    printf("texture id %d\n", buttons_p->texture_id);
}
