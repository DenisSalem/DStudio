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

#include "text.h"
#include "ui.h"


static void (*s_cancel_callback)(UIElements * ui_elements) = 0;
static void (*s_select_callback)(UIElements * ui_elements) = 0;

void init_open_menu(UIElements * ui_elements) {
    UIElements * prompt = ui_elements;
    init_ui_elements(
        prompt,
        &g_charset_8x18_texture_ids[0],
        &g_charset_8x18_scale_matrix[0],
        0,
        0,
        g_dstudio_viewport_width,
        26,
        0,
        0,
        1,
        1,
        29,
        DSTUDIO_UI_ELEMENT_TYPE_TEXT,
        DSTUDIO_FLAG_NONE
    );
    update_text(prompt, "OPEN FILE", 29);
    prompt->request_render = 0;
}

void open_file_menu(
    void (*cancel_callback)(UIElements * ui_elements),
    void (*select_callback)(UIElements * ui_elements)
) {
    s_cancel_callback = cancel_callback;
    s_select_callback = select_callback;
}
