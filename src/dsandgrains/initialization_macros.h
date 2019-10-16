/*
 * Copyright 2019 Denis Salem
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

#define LOAD_SHARED_TEXTURE_AND_PREPARE_SHARED_SCALE_MATRICES \
    GLuint background_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_NONE, \
        DSANDGRAINS_VIEWPORT_WIDTH, \
        DSANDGRAINS_VIEWPORT_HEIGHT, \
        DSANDGRAINS_BACKGROUND_ASSET_PATH, \
        background_scale_matrix \
    ); \
 \
    GLuint knob1_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_SCALE, \
        DSANDGRAINS_KNOB1_ASSET_PATH, \
        knob1_scale_matrix \
    );
 \
    GLuint knob2_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA | DSTUDIO_FLAG_USE_ANTI_ALIASING, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_SCALE, \
        DSANDGRAINS_KNOB2_ASSET_PATH, \
        knob2_scale_matrix \
    ); \
 \
    GLuint slider_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_ASSET_PATH, \
        slider_scale_matrix \
    ); \
 \
    GLuint slider_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_SCALE, \
        DSANDGRAINS_SLIDER1_ASSET_PATH, \
        slider_scale_matrix \
    ); \
 \
    GLuint system_usage_texture_id = setup_texture_n_scale_matrix( \
        DSTUDIO_FLAG_USE_ALPHA, \
        DSANDGRAINS_SYSTEM_USAGE_WIDTH, \
        DSANDGRAINS_SYSTEM_USAGE_HEIGHT, \
        DSANDGRAINS_SYSTEM_USAGE_ASSET_PATH, \
        system_usage_scale_matrix \
    );  \
 \
    GLuint charset_texture_id = setup_texture_n_scale_matrix(
        DSTUDIO_FLAG_USE_ALPHA, \
        104,
        234,
        DSTUDIO_CHAR_TABLE_ASSET_PATH,
        NULL
    );
    
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX(charset_scale_matrix, 104, 234)
    GLuint charset_small_texture_id = setup_texture_n_scale_matrix(0, 1, 52, 117, DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH, NULL);
    DSTUDIO_SET_UI_ELEMENT_SCALE_MATRIX(charset_small_scale_matrix, 52, 117)
