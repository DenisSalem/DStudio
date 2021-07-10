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

#ifndef DSTUDIO_OPEN_FILE_H_INCLUDED
#define DSTUDIO_OPEN_FILE_H_INCLUDED

void init_open_menu(UIElements * menu_background, UIElements * ui_elements);

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    uint_fast32_t (*select_callback)(char * path, char * filename, FILE * file_fd),
    uint_fast32_t (*filter_callback)(const char * path, const char * filename)
);

uint_fast32_t select_file_from_list(
    uint_fast32_t index,
    DStudioContextsLevel contexts_level
);

void update_open_file_error(const char * message);

void update_open_file_ui_list();

#endif
