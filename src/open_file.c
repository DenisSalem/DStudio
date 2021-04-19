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

#include <errno.h>
#include <dirent.h>
#include <semaphore.h>
#include <stdlib.h>
#include <sys/types.h>

#include "fileutils.h"
#include "open_file.h"
#include "text.h"
#include "text_pointer.h"
#include "ui.h"
#include "window_management.h"
#include "sliders.h"

static void (*s_cancel_callback)(UIElements * ui_elements) = 0;
static unsigned int (*s_select_callback)(char * path, char * filename, FILE * file_fd) = 0;
static char * s_current_directory = 0;
static UIElements * s_menu_background;
static Vec2 s_open_file_prompt_box_scale_matrix[2] = {0};
static Vec2 s_open_file_list_box_scale_matrix[2] = {0};
static Vec2 s_open_file_buttons_scale_matrix[2] = {0};
static Vec2 s_slider_background_scale_matrix[2] = {0};
static Vec2 s_slider_scale_matrix[2] = {0};
static Vec2 s_list_item_highlight_scale_matrix[2] = {0};
static UIElements * s_prompt = {0};
static UIElements * s_ui_elements = {0};
static UIElements * s_error_message = {0};
static UIInteractiveList s_interactive_list = {0};
static unsigned int s_list_lines_number = 0;
static char * s_files_list = 0;
static unsigned int s_files_count = 0;
static unsigned int s_file_index = 0;
static unsigned int s_max_characters_for_error_prompt = 0;
static unsigned int s_max_prompt_char = 0;
static char * s_prompt_value = 0;
static char * s_prompt_cwd_value = 0;

static void close_open_file_menu(int has_cancel) {
    g_active_interactive_list = 0;
    configure_input(0);
    set_prime_interface(1);
    set_ui_elements_visibility(s_menu_background, 0, 1);
    set_ui_elements_visibility(s_ui_elements, 0, DSTUDIO_OPEN_FILE_BASE_UI_ELEMENTS_COUNT + s_list_lines_number);
    g_menu_background_enabled = 0;
    g_active_interactive_list = 0;
    dstudio_free(s_files_list);
    s_files_list = 0;
    if (has_cancel && s_cancel_callback) {
        s_cancel_callback(NULL);
    }
    g_request_render_all = 1;
    dstudio_clear_sub_menu_callback();
}

static void close_open_file_menu_button_callback(UIElements * ui_elements) {
    (void) ui_elements;
    close_open_file_menu(DSTUDIO_OPEN_FILE_MENU_CONSUME_CANCEL_CALLBACK);
}

static unsigned int refresh_file_list(char * path);

static void open_file_and_consume_callback(UIElements * ui_element) {
    (void) ui_element;
    unsigned int callback_status=0;
    char * path = 0;
    char * current_item_value = &s_interactive_list.source_data[s_file_index*s_interactive_list.stride];
    char * saved_current_directory = dstudio_alloc(
        strlen(s_current_directory)+1,
        DSTUDIO_FAILURE_IS_FATAL
    );
    FILE * file_fd = 0;
    strcpy(saved_current_directory, s_current_directory);
    s_current_directory = dstudio_realloc(
        s_current_directory,
        strlen(s_current_directory) +
        strlen(current_item_value) +
        2 // separator + Null byte 
    );
    
    strcat(s_current_directory, "/");
    path = dstudio_alloc(
        strlen(s_current_directory)+1,
        DSTUDIO_FAILURE_IS_FATAL
    );
    strcat(path, s_current_directory);
    
    strcat(s_current_directory, current_item_value);

    switch(dstudio_is_directory(s_current_directory)) {
        case -1:
            update_open_file_error(strerror(errno));
            break;
        case 1:
            dstudio_canonize_path(&s_current_directory);
            if (refresh_file_list(s_current_directory)) {
                strncpy(s_prompt_cwd_value, s_current_directory, s_max_prompt_char-DSTUDIO_PROMPT_CWD_CHAR_OFFSET);
                update_text(s_prompt, s_prompt_value, s_max_prompt_char);
                update_open_file_error("");
                update_text(s_error_message, "", s_max_characters_for_error_prompt);
                dstudio_free(saved_current_directory);
                dstudio_free(path);
                return;
            };
            break;
        case 0:
            file_fd = fopen(s_current_directory, "r");
            if (file_fd == NULL) {
                update_open_file_error(strerror(errno));
                return;
            }
            callback_status = s_select_callback(
                path,
                current_item_value,
                file_fd // file_fd must be closed by consumer
            );
            if (callback_status) {
                close_open_file_menu(DSTUDIO_OPEN_FILE_MENU_CONSUME_NO_CALLBACK);
            }
            break;
    }
    /* In any cases, except for sucessful directory opening, we're
     * setting path in it's previous state
     */
     dstudio_free(s_current_directory);
     dstudio_free(path);
     s_current_directory = saved_current_directory;  
}

