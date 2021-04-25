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

#include "transition_animation.h"

static unsigned int         s_transition_animations_count = 0;
static TransitionAnimation * s_transition_animations = 0;

void allocate_transition_animation(int flags, UIElements * ui_element_p) {
    if (flags & DSTUDIO_FLAG_ANIMATE) {
        unsigned int index = s_transition_animations_count;
        s_transition_animations = dstudio_realloc(s_transition_animations, (++s_transition_animations_count) * sizeof(TransitionAnimation));
        explicit_bzero(&s_transition_animations[index], sizeof(TransitionAnimation));
        s_transition_animations[index].ui_element = ui_element_p;
        
        if (flags & DSTUDIO_FLAG_ANIMATE_OFFSET) {
            s_transition_animations[index].instances_offsets_steps_buffer = dstudio_alloc(sizeof(Vec4) * ui_element_p->count, DSTUDIO_FAILURE_IS_FATAL);
        }
        if (flags & DSTUDIO_FLAG_ANIMATE_ALPHA) {
            s_transition_animations[index].instances_alphas_steps_buffer = dstudio_alloc(sizeof(GLfloat) * ui_element_p->count, DSTUDIO_FAILURE_IS_FATAL);
        }
        if (flags & DSTUDIO_FLAG_ANIMATE_MOTION) {
            s_transition_animations[index].instances_motions_steps_buffer = dstudio_alloc(sizeof(GLfloat) * ui_element_p->count, DSTUDIO_FAILURE_IS_FATAL);
        }
        // Reassign TransisionAnimation adresses because of realloc
        for (unsigned int i = 0; i < s_transition_animations_count; i++) {
            s_transition_animations[i].ui_element->transition_animation = &s_transition_animations[i];
        }
    }
}

void animate_alphas_transitions(GLfloat * target_values, UIElements * ui_element_p) {
    GLfloat * original_values = ui_element_p->instance_alphas_buffer;
    GLfloat * steps = ui_element_p->transition_animation->instances_alphas_steps_buffer;
    for (unsigned int i = 0; i < ui_element_p->count; i++) {
        steps[i] = (target_values[i] - original_values[i]) / (GLfloat) DSTUDIO_TRANSITION_STEPS;
    }

    ui_element_p->transition_animation->iterations = DSTUDIO_TRANSITION_STEPS;
    ui_element_p->transition_animation->flags |= DSTUDIO_ALPHA_TRANSITION;
}

void animate_motions_transitions(GLfloat * target_values, UIElements * ui_element_p) {
    GLfloat * original_values = ui_element_p->instance_motions_buffer;
    GLfloat * steps = ui_element_p->transition_animation->instances_motions_steps_buffer;
    
    for (unsigned int i = 0; i < ui_element_p->count; i++) {
        steps[i] = (target_values[i] - original_values[i]) / (GLfloat) DSTUDIO_TRANSITION_STEPS;

    }

    ui_element_p->transition_animation->flags |= DSTUDIO_MOTION_TRANSITION;
    ui_element_p->transition_animation->iterations = DSTUDIO_TRANSITION_STEPS;
}

void animate_offsets_transitions(Vec4 * target_values, UIElements * ui_element_p, unsigned int member_flag) {
    Vec4 * original_values = ui_element_p->coordinates_settings.instance_offsets_buffer;
    Vec4 * steps = ui_element_p->transition_animation->instances_offsets_steps_buffer;
    
    for (unsigned int i = 0; i < ui_element_p->count; i++) {
        if (member_flag & DSTUDIO_OFFSET_X_TRANSITION)
            steps[i].x = (target_values[i].x - original_values[i].x) / (GLfloat) DSTUDIO_TRANSITION_STEPS;

        if (member_flag & DSTUDIO_OFFSET_Y_TRANSITION)
            steps[i].y = (target_values[i].y - original_values[i].y) / (GLfloat) DSTUDIO_TRANSITION_STEPS;

        if (member_flag & DSTUDIO_OFFSET_Z_TRANSITION)
            steps[i].z = (target_values[i].z - original_values[i].z) / (GLfloat) DSTUDIO_TRANSITION_STEPS;

        if (member_flag & DSTUDIO_OFFSET_W_TRANSITION)
            steps[i].w = (target_values[i].w - original_values[i].w) / (GLfloat) DSTUDIO_TRANSITION_STEPS;
    }
    ui_element_p->transition_animation->flags |= (member_flag & 0xF);
    ui_element_p->transition_animation->iterations = DSTUDIO_TRANSITION_STEPS;
}


void perform_transition_animation() {
    Vec4 * offsets_buffer = 0;
    Vec4 * offsets_steps = 0;
    GLfloat * motions_buffer = 0;
    GLfloat * motions_steps = 0;
    GLfloat * alphas_buffer = 0;
    GLfloat * alphas_steps = 0;
    
    for (unsigned int index = 0; index < s_transition_animations_count; index++) {
        if (s_transition_animations[index].iterations > 0) {
            if (s_transition_animations[index].flags & (DSTUDIO_OFFSET_X_TRANSITION | DSTUDIO_OFFSET_Y_TRANSITION | DSTUDIO_OFFSET_Z_TRANSITION | DSTUDIO_OFFSET_W_TRANSITION)) {
                offsets_buffer = s_transition_animations[index].ui_element->coordinates_settings.instance_offsets_buffer;
                offsets_steps = s_transition_animations[index].instances_offsets_steps_buffer;
                
                if (s_transition_animations[index].flags & DSTUDIO_OFFSET_X_TRANSITION) {
                    for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++)
                        offsets_buffer[i].x += offsets_steps[i].x;
                }
                
                if (s_transition_animations[index].flags & DSTUDIO_OFFSET_Y_TRANSITION) {
                    for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++)
                        offsets_buffer[i].y += offsets_steps[i].y;
                }

                if (s_transition_animations[index].flags & DSTUDIO_OFFSET_Z_TRANSITION) {
                    for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++)
                        offsets_buffer[i].z += offsets_steps[i].z;
                }

                if (s_transition_animations[index].flags & DSTUDIO_OFFSET_W_TRANSITION) {
                    for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++)
                        offsets_buffer[i].w += offsets_steps[i].w;
                }
            }
            
            if (s_transition_animations[index].flags & DSTUDIO_MOTION_TRANSITION) {
                motions_buffer = s_transition_animations[index].ui_element->instance_motions_buffer;
                motions_steps = s_transition_animations[index].instances_motions_steps_buffer;
                for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++) 
                    motions_buffer[i] += motions_steps[i];
            }
            
            if (s_transition_animations[index].flags & DSTUDIO_ALPHA_TRANSITION) {
                alphas_buffer = s_transition_animations[index].ui_element->instance_alphas_buffer;
                alphas_steps = s_transition_animations[index].instances_alphas_steps_buffer;
                for (unsigned int i = 0; i < s_transition_animations[index].ui_element->count; i++)
                    alphas_buffer[i] += alphas_steps[i];
            }

            s_transition_animations[index].ui_element->render_state = DSTUDIO_UI_ELEMENT_UPDATE_AND_RENDER_REQUESTED;
            s_transition_animations[index].iterations--;    
        }
        else {
            s_transition_animations[index].flags = DSTUDIO_NONE_TRANSITION;
        }
    }
}

