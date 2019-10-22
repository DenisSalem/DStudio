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

layout(location = 0) in vec2 vertex_position;
layout(location = 1) in vec2 texture_coordinates;
layout(location = 2) in vec4 offset;

out vec2 fragment_texture_coordinates;
flat out vec2 no_texture_args;

uniform mat2 scale_matrix;
uniform float no_texture;

void main() {
    gl_Position = vec4(scale_matrix * vertex_position + offset.xy, 0, 1.0);
    if (no_texture == 1.0) {
        no_texture_args = vec2(1.0, offset.z);
    }
    else {
        no_texture_args = vec2(0.0);
        fragment_texture_coordinates = texture_coordinates + offset.zw;
    }
} 
