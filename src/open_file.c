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

#include <dirent.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>

#include "fileutils.h"
#include "open_file.h"
#include "text.h"
#include "ui.h"
#include "window_management.h"


ThreadControl g_open_file_thread_control = {0};

static void (*s_cancel_callback)(UIElements * ui_elements) = 0;
static void (*s_select_callback)(FILE * file_fd) = 0;
static UIElements * s_menu_background;
static Vec2 s_open_file_prompt_box_scale_matrix[2] = {0};
static Vec2 s_open_file_list_box_scale_matrix[2] = {0};
static Vec2 s_open_file_buttons_scale_matrix[2] = {0};
static Vec2 s_slider_background_scale_matrix[2] = {0};
static Vec2 s_slider_scale_matrix[2] = {0};
static Vec2 s_list_item_highlight_scale_matrix[2] = {0};
static UIElements * s_ui_elements;
static UIInteractiveList s_interactive_list = {0};
static unsigned int s_list_lines_number = 0;
static char * s_files_list = 0;
static unsigned int s_files_count = 0;
static unsigned int s_file_index = 0;

static void close_open_file_menu() {
    configure_input(0);
    set_prime_interface(1);
    set_ui_elements_visibility(s_menu_background, 0, 1);
    set_ui_elements_visibility(s_ui_elements, 0, DSTUDIO_OPEN_FILE_BASE_UI_ELEMENTS_COUNT + s_list_lines_number);
    dstudio_free(s_files_list);
    
    if (s_cancel_callback) {
        s_cancel_callback(NULL);
    }
    g_active_interactive_list = 0;
}

static void close_open_file_menu_button_callback(UIElements * ui_elements) {
    (void) ui_elements;
    close_open_file_menu();
}

static int strcoll_proxy(const void * a, const void *b) {
    return strcoll( (const char*) a, (const char *) b);
}

