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

#include "extensions.h"
#include <stdio.h>

int load_extensions() {
    void* libGL = dlopen("libGL.so", RTLD_LAZY);
    if (!libGL) {
        printf("ERROR: libGL.so couldn't be loaded\n");
        return -1;
    }
    
    DSTUDIO_BIND_GL_FUN(CompileShader)
    DSTUDIO_BIND_GL_FUN(CreateShader)
    
    #ifdef DSTUDIO_DEBUG
    DSTUDIO_BIND_GL_FUN(GetShaderInfoLog)
    DSTUDIO_BIND_GL_FUN(GetStringi)
    #endif

    DSTUDIO_BIND_GL_FUN(ShaderSource)
    DSTUDIO_BIND_GL_FUN(UseProgram)

    #ifdef DSTUDIO_DEBUG
    GLint extensions_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_count);
    for (GLint i=0; i < extensions_count; i++) { 
        const char* extension = (const char*) glGetStringi(GL_EXTENSIONS, i);
        printf("Extention\t%d : %s\n", i, extension);
    } 
    #endif
    
    return 0;
}

