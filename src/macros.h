/*
 * Copyright 2019, 2023 Denis Salem
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

#ifndef DSTUDIO_MACROS_H_INCLUDED
#define DSTUDIO_MACROS_H_INCLUDED

#define DSTUDIO_EXIT_IF_FAILURE(value) \
    if ((value) != 0) { \
        printf("%s:%d !" #value "\n", __FILE__, __LINE__); \
        exit(-1); \
    }

#define DSTUDIO_EXIT_IF_NULL(value) \
    if ((value) == 0) { \
        printf("0 == " #value "\n"); \
        exit(-1); \
    }

#define DSTUDIO_RETURN_IF_FAILURE(value) \
    if ( (value) != 0 ) { \
        printf("%s:%d !" #value "\n", __FILE__, __LINE__); \
        return -1; \
    }

#define DSTUDIO_TRACE \
    printf("%s %d:\n", __FILE__, __LINE__);

#define DSTUDIO_TRACE_STR(string) \
    printf("%s %d: " #string "\n", __FILE__, __LINE__);

#define DSTUDIO_TRACE_ARGS(string, ...) \
    printf("%s %d: " #string "\n", __FILE__, __LINE__, __VA_ARGS__);
    
#endif
