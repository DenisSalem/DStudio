#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#include <semaphore.h>

#include "extensions.h"

typedef struct ThreadControl_t {
    unsigned int    ready;
    unsigned int    update;
    unsigned int    cut_thread;
    sem_t           mutex;
} ThreadControl;

// PATHS
#define DSTUDIO_RESSOURCE_USAGE_PROMPT_ASSET_PATH   "../assets/ressource_usage.png"
#define DSTUDIO_ACTIVE_ARROW_INSTANCES_ASSET_PATH   "../assets/active_arrow_instances.png"
#define DSTUDIO_ACTIVE_BUTTON_ADD_ASSET_PATH        "../assets/active_button_add.png"
#define DSTUDIO_ARROW_INSTANCES_ASSET_PATH          "../assets/arrow_instances.png"
#define DSTUDIO_BUTTON_ADD_ASSET_PATH               "../assets/button_add.png"
#define DSTUDIO_CHAR_TABLE_8X18_ASSET_PATH          "../assets/char_table_8x18.png"
#define DSTUDIO_CHAR_TABLE_4X9_ASSET_PATH           "../assets/char_table_4x9.png"
#define DSTUDIO_FRAGMENT_SHADER_PATH                "../assets/fragment.shader"
#define DSTUDIO_KNOB_1_64x64_TEXTURE_PATH           "../assets/knob1_64x64.png"
#define DSTUDIO_KNOB_1_48x48_TEXTURE_PATH           "../assets/knob1_48x48.png"
#define DSTUDIO_SLIDER_1_10x10_TEXTURE_PATH         "../assets/slider1.png"
#define DSTUDIO_VERTEX_SHADER_PATH                  "../assets/vertex.shader"

// TEXTURES SCALES
#define DSTUDIO_KNOB_1_48_WIDTH  48
#define DSTUDIO_KNOB_1_48_HEIGHT 48
#define DSTUDIO_KNOB_1_48_AREA_WIDTH  DSTUDIO_KNOB_1_48_WIDTH
#define DSTUDIO_KNOB_1_48_AREA_HEIGHT DSTUDIO_KNOB_1_48_HEIGHT

#define DSTUDIO_KNOB_1_64_WIDTH  64
#define DSTUDIO_KNOB_1_64_HEIGHT 64
#define DSTUDIO_KNOB_1_64_AREA_WIDTH  DSTUDIO_KNOB_1_64_WIDTH
#define DSTUDIO_KNOB_1_64_AREA_HEIGHT DSTUDIO_KNOB_1_64_HEIGHT

#define DSTUDIO_RESSOURCE_USAGE_WIDTH 30
#define DSTUDIO_RESSOURCE_USAGE_HEIGHT 23

#define DSTUDIO_SLIDER_1_10_WIDTH 10
#define DSTUDIO_SLIDER_1_10_HEIGHT 10
#define DSTUDIO_SLIDER_1_10_AREA_WIDTH DSTUDIO_SLIDER_1_10_WIDTH
#define DSTUDIO_SLIDER_1_10_AREA_HEIGHT 42

// FLAGS
#define DSTUDIO_FLAG_NONE                   0
#define DSTUDIO_FLAG_FLIP_Y                 1
#define DSTUDIO_FLAG_USE_ALPHA              2
#define DSTUDIO_FLAG_USE_ANTI_ALIASING      4
#define DSTUDIO_FLAG_USE_TEXT_SCALE_MATRIX  8

// MISCELLANEOUS CONSTANTS
#define DSTUDIO_DOUBLE_CLICK_DELAY   0.2
#define DSTUDIO_FRAMERATE 20000
#define DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE 8
#define DSTUDIO_RESSOURCE_USAGE_STRING_SIZE 6
#define DSTUDIO_CHAR_SIZE_DIVISOR 13

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

extern sem_t g_alloc_register_mutex;
extern const char g_application_name[];
void   dstudio_cut_thread(ThreadControl * thread_control);

/*
 * Safely allocate and initialize memory. Automatically stop the program on failure.
 */
void * dstudio_alloc(unsigned int buffer_size);
void   dstudio_free(void * buffer);

double get_timestamp();

#endif
