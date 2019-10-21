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

#version 330

layout(location = 0) in vec2  vertex_position;
layout(location = 1) in vec2  texture_coordinates;
layout(location = 2) in vec2  translation;
layout(location = 3) in float motion;

uniform mat2 scale_matrix;
uniform float motion_type;
flat out vec2 no_texture_args;
out vec2 fragment_texture_coordinates;

void main() {
    no_texture_args = vec2(0,0);
    if (motion_type == 0.0) {
        vec2 applied_rotation; 
        float c = cos(motion);
        float s = sin(motion);
    
        if (motion != 0) {  
            applied_rotation.x = vertex_position.x * c - vertex_position.y * s;
            applied_rotation.y = vertex_position.x * s + vertex_position.y * c;
        }
        else {
            applied_rotation = vertex_position.xy;
        }
    
        gl_Position = vec4( scale_matrix * applied_rotation + translation, 0, 1.0);
    }
    else {
            if (motion != 0) {
                gl_Position = vec4( scale_matrix * vertex_position.xy + translation + vec2(0, motion), 0, 1.0);
            }
            else {
                gl_Position = vec4( scale_matrix * vertex_position.xy + translation, 0, 1.0);
            }
    }
    fragment_texture_coordinates = texture_coordinates;
} 
