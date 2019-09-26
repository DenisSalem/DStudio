#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#include "extensions.h"

#define DSTUDIO_RETURN_IF_FAILURE(value) \
    if ( (value) != 0 ) { \
        printf("!" #value "\n"); \
        return -1; \
    }

#define DSTUDIO_EXIT_IF_FAILURE(value) \
    if ((value) != 0) { \
        printf("!" #value "\n"); \
        exit(-1); \
    }

#define DSTUDIO_EXIT_IF_NULL(value) \
    if ((value) == 0) { \
        printf("0 == " #value "\n"); \
        exit(-1); \
    }

#define SET_SCISSOR \
    scissor_x = (GLint) ui_areas[i].min_x; \
    scissor_y = (GLint) DSTUDIO_VIEWPORT_HEIGHT - ui_areas[i].max_y; \
    scissor_width = (GLsizei) ui_areas[i].max_x - ui_areas[i].min_x; \
    scissor_height = (GLsizei) ui_areas[i].max_y - ui_areas[i].min_y;
    
extern GLint scissor_x, scissor_y;
extern GLsizei scissor_width, scissor_height;

double get_timestamp();
    
#endif
