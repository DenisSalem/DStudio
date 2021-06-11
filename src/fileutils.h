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

#include <stdint.h>

void count_instances(
    const char * directory,
    uint_fast32_t * count,
    uint_fast32_t * last_id
);

uint_fast32_t count_process(
    const char * process_name
);

void dstudio_canonize_path(
    char ** src
);

void dstudio_expand_user(
    char ** dest,
    const char * directory
);

int dstudio_is_directory(
    char * path
);

double get_proc_memory_usage();

void recursive_mkdir(
    char * directory
);

int is_directory(const char *path);

int set_physical_memory();
