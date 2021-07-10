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

#ifndef DSANDGRAINS_ADD_H_INCLUDED
#define DSANDGRAINS_ADD_H_INCLUDED

void add_instance(UIElements * ui_elements);
void add_sample(UIElements * ui_elements);
void add_sub_menu(UIElements * ui_elements);

/*
 * Same as add_sub_menu but without arguments.
 */
 
void add_sub_menu_proxy();

void add_voice(UIElements * ui_elements);
void close_add_sub_menu();
uint_fast32_t filter_non_audio_file(const char * path, const char * filename);

#endif
