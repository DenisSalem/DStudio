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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "ui.h"

//~ extern ThreadControl g_open_file_thread_control;

void init_open_menu(UIElements * menu_background, UIElements * ui_elements);

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    unsigned int (*select_callback)(char * path, char * filename, FILE * file_fd)
);

unsigned int select_file_from_list(
    unsigned int index
);

void update_open_file_error(const char * message);

void update_open_file_ui_list();
