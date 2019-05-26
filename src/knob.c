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

#include "knob.h"

void new_knob(Knob * knob, float width, float height, float x, float y) {
    VertexAttributes * vertexes_attributes = knob->vertexes_attributes;
    GLuint * indices = knob->indices;
    
    vertexes_attributes[0].x = -1.0f;
    vertexes_attributes[0].y =  1.0f;
    vertexes_attributes[1].x = -1.0f;
    vertexes_attributes[1].y = -1.0f;
    vertexes_attributes[2].x =  1.0f;
    vertexes_attributes[2].y =  1.0f;   
    vertexes_attributes[3].x =  1.0f;
    vertexes_attributes[3].y = -1.0f;
    
    vertexes_attributes[0].s = 0.0f;
    vertexes_attributes[0].t = 0.0f;
    vertexes_attributes[1].s = 0.0f;
    vertexes_attributes[1].t = 1.0f;
    vertexes_attributes[2].s = 1.0f;
    vertexes_attributes[2].t = 0.0f;
    vertexes_attributes[3].s = 1.0f;
    vertexes_attributes[3].t = 1.0f;
    
    indices[0] = 
}