void init_open_menu(
    UIElements * menu_background,
    UIElements * ui_elements
) {
    s_ui_elements = ui_elements;
    s_menu_background = menu_background;
    GLuint texture_ids[2] = {0};
    
    UIElements * prompt_box  = ui_elements;
    UIElements * prompt = &ui_elements[1];
    UIElements * buttons_box = &ui_elements[2];
    UIElements * button_cancel = &ui_elements[3];
    UIElements * button_open = &ui_elements[4];
    UIElements * list_box = &ui_elements[5];
    UIElements * slider_background = &ui_elements[6];
    UIElements * slider = &ui_elements[7];
    UIElements * list_highlight = &ui_elements[8];
    UIElements * list = &ui_elements[9];
    
    s_open_file_prompt_box_scale_matrix[0].x = 1;
    s_open_file_prompt_box_scale_matrix[1].y = ((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT / (GLfloat) g_dstudio_viewport_height);
    
    prompt_box->color.r = 0;
    prompt_box->color.g = 0;
    prompt_box->color.b = 0;
    prompt_box->color.a = 0.5;
    
    init_ui_elements(
        prompt_box,
        NULL,
        &s_open_file_prompt_box_scale_matrix[0],
        0,
        1.0 - ( ((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_BOX_OFFSET_Y) / g_dstudio_viewport_height),
        g_dstudio_viewport_width,
        DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        10,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );

    init_ui_elements(
        prompt,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        -1.0 + (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_OFFSET_X) / g_dstudio_viewport_width),
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_OFFSET_Y) / g_dstudio_viewport_height),
        DSTUDIO_OPEN_FILE_PROMPT_AREA_WIDTH,
        DSTUDIO_OPEN_FILE_PROMPT_AREA_HEIGHT,
        0,
        0,
        DSTUDIO_OPEN_FILE_PROMPT_COLUMN,
        DSTUDIO_OPEN_FILE_PROMPT_COUNT,
        DSTUDIO_OPEN_FILE_PROMPT_BUFFER_SIZE,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_NONE
    );
    update_text(prompt, "OPEN FILE", 9);
    prompt->request_render = 0;

    buttons_box->color.r = 0;
    buttons_box->color.g = 0;
    buttons_box->color.b = 0;
    buttons_box->color.a = 0.5;
    
    init_ui_elements(
        buttons_box,
        NULL,
        &s_open_file_prompt_box_scale_matrix[0],
        0,
        -1.0 + ( ((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_BOX_OFFSET_Y) / g_dstudio_viewport_height),
        g_dstudio_viewport_width,
        DSTUDIO_OPEN_FILE_PROMPT_BOX_AREA_HEIGHT,
        0,
        0,
        1,
        1,
        10,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );

    GLfloat buttons_pos_y = -1.0 +((GLfloat) DSTUDIO_OPEN_FILE_BUTTON_OFFSET_Y) / (GLfloat) (g_dstudio_viewport_height>>1);
    /* Add a half-pixel to minimize interpolation */
    buttons_pos_y += 0.5 / (GLfloat) (g_dstudio_viewport_height>>1);
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_CANCEL_BUTTON_ASSET_PATH,
        s_open_file_buttons_scale_matrix
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_ACTIVE_CANCEL_BUTTON_ASSET_PATH,
        NULL
    ); 
    
    init_ui_elements(
        button_cancel,
        &texture_ids[0],
        &s_open_file_buttons_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_CANCEL_BUTTON_OFFSET_X) / g_dstudio_viewport_width),
        buttons_pos_y,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        0,
        0,
        DSTUDIO_CANCEL_BUTTON_COLUMNS,
        DSTUDIO_CANCEL_BUTTON_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );

    button_cancel->application_callback = close_open_file_menu_button_callback;

    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_OPEN_FILE_BUTTON_ASSET_PATH,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_ACTIVE_OPEN_FILE_BUTTON_ASSET_PATH,
        NULL
    ); 

    init_ui_elements(
        button_open,
        &texture_ids[0],
        &s_open_file_buttons_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_BUTTON_OFFSET_X) / g_dstudio_viewport_width),
        buttons_pos_y, 
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        0,
        0,
        DSTUDIO_OPEN_FILE_BUTTON_COLUMNS,
        DSTUDIO_OPEN_FILE_BUTTON_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );
    
    list_box->color.r = 0;
    list_box->color.g = 0;
    list_box->color.b = 0;
    list_box->color.a = 0.5;
    
    s_open_file_list_box_scale_matrix[0].x = 1;
    s_open_file_list_box_scale_matrix[1].y = ((GLfloat) DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT)/ (GLfloat) g_dstudio_viewport_height;

    init_ui_elements(
        list_box,
        NULL,
        &s_open_file_list_box_scale_matrix[0],
        0,
        0,
        g_dstudio_viewport_width,
        DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE,
        DSTUDIO_FLAG_NONE
    );

    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_SLIDER_BACKGROUND_WIDTH,
        DSTUDIO_SLIDER_BACKGROUND_HEIGHT, 
        DSTUDIO_SLIDER_BACKGROUND_ASSET_PATH,
        NULL
    );

    s_list_lines_number = (DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT / 18) - 2;
    #ifdef DSTUDIO_DEBUG
        printf("Open file menu should have %u lines.\n", s_list_lines_number);
    #endif

    s_slider_background_scale_matrix[0].x = (GLfloat) DSTUDIO_SLIDER_BACKGROUND_WIDTH / (GLfloat) g_dstudio_viewport_width;
    s_slider_background_scale_matrix[1].y = (GLfloat) (DSTUDIO_SLIDER_BACKGROUND_HEIGHT/3) / (GLfloat) g_dstudio_viewport_height;

    init_ui_elements(
        slider_background,
        &texture_ids[0],
        &s_slider_background_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_SLIDER_BACKGROUND_OFFSET_X) / g_dstudio_viewport_width),
        ((GLfloat) s_list_lines_number*18-9) / (GLfloat) (g_dstudio_viewport_height),
        18,
        s_list_lines_number*18,
        0,
        0,
        1,
        1,
        s_list_lines_number * 2,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER_BACKGROUND,
        DSTUDIO_FLAG_NONE
    );    

    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_SLIDER_WIDTH,
        DSTUDIO_OPEN_FILE_SLIDER_HEIGHT, 
        DSTUDIO_OPEN_FILE_SLIDER_ASSET_PATH,
        s_slider_scale_matrix
    );

    init_ui_elements(
        slider,
        &texture_ids[0],
        &s_slider_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_SLIDER_BACKGROUND_OFFSET_X) / g_dstudio_viewport_width),
        0,
        18,
        s_list_lines_number*18,
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_SLIDER,
        DSTUDIO_FLAG_SLIDER_TO_TOP
    );

    GLfloat list_y_pos = ((GLfloat) s_list_lines_number*18 - 18) / (GLfloat) (g_dstudio_viewport_height);
    
    init_ui_elements(
        list,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        -1.0 + (((GLfloat)DSTUDIO_OPEN_FILE_LIST_OFFSET_X) / g_dstudio_viewport_width),
        list_y_pos,
        800,
        18,
        0,
        -((GLfloat) 18) / (GLfloat) (g_dstudio_viewport_height >> 1), /* offset y */
        1,
        (DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT / 18) - 2,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE,
        DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM,
        DSTUDIO_FLAG_NONE
    );

    DEFINE_SCALE_MATRIX(
        s_list_item_highlight_scale_matrix,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE*8,
        18
    )

    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_TEXTURE_IS_PATTERN,
        DSTUDIO_PATTERN_SCALE,
        DSTUDIO_PATTERN_SCALE, 
        DSTUDIO_LIST_ITEM_HIGHLIGHT_PATTERN_PATH,
        NULL
    );

    init_ui_elements(
        list_highlight,
        &texture_ids[0],
        &s_list_item_highlight_scale_matrix[0],
        DSTUDIO_OPEN_FILE_LIST_HIGHLIGHT_POS_X,
        list_y_pos,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE*8,
        18, 
        0,
        0,
        1,
        1,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_PATTERN,
        DSTUDIO_FLAG_TEXTURE_IS_PATTERN
    );    
    
    init_interactive_list(
        &s_interactive_list,
        list_highlight,
        s_list_lines_number,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE,
        &s_files_count,
        NULL, /* At this point has not been allocated yet. */
        &g_open_file_thread_control,
        select_file_from_list,
        0,
        DSTUDIO_OPEN_FILE_LIST_HIGHLIGHT_OFFSET_Y
    );
    
    bind_scroll_bar(
        &s_interactive_list,
        slider
    );
    
    sem_init(&g_open_file_thread_control.mutex, 0, 1);
    g_open_file_thread_control.ready = 1;
}

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    void (*select_callback)(FILE * file_fd)
) {
    UIElements * highlight = 0;
    s_cancel_callback = cancel_callback;
    s_select_callback = select_callback;
    configure_input(PointerMotionMask);
    set_prime_interface(0);
    set_ui_elements_visibility(s_menu_background, 1, 1);
    set_ui_elements_visibility(s_ui_elements, 1, DSTUDIO_OPEN_FILE_BASE_UI_ELEMENTS_COUNT + s_list_lines_number);
    set_close_sub_menu_callback(close_open_file_menu);
    g_menu_background_enabled = s_menu_background;
    
    char * default_path = 0;
    expand_user(&default_path, "~");
    DIR * dr = opendir(default_path);
    struct dirent *de;

    s_files_count = 0;
    s_files_list = dstudio_alloc( DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number);
    unsigned int allocation_size = DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number;
    
    while ((de = readdir(dr)) != NULL) {
        if (s_files_count == (allocation_size / DSTUDIO_OPEN_FILE_CHAR_PER_LINE)) {
            allocation_size += DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number;
            s_files_list = dstudio_realloc(s_files_list, allocation_size);
        }
        strncpy(&s_files_list[s_files_count * DSTUDIO_OPEN_FILE_CHAR_PER_LINE], de->d_name, DSTUDIO_OPEN_FILE_CHAR_PER_LINE);
        s_files_count +=1;
    }
    s_interactive_list.source_data = s_files_list;
    
    qsort(s_files_list, s_files_count, DSTUDIO_OPEN_FILE_CHAR_PER_LINE, strcoll_proxy);
    highlight = s_interactive_list.highlight;
    highlight->instance_offsets_buffer->y = s_interactive_list.highlight_offset_y;
    s_interactive_list.index = 0;
    s_interactive_list.window_offset = 0;
    s_interactive_list.update_highlight = 1;
    highlight->scissor.y = (1 + highlight->instance_offsets_buffer->y - highlight->scale_matrix[1].y) * (g_dstudio_viewport_height >> 1);
    update_insteractive_list(&s_interactive_list);
    update_ui_element_motion(s_interactive_list.scroll_bar, s_interactive_list.max_scroll_bar_offset);
    closedir(dr);    
    dstudio_free(default_path);
    g_active_interactive_list = &s_interactive_list;
}

unsigned int select_file_from_list(
    unsigned int index
) {
    if (index != s_file_index && index < s_files_count) {
        s_file_index = index;
        return 1;
    }
    return 0;
}

void update_open_file_ui_list() {
    update_insteractive_list(&s_interactive_list);
}