static int strcoll_proxy(const void * a, const void *b) {
    return strcoll( (const char*) a, (const char *) b);
}

static unsigned int refresh_file_list(char * path) {
    DIR * dr = opendir(path);
    if (dr == NULL) {
        update_open_file_error(strerror(errno));
        return 0;
    }
    struct dirent *de;   
    s_files_count = 0;
    if (s_files_list != NULL) {
        dstudio_free(s_files_list);
    }
    s_files_list = dstudio_alloc(
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number,
        DSTUDIO_FAILURE_IS_FATAL
    );
    unsigned int allocation_size = DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number;
    
    while ((de = readdir(dr)) != NULL) {
        if (strcmp(de->d_name, ".") == 0) {
            continue;
        }
        if (s_files_count == (allocation_size / DSTUDIO_OPEN_FILE_CHAR_PER_LINE)) {
            allocation_size += DSTUDIO_OPEN_FILE_CHAR_PER_LINE * s_list_lines_number;
            s_files_list = dstudio_realloc(s_files_list, allocation_size);
        }
        strncpy(&s_files_list[s_files_count * DSTUDIO_OPEN_FILE_CHAR_PER_LINE], de->d_name, DSTUDIO_OPEN_FILE_CHAR_PER_LINE-1);
        s_files_count +=1;
    }
    s_interactive_list.source_data = s_files_list;
    qsort(s_files_list, s_files_count, DSTUDIO_OPEN_FILE_CHAR_PER_LINE, strcoll_proxy);

    s_interactive_list.window_offset = 0;
    select_item(s_interactive_list.lines, DSTUDIO_SELECT_ITEM_WITHOUT_CALLBACK);
    
    s_file_index = 0;
    
    update_ui_element_motion(s_interactive_list.scroll_bar, s_interactive_list.max_scroll_bar_offset);
    compute_slider_in_motion_scissor_y(s_interactive_list.scroll_bar);
    
    closedir(dr);
    return 1;  
}

