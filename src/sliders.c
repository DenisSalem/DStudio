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

#include <stdlib.h>
#include "extensions.h"

#include "sliders.h"

void init_slider_settings(
    UIElementSetting ** settings_p,
    unsigned int texture_scale,
    GLfloat x,
    GLfloat y,
    unsigned int offset,
    unsigned int slide,
    unsigned int count
) {
    *settings_p = malloc(count * sizeof(UIElementSetting));
    UIElementSetting * settings = *settings_p;
    settings->gl_x = (GLfloat) (x - (DSTUDIO_VIEWPORT_WIDTH >> 1) + (texture_scale >>1)) / (GLfloat) (DSTUDIO_VIEWPORT_WIDTH >> 1);
    settings->gl_y = (GLfloat) ((DSTUDIO_VIEWPORT_HEIGHT >> 1) - y + (texture_scale >> 1)) / (GLfloat) (DSTUDIO_VIEWPORT_HEIGHT >> 1);
    settings->min_area_x = x - 1.0;
    settings->max_area_x = x + (float) texture_scale + 1.0;
    settings->min_area_y = (GLfloat) y - slide -1 - texture_scale;
    settings->max_area_y = (GLfloat) y + slide + 1;
    
    settings->ui_element_type = DSTUDIO_SLIDER_TYPE_1;
    GLfloat gl_offset = (GLfloat) offset / (GLfloat) (DSTUDIO_VIEWPORT_WIDTH >> 1);
    for (unsigned int i = 0 ; i < count - 1; i++){
        memcpy(&settings[i+1], &settings[i], sizeof(UIElementSetting));
        settings[i+1].gl_x += gl_offset;
        settings[i+1].min_area_x += offset;
        settings[i+1].max_area_x += offset;
    }
}

void update_slider(int index, UIElements * sliders_p, void * args) {
    float * motion = (float*) args;
    sliders_p->instance_motions_buffer[index] = *motion;
    glBindBuffer(GL_ARRAY_BUFFER, sliders_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * sliders_p->count, sliders_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
