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

uniform sampler2D texture_knob;
uniform int no_texture;

in vec2 fragment_texture_coordinates;
out vec4 color;

void main (void) {
    if (no_texture != 0) {
        color = vec4(0,0,0,fragment_texture_coordinates.x);
    }
    else {
        color = texture(texture_knob, fragment_texture_coordinates);
    }
} 
