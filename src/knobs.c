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
#include "knobs.h"

void init_knob(UIElements * knobs, int index, float offset_x, float offset_y) {    
    init_ui_element(&knobs->instance_offsets_buffer[index], offset_x, offset_y, &knobs->instance_motions_buffer[index]);
}

void render_knobs(UIElements * knobs) {    
    render_ui_elements(knobs->texture_id, knobs->vertex_array_object, knobs->index_buffer_object, knobs->count);
}

void update_knob(int index, UIElements * knobs_p, void * args) {
    float * motion = (float*) args;
    knobs_p->instance_motions_buffer[index] = *motion;
    glBindBuffer(GL_ARRAY_BUFFER, knobs_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * knobs_p->count, knobs_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
