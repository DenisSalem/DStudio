/*
 * Copyright 2019, 2025 Denis Salem
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

#include <stdio.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include "../paths.h"

static FT_Library s_freetype_library;
static FT_Face    s_face;

int dstudio_init_text() {
    int error = FT_Init_FreeType(&s_freetype_library);
    if (error != FT_Err_Ok) {
        printf("DSTUDIO: FT_Init_FreeType(...) != FT_Err_Ok\n");
        return error;
    }

    error = FT_New_Face(
        s_freetype_library,
        DSTUDIO_DEFAULT_FONT_PATH,
        0,
        &s_face
    );

    if ( error == FT_Err_Unknown_File_Format ) {
        printf("DSTUDIO: FT_New_Face(...) == FT_Err_Unknown_File_Format\n");
        return FT_Err_Unknown_File_Format;
    }
    else if ( error ) {
        printf("DSTUDIO: FT_New_Face(...) != FT_Err_Ok\n");
        return error;
    }


    error = FT_Set_Pixel_Sizes(
      s_face,
      16,
      16
    );   
        
    if (error != FT_Err_Ok) {
        printf("DSTUDIO: FT_Set_Pixel_Sizes(...) != FT_Err_Ok\n");
        return error;
    }

    return FT_Err_Ok;
}


// NEXT IS http://freetype.org/freetype2/docs/tutorial/step1.html#section-7
