/*
 * Copyright 2019, 2020 Denis Salem
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

#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#include <semaphore.h>

#include "extensions.h"

typedef struct ThreadControl_t {
    unsigned int    ready;
    unsigned int    update;
    unsigned int    cut_thread;
    sem_t           mutex;
    sem_t *         shared_mutex;
} ThreadControl;


// MISCELLANEOUS CONSTANTS
#define DSTUDIO_DOUBLE_CLICK_DELAY   0.2
#define DSTUDIO_FRAMERATE 20000
#define DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE 8
#define DSTUDIO_RESSOURCE_USAGE_STRING_SIZE 6
#define DSTUDIO_CHAR_SIZE_DIVISOR 16.0
#define DSTUDIO_OPEN_FILE_CHAR_PER_LINE ((int) (g_dstudio_viewport_width - 62) / 8)
#define DSTUDIO_OPEN_FILE_BASE_UI_ELEMENTS_COUNT 9
#define DSTUDIO_INSTANCE_NAME_LENGTH 27
// PATHS
#define DSTUDIO_ACTIVE_ADD_INSTANCE_ASSET_PATH      "../assets/active_button_add_instance.png"
#define DSTUDIO_ACTIVE_ADD_SAMPLE_ASSET_PATH        "../assets/active_button_add_sample.png"
#define DSTUDIO_ACTIVE_ADD_VOICE_ASSET_PATH         "../assets/active_button_add_voice.png"
#define DSTUDIO_ACTIVE_BUTTON_ADD_ASSET_PATH        "../assets/active_button_add.png"
#define DSTUDIO_ACTIVE_CANCEL_BUTTON_ASSET_PATH     "../assets/active_button_cancel.png"
#define DSTUDIO_ACTIVE_OPEN_FILE_BUTTON_ASSET_PATH  "../assets/active_button_open.png"
#define DSTUDIO_ADD_INSTANCE_ASSET_PATH             "../assets/button_add_instance.png"
#define DSTUDIO_ADD_SAMPLE_ASSET_PATH               "../assets/button_add_sample.png"
#define DSTUDIO_ADD_VOICE_ASSET_PATH                "../assets/button_add_voice.png"
#define DSTUDIO_BACKGROUND_MENU_PATTERN_ASSET_PATH  "../assets/background_menu_pattern.png"
#define DSTUDIO_BUTTON_ADD_ASSET_PATH               "../assets/button_add.png"
#define DSTUDIO_CANCEL_BUTTON_ASSET_PATH            "../assets/button_cancel.png"
#define DSTUDIO_CHAR_TABLE_8X18_ASSET_PATH          "../assets/char_table_8x18.png"
#define DSTUDIO_CHAR_TABLE_4X9_ASSET_PATH           "../assets/char_table_4x9.png"
#define DSTUDIO_FRAGMENT_SHADER_PATH                "../assets/fragment.shader"
#define DSTUDIO_KNOB_1_64x64_TEXTURE_PATH           "../assets/knob1_64x64.png"
#define DSTUDIO_KNOB_1_48x48_TEXTURE_PATH           "../assets/knob1_48x48.png"
#define DSTUDIO_LIST_ITEM_HIGHLIGHT_PATTERN_PATH    "../assets/list_item_highlight_pattern.png"
#define DSTUDIO_OPEN_FILE_BUTTON_ASSET_PATH         "../assets/button_open.png"
#define DSTUDIO_OPEN_FILE_SLIDER_ASSET_PATH         "../assets/slider2.png"
#define DSTUDIO_RESSOURCE_USAGE_PROMPT_ASSET_PATH   "../assets/ressource_usage.png"
#define DSTUDIO_SLIDER_1_10x10_TEXTURE_PATH         "../assets/slider1.png"
#define DSTUDIO_SLIDER_2_7x7_TEXTURE_PATH           "../assets/slider3.png"
#define DSTUDIO_SLIDER_BACKGROUND_ASSET_PATH        "../assets/slider_background.png"
#define DSTUDIO_VERTEX_SHADER_PATH                  "../assets/vertex.shader"

// TEXTURES SCALES

#define DSTUDIO_ADD_INSTANCE_WIDTH 68
#define DSTUDIO_ADD_INSTANCE_HEIGHT 15

#define DSTUDIO_ADD_SAMPLE_WIDTH 68
#define DSTUDIO_ADD_SAMPLE_HEIGHT 15

#define DSTUDIO_ADD_VOICE_WIDTH 68
#define DSTUDIO_ADD_VOICE_HEIGHT 15

#define DSTUDIO_CANCEL_BUTTON_COLUMNS 1
#define DSTUDIO_CANCEL_BUTTON_COUNT 1
#define DSTUDIO_CANCEL_BUTTON_OFFSET_X 252
#define DSTUDIO_CANCEL_BUTTON_OFFSET_Y 38

#define DSTUDIO_CHAR_TABLE_4X9_WIDTH 64
#define DSTUDIO_CHAR_TABLE_4X9_HEIGHT 144
#define DSTUDIO_CHAR_TABLE_8X18_WIDTH 128
#define DSTUDIO_CHAR_TABLE_8X18_HEIGHT 288

#define DSTUDIO_KNOB_1_48_WIDTH  48
#define DSTUDIO_KNOB_1_48_HEIGHT 48
#define DSTUDIO_KNOB_1_48_AREA_WIDTH  DSTUDIO_KNOB_1_48_WIDTH
#define DSTUDIO_KNOB_1_48_AREA_HEIGHT DSTUDIO_KNOB_1_48_HEIGHT

#define DSTUDIO_KNOB_1_64_WIDTH  64
#define DSTUDIO_KNOB_1_64_HEIGHT 64
#define DSTUDIO_KNOB_1_64_AREA_WIDTH  DSTUDIO_KNOB_1_64_WIDTH
#define DSTUDIO_KNOB_1_64_AREA_HEIGHT DSTUDIO_KNOB_1_64_HEIGHT

#define DSTUDIO_OPEN_FILE_BUTTON_COLUMNS 1
#define DSTUDIO_OPEN_FILE_BUTTON_COUNT 1
#define DSTUDIO_OPEN_FILE_BUTTON_OFFSET_X 92
#define DSTUDIO_OPEN_FILE_BUTTON_OFFSET_Y 19
#define DSTUDIO_OPEN_FILE_BUTTONS_WIDTH 68
#define DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT 15

#define DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT (g_dstudio_viewport_height - 76)
#define DSTUDIO_OPEN_FILE_LIST_OFFSET_X 32
#define DSTUDIO_OPEN_FILE_LIST_HIGHLIGHT_POS_X (-0.97 + (GLfloat) (g_dstudio_viewport_width - 62) / (GLfloat) g_dstudio_viewport_width)
#define DSTUDIO_OPEN_FILE_LIST_HIGHLIGHT_OFFSET_Y (-18.0 / (GLfloat) (g_dstudio_viewport_height >> 1))

#define DSTUDIO_OPEN_FILE_PROMPT_COLUMN 1
#define DSTUDIO_OPEN_FILE_PROMPT_COUNT 1
#define DSTUDIO_OPEN_FILE_PROMPT_OFFSET_X 32
#define DSTUDIO_OPEN_FILE_PROMPT_OFFSET_Y 38
#define DSTUDIO_OPEN_FILE_PROMPT_AREA_WIDTH 72
#define DSTUDIO_OPEN_FILE_PROMPT_AREA_HEIGHT 10
#define DSTUDIO_OPEN_FILE_PROMPT_BUFFER_SIZE 9

#define DSTUDIO_OPEN_FILE_SLIDER_WIDTH 18
#define DSTUDIO_OPEN_FILE_SLIDER_HEIGHT 18

#define DSTUDIO_OPEN_FILE_SLIDER_BACKGROUND_OFFSET_X 42

#define DSTUDIO_OPEN_FILE_PROMPT_BOX_OFFSET_Y 38
#define DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT 26

#define DSTUDIO_PATTERN_SCALE 16

#define DSTUDIO_RESSOURCE_USAGE_WIDTH 30
#define DSTUDIO_RESSOURCE_USAGE_HEIGHT 23

#define DSTUDIO_SLIDER_1_10_WIDTH 10
#define DSTUDIO_SLIDER_1_10_HEIGHT 10
#define DSTUDIO_SLIDER_1_10_AREA_WIDTH DSTUDIO_SLIDER_1_10_WIDTH
#define DSTUDIO_SLIDER_1_10_AREA_HEIGHT 42

#define DSTUDIO_SLIDER_2_7_WIDTH 7
#define DSTUDIO_SLIDER_2_7_HEIGHT 7
#define DSTUDIO_SLIDER_2_7_AREA_WIDTH DSTUDIO_SLIDER_1_10_WIDTH
#define DSTUDIO_SLIDER_2_7_AREA_HEIGHT 95

#define DSTUDIO_SLIDER_BACKGROUND_WIDTH 18
#define DSTUDIO_SLIDER_BACKGROUND_HEIGHT 27

#define DSTUDIO_TEXT_POINTER_WIDTH 1
#define DSTUDIO_TEXT_POINTER_1_HEIGHT DSTUDIO_CHAR_TABLE_4X9_HEIGHT / 13
#define DSTUDIO_TEXT_POINTER_2_HEIGHT DSTUDIO_CHAR_TABLE_8X18_HEIGHT / 13

#define DSANDGRAINS_TINY_BUTTON_SCALE 32

// FLAGS
#define DSTUDIO_FLAG_NONE                           0
#define DSTUDIO_FLAG_FLIP_Y                         1
#define DSTUDIO_FLAG_USE_ALPHA                      2
#define DSTUDIO_FLAG_USE_ANTI_ALIASING              4
#define DSTUDIO_FLAG_USE_TEXT_SETTING               8
#define DSTUDIO_FLAG_TEXTURE_IS_PATTERN             16
#define DSTUDIO_FLAG_IS_VISIBLE                     32
#define DSTUDIO_FLAG_USE_SLIDER_BACKGROUND_SETTING  64
#define DSTUDIO_FLAG_SLIDER_TO_TOP                  128

#define DSTUDIO_USE_MUTEX                   1
#define DSTUDIO_DO_NOT_USE_MUTEX            0

#define DEFINE_SCALE_MATRIX(scale_matrix, width, height) \
        scale_matrix[0].x = (GLfloat) width / (GLfloat) g_dstudio_viewport_width;\
        scale_matrix[0].y = 0; \
        scale_matrix[1].x = 0; \
        scale_matrix[1].y = (GLfloat) height / (GLfloat) g_dstudio_viewport_height;

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
    printf("%s %d\n", __FILE__, __LINE__);


    
extern const char g_application_name[];

/*
 * Safely allocate and initialize memory. Automatically stop the program on failure.
 */
void * dstudio_alloc(unsigned int buffer_size);
void   dstudio_cut_thread(ThreadControl * thread_control);
void   dstudio_free(void * buffer);
void dstudio_init_memory_management();
void * dstudio_realloc(void * buffer, unsigned int new_size);
double get_timestamp();

#endif