void init_open_menu(
    UIElements * menu_background,
    UIElements * ui_elements
) {
    s_ui_elements = ui_elements;
    s_menu_background = menu_background;
    GLuint texture_ids[2] = {0};
    
    UIElements * prompt_box  = ui_elements;
    s_prompt = &ui_elements[1];
    UIElements * buttons_box = &ui_elements[2];
    s_error_message = &ui_elements[3];
    UIElements * button_cancel = &ui_elements[4];
    UIElements * button_open = &ui_elements[5];
    UIElements * list_box = &ui_elements[6];
    UIElements * slider_background = &ui_elements[7];
    UIElements * slider = &ui_elements[8];
    UIElements * list_highlight = &ui_elements[9];
    UIElements * list = &ui_elements[10];
    
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
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND,
        DSTUDIO_FLAG_NONE
    );

    s_max_prompt_char = (g_dstudio_viewport_width - DSTUDIO_OPEN_FILE_PROMPT_PADDING) / 8;
    s_prompt_value = dstudio_alloc(
        s_max_prompt_char,
        DSTUDIO_FAILURE_IS_FATAL
    );
    strcat(s_prompt_value, "OPEN FILE: ");
    s_prompt_cwd_value = &s_prompt_value[DSTUDIO_PROMPT_CWD_CHAR_OFFSET];
    init_ui_elements(
        s_prompt,
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
        s_max_prompt_char,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_NONE
    );
    update_text(s_prompt, s_prompt_value, s_max_prompt_char);
    s_prompt->render_state = DSTUDIO_UI_ELEMENT_NO_RENDER_REQUESTED;

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
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND,
        DSTUDIO_FLAG_NONE
    );

    GLfloat buttons_pos_y = -1.0 +((GLfloat) DSTUDIO_OPEN_FILE_BUTTON_OFFSET_Y) / (GLfloat) (g_dstudio_viewport_height>>1);
    
    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_CANCEL_BUTTON_ASSET_PATH,
        s_open_file_buttons_scale_matrix,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_ACTIVE_CANCEL_BUTTON_ASSET_PATH,
        NULL,
        NULL
    ); 
    
    
    s_max_characters_for_error_prompt = ((g_dstudio_viewport_width - DSTUDIO_OPEN_FILE_ERROR_PROMPT_PADDING_RIGHT) / 8)-1;
    #ifdef DSTUDIO_DEBUG
        printf("Open file menu should have %u characters for error prompt.\n", s_max_characters_for_error_prompt);
    #endif
    init_ui_elements(
        s_error_message,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        -1.0 + (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_OFFSET_X) / g_dstudio_viewport_width),
        -1.0 + (((GLfloat) DSTUDIO_OPEN_FILE_PROMPT_OFFSET_Y) / g_dstudio_viewport_height),
        DSTUDIO_OPEN_FILE_PROMPT_AREA_WIDTH,
        DSTUDIO_OPEN_FILE_PROMPT_AREA_HEIGHT,
        0,
        0,
        DSTUDIO_OPEN_FILE_PROMPT_COLUMN,
        DSTUDIO_OPEN_FILE_PROMPT_COUNT,
        s_max_characters_for_error_prompt,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_NONE
    );
    
    update_open_file_error("");
    
    init_ui_elements(
        button_cancel,
        &texture_ids[0],
        &s_open_file_buttons_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_CANCEL_BUTTON_OFFSET_X) / g_dstudio_viewport_width),
        buttons_pos_y,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT,
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
        NULL,
        NULL
    );
    
    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT, 
        DSTUDIO_ACTIVE_OPEN_FILE_BUTTON_ASSET_PATH,
        NULL,
        NULL
    ); 

    init_ui_elements(
        button_open,
        &texture_ids[0],
        &s_open_file_buttons_scale_matrix[0],
        1.0 - (((GLfloat) DSTUDIO_OPEN_FILE_BUTTON_OFFSET_X) / g_dstudio_viewport_width),
        buttons_pos_y, 
        DSTUDIO_OPEN_FILE_BUTTONS_WIDTH,
        DSTUDIO_OPEN_FILE_BUTTONS_HEIGHT,
        0,
        0,
        DSTUDIO_OPEN_FILE_BUTTON_COLUMNS,
        DSTUDIO_OPEN_FILE_BUTTON_COUNT,
        1,
        DSTUDIO_UI_ELEMENT_TYPE_BUTTON,
        DSTUDIO_FLAG_NONE
    );
    
    button_open->application_callback = open_file_and_consume_callback;
    button_open->application_callback_args = list;
    
    list_box->color.r = 0;
    list_box->color.g = 0;
    list_box->color.b = 0;
    list_box->color.a = 0.5;
    
    s_open_file_list_box_scale_matrix[0].x = 1;
    s_open_file_list_box_scale_matrix[1].y = ((GLfloat) DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT) / (GLfloat) g_dstudio_viewport_height;

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
        DSTUDIO_UI_ELEMENT_TYPE_NO_TEXTURE_BACKGROUND,
        DSTUDIO_FLAG_NONE
    );

    texture_ids[0] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA,
        DSTUDIO_SLIDER_BACKGROUND_WIDTH,
        DSTUDIO_SLIDER_BACKGROUND_HEIGHT, 
        DSTUDIO_SLIDER_BACKGROUND_ASSET_PATH,
        NULL,
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
        s_slider_scale_matrix,
        NULL
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
        -1.0 + (((GLfloat)DSTUDIO_OPEN_FILE_LIST_OFFSET_X) / g_dstudio_viewport_width) + 1.0/g_dstudio_viewport_width,
        list_y_pos,
        g_dstudio_viewport_width,
        18,
        0,
        -((GLfloat) 18) / (GLfloat) (g_dstudio_viewport_height >> 1), /* offset y */
        1,
        (DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT / 18) - 2,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE,
        DSTUDIO_UI_ELEMENT_TYPE_LIST_ITEM,
        DSTUDIO_FLAG_NONE
    );
    
    for (unsigned int i = 0; i < (DSTUDIO_OPEN_FILE_LIST_BOX_HEIGHT / 18) - 2; i++) {
        list[i].application_callback = open_file_and_consume_callback;
    }

    DEFINE_SCALE_MATRIX(
        s_list_item_highlight_scale_matrix,
        DSTUDIO_OPEN_FILE_CHAR_PER_LINE*8,
        18
    )

    texture_ids[1] = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_TEXTURE_IS_PATTERN,
        DSTUDIO_PATTERN_SCALE,
        DSTUDIO_PATTERN_SCALE, 
        DSTUDIO_LIST_ITEM_HIGHLIGHT_PATTERN_PATH,
        NULL,
        &list_highlight->pattern_scale
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
        DSTUDIO_UI_ELEMENT_TYPE_HIGHLIGHT,
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
        select_file_from_list,
        DSTUDIO_NO_CALLBACK,
        0,
        DSTUDIO_OPEN_FILE_LIST_HIGHLIGHT_OFFSET_Y
    );
    
    bind_scroll_bar(
        &s_interactive_list,
        slider
    );
}

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    unsigned int (*select_callback)(char * path, char * filename, FILE * file_fd)
) {
    s_cancel_callback = cancel_callback;
    s_select_callback = select_callback;
    configure_input(PointerMotionMask);
    set_prime_interface(0);
    set_ui_elements_visibility(s_menu_background, 1, 1);
    set_ui_elements_visibility(s_ui_elements, 1, DSTUDIO_OPEN_FILE_BASE_UI_ELEMENTS_COUNT + s_list_lines_number);
    set_close_sub_menu_callback(close_open_file_menu);
    g_menu_background_enabled = s_menu_background;
    
    dstudio_expand_user(&s_current_directory, "~");
    refresh_file_list(s_current_directory);
    
    strncpy(s_prompt_cwd_value, s_current_directory, s_max_prompt_char-DSTUDIO_PROMPT_CWD_CHAR_OFFSET);
    update_text(s_prompt, s_prompt_value, s_max_prompt_char);
    
    g_active_interactive_list = &s_interactive_list;
    g_request_render_all = 1;
}

