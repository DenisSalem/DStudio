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

void update_slider(int index, UIElements * sliders_p, void * args) {
    float * motion = (float*) args;
    sliders_p->instance_motions_buffer[index] = *motion;
    glBindBuffer(GL_ARRAY_BUFFER, sliders_p->instance_motions);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * sliders_p->count, sliders_p->instance_motions_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
