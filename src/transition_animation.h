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

#ifndef DSTUDIO_TRANSITION_ANIMATION_H_INCLUDED
#define DSTUDIO_TRANSITION_ANIMATION_H_INCLUDED

/*
 * A step every DSTUDIO_FRAMERATE = 33.33333, repeated
 * DSTUDIO_TRANSITION_STEPS = 4 times means
 * the animation duration is ~0.125s.
 **/

#define DSTUDIO_TRANSITION_STEPS 4

typedef struct UIElements_t UIElements;
typedef struct Vec4_t Vec4;

typedef struct TransitionAnimation_t {
    UIElements * ui_element;
    float * instances_motions_steps_buffer;
    float * instances_alphas_steps_buffer;
    Vec4 * instances_offsets_steps_buffer;
    uint_fast32_t iterations;
    uint_fast32_t flags;
} TransitionAnimation;

typedef enum TransitionAnimationOffsetsMember_t {
    DSTUDIO_NONE_TRANSITION = 0,
    DSTUDIO_OFFSET_X_TRANSITION = 1,
    DSTUDIO_OFFSET_Y_TRANSITION = 2,
    DSTUDIO_OFFSET_Z_TRANSITION = 4,
    DSTUDIO_OFFSET_W_TRANSITION = 8,
    DSTUDIO_MOTION_TRANSITION = 16,
    DSTUDIO_ALPHA_TRANSITION = 32,
} TransitionAnimationOffsetsMember;

void allocate_transition_animation(uint_fast32_t flags, UIElements * ui_element_p);

void animate_alphas_transitions(GLfloat * target_values, UIElements * ui_element_p);
void animate_motions_transitions(GLfloat * target_values, UIElements * ui_element_p);
void animate_offsets_transitions(Vec4 * target_values, UIElements * ui_element_p, uint_fast32_t member_flag);
void perform_transition_animation();

#endif
