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
#include "math.h"

#include "knob.h"


void bind_and_update_ui_knob(UIElements * knob, void * callback_args) {
    knob->application_callback_args = callback_args;
    update_ui_knob(knob);
}

void update_knob_value(UIElements * knob) {
    KnobValue * knob_value = knob->application_callback_args;
    float value = 0;
    if (knob_value == NULL) {
        return;
    }
    
    value = knob_value->offset + knob_value->multiplier * (1.0 - (*knob->instance_motions_buffer - KNOB_LOWEST_POSITION) / (2.0 * (GLfloat) KNOB_HIGHEST_POSITION));

    switch(knob_value->sensitivity) {
        case DSTUDIO_KNOB_SENSITIVITY_LINEAR:
            knob_value->computed = value;
            break;
            
        case DSTUDIO_KNOB_SENSITIVITY_LOGARITHMIC:
            break;
            
        case DSTUDIO_KNOB_SENSITIVITY_EXPONENTIAL:
            break;
    }
}

void update_ui_knob(UIElements * knob) {
    float rotation = 0;
    
    KnobValue default_value = {1.0, 0, 0.5, DSTUDIO_KNOB_SENSITIVITY_LINEAR};
    
    KnobValue * knob_value = knob->application_callback_args == 0 ? &default_value : knob->application_callback_args;
    float value = 0;
    switch(knob_value->sensitivity) {
        case DSTUDIO_KNOB_SENSITIVITY_LINEAR:
            value = -(knob_value->computed - knob_value->offset) / knob_value->multiplier;
            break;
            
        case DSTUDIO_KNOB_SENSITIVITY_LOGARITHMIC:
            break;
            
        case DSTUDIO_KNOB_SENSITIVITY_EXPONENTIAL:
            break;
    }
    rotation = -KNOB_LOWEST_POSITION + (2.0 * KNOB_HIGHEST_POSITION) * value;
    
    knob->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
    if (knob->transition_animation) {
        animate_motions_transitions(&rotation, knob);
    }
    else {
        *knob->instance_motions_buffer = rotation;
    }
}
