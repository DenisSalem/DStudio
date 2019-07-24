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

#include <stdio.h>
#include <string.h>

#include "extensions.h"

// https://www.khronos.org/opengl/wiki/Tutorial:_OpenGL_3.0_Context_Creation_(GLX)
int is_extension_supported(const char * list, const char * extension) {
    const char *start;
    const char *where, *terminator;
  
    /* Extension names should not have spaces. */
    where = strchr(extension, ' ');
    if (where || *extension == '\0') {
        return 0;
    }

    /* It takes a bit of care to be fool-proof about parsing the
    OpenGL extensions string. Don't be fooled by sub-strings,
    etc. */
    for (start=list;;) {
        where = strstr(start, extension);
        if (!where) {
            break;
        }
    
        terminator = where + strlen(extension);

        if ( where == start || *(where - 1) == ' ' ) {
            if ( *terminator == ' ' || *terminator == '\0' ) {
                return 1;
            }
        }

        start = terminator;
    }

  return 0;
}

int load_extensions() {
    void* libGL = dlopen("libGL.so", RTLD_LAZY);
    if (!libGL) {
        printf("ERROR: libGL.so couldn't be loaded\n");
        return -1;
    }
    
    DSTUDIO_BIND_GL_FUN(AttachShader)
    DSTUDIO_BIND_GL_FUN(BindBuffer)
    DSTUDIO_BIND_GL_FUN(BindVertexArray)
    DSTUDIO_BIND_GL_FUN(BufferData)
    DSTUDIO_BIND_GL_FUN(BufferSubData)
    DSTUDIO_BIND_GL_FUN(CompileShader)
    DSTUDIO_BIND_GL_FUN(CreateProgram)
    DSTUDIO_BIND_GL_FUN(CreateShader)
    DSTUDIO_BIND_GL_FUN(DeleteProgram)
    DSTUDIO_BIND_GL_FUN(DeleteShader)
    DSTUDIO_BIND_GL_FUN(DrawElementsInstanced)
    DSTUDIO_BIND_GL_FUN(EnableVertexAttribArray)
    DSTUDIO_BIND_GL_FUN(GenBuffers)
    DSTUDIO_BIND_GL_FUN(GenerateMipmap)
    DSTUDIO_BIND_GL_FUN(GenVertexArrays)
    DSTUDIO_BIND_GL_FUN(GetUniformLocation)
    DSTUDIO_BIND_GL_FUN(LinkProgram)
    DSTUDIO_BIND_GL_FUN(VertexAttribDivisor)
    DSTUDIO_BIND_GL_FUN(VertexAttribPointer)
    DSTUDIO_BIND_GL_FUN(ShaderSource)
    DSTUDIO_BIND_GL_FUN(UniformMatrix2fv)
    DSTUDIO_BIND_GL_FUN(Uniform1f)
    DSTUDIO_BIND_GL_FUN(Uniform2fv)
    DSTUDIO_BIND_GL_FUN(UseProgram)

    #ifdef DSTUDIO_DEBUG
    DSTUDIO_BIND_GL_FUN(GetProgramInfoLog)
    DSTUDIO_BIND_GL_FUN(GetProgramiv)
    DSTUDIO_BIND_GL_FUN(GetShaderInfoLog)
    DSTUDIO_BIND_GL_FUN(GetStringi)
    
    GLint extensions_count;
    glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_count);
    printf("Extensions count: %d.\n", extensions_count);
    for (GLint i=0; i < extensions_count; i++) { 
        const char* extension = (const char*) glGetStringi(GL_EXTENSIONS, i);
        printf("Extention\t%d : %s\n", i, extension);
    }
    
    #endif
    
    return 0;
}
