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
flat in vec2 no_texture_args;

in vec2 fragment_texture_coordinates;
out vec4 color;

void main (void) {
    if (no_texture_args.x == 1.0) {
        color = vec4(1, 0.5, 0, no_texture_args.y);
        //color = vec4(1, 0.5, 0, 1);

    }
    else {
        color = texture(texture_knob, fragment_texture_coordinates);
    }
} 