unsigned int select_file_from_list(
    unsigned int index
) {
    unsigned int char_offset = 0;
    char * path = 0;
    if (index != s_file_index && index < s_files_count) {
        path = dstudio_alloc(
            strlen(s_current_directory) +
            strlen(&s_interactive_list.source_data[s_interactive_list.stride * index]) +
            2, // slash + null byte
            DSTUDIO_FAILURE_IS_FATAL
        );
        strcat(path, s_current_directory);
        strcat(path, "/");
        strcat(path, &s_interactive_list.source_data[s_interactive_list.stride * index]);
        if (strlen(path) > (s_max_prompt_char - DSTUDIO_PROMPT_CWD_CHAR_OFFSET)) {
            char_offset = strlen(path) - (s_max_prompt_char - DSTUDIO_PROMPT_CWD_CHAR_OFFSET) + 1;
        }
        strcpy(s_prompt_cwd_value, &path[char_offset]);
        update_text(s_prompt, s_prompt_value, s_max_prompt_char);
        update_text(s_error_message, "", s_max_characters_for_error_prompt);
        s_file_index = index;
        dstudio_free(path);
        return 1;
    }
    return 0;
}

void update_open_file_error(const char * message) {
    // TODO: Forward in log
    update_text(s_error_message, (char *) message, s_max_characters_for_error_prompt);
}

void update_open_file_ui_list() {
    if (s_interactive_list.update_request) {
        update_insteractive_list(&s_interactive_list);
    }
}
