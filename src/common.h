#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#include <semaphore.h>

#include "extensions.h"

// PATHS
#define DSTUDIO_FRAGMENT_SHADER_PATH                "../assets/fragment.shader"
#define DSTUDIO_KNOB_1_64x64_TEXTURE_PATH           "../assets/knob1_64x64.png"
#define DSTUDIO_KNOB_1_48x48_TEXTURE_PATH           "../assets/knob1_48x48.png"
#define DSTUDIO_VERTEX_SHADER_PATH                  "../assets/vertex.shader"
#define DSTUDIO_CHAR_TABLE_ASSET_PATH               "../assets/char_table.png"
#define DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH         "../assets/char_table_small.png"
#define DSTUDIO_ARROW_INSTANCES_ASSET_PATH          "../assets/arrow_instances.png"
#define DSTUDIO_ACTIVE_ARROW_INSTANCES_ASSET_PATH   "../assets/active_arrow_instances.png"
#define DSTUDIO_BUTTON_ADD_ASSET_PATH               "../assets/button_add.png"
#define DSTUDIO_ACTIVE_BUTTON_ADD_ASSET_PATH        "../assets/active_button_add.png"

// TEXTURES SCALES
#define DSTUDIO_KNOB_1_64_WIDTH  64
#define DSTUDIO_KNOB_1_64_HEIGHT 64
#define DSTUDIO_KNOB_1_64_AREA_WIDTH  DSTUDIO_KNOB_1_64_WIDTH
#define DSTUDIO_KNOB_1_64_AREA_HEIGHT DSTUDIO_KNOB_1_64_HEIGHT
#define DSTUDIO_KNOB_1_48_WIDTH  48
#define DSTUDIO_KNOB_1_48_HEIGHT 48
#define DSTUDIO_KNOB_1_48_AREA_WIDTH  DSTUDIO_KNOB_1_48_WIDTH
#define DSTUDIO_KNOB_1_48_AREA_HEIGHT DSTUDIO_KNOB_1_48_HEIGHT

// FLAGS
#define DSTUDIO_FLAG_NONE               0
#define DSTUDIO_FLAG_FLIP_Y             1
#define DSTUDIO_FLAG_USE_ALPHA          2
#define DSTUDIO_FLAG_USE_ANTI_ALIASING  4

// MISCELLANEOUS CONSTANTS
#define DSTUDIO_DOUBLE_CLICK_DELAY   0.2
#define DSTUDIO_FRAMERATE 20000
#define DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE 8

// USEFUL MACROS
#define DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX(matrix, width, height) \
    matrix[0].x = ((float) width / (float) DSTUDIO_VIEWPORT_WIDTH); \
    matrix[1].y = ((float) height / (float) DSTUDIO_VIEWPORT_HEIGHT);

#define DSTUDIO_SET_UI_TEXT_SCALE_MATRIX(matrix, width, height) \
    matrix[0].x = ((float) width / (float) DSTUDIO_VIEWPORT_WIDTH) / DSTUDIO_CHAR_SIZE_DIVISOR; \
    matrix[1].y = ((float) height / (float) DSTUDIO_VIEWPORT_HEIGHT) / DSTUDIO_CHAR_SIZE_DIVISOR;

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

#define DSTUDIO_DYNAMIC_ALLOCATION(dest, size) \
    dest = malloc(size); \
    explicit_bzero(dest, size);

/*
 * Safely allocate and initialize memory. Automatically stop the program on failure.
 */
void * dstudio_alloc(unsigned int buffer_size);
void dstudio_free(void * buffer);

extern sem_t g_alloc_register_mutex;
extern const char g_application_name[];
double get_timestamp();

#endif
