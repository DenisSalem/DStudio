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
    
    Binder binder[] = {
        DSTUDIO_SET_BINDER_ELEMENT(glAttachShader),
        DSTUDIO_SET_BINDER_ELEMENT(glBindBuffer),
        DSTUDIO_SET_BINDER_ELEMENT(glBindFramebuffer),
        DSTUDIO_SET_BINDER_ELEMENT(glBindVertexArray),
        DSTUDIO_SET_BINDER_ELEMENT(glBlendFuncSeparate),
        DSTUDIO_SET_BINDER_ELEMENT(glBufferData),
        DSTUDIO_SET_BINDER_ELEMENT(glBufferSubData),
        DSTUDIO_SET_BINDER_ELEMENT(glCompileShader),
        DSTUDIO_SET_BINDER_ELEMENT(glCreateProgram),
        DSTUDIO_SET_BINDER_ELEMENT(glCreateShader),
        DSTUDIO_SET_BINDER_ELEMENT(glDeleteShader),
        DSTUDIO_SET_BINDER_ELEMENT(glDrawElementsInstanced),
        DSTUDIO_SET_BINDER_ELEMENT(glEnableVertexAttribArray),
        DSTUDIO_SET_BINDER_ELEMENT(glFramebufferTexture2D),
        DSTUDIO_SET_BINDER_ELEMENT(glGenBuffers),
        DSTUDIO_SET_BINDER_ELEMENT(glGenerateMipmap),
        DSTUDIO_SET_BINDER_ELEMENT(glGenFramebuffers),
        DSTUDIO_SET_BINDER_ELEMENT(glGenVertexArrays),
        DSTUDIO_SET_BINDER_ELEMENT(glGetUniformLocation),
        DSTUDIO_SET_BINDER_ELEMENT(glLinkProgram),
        DSTUDIO_SET_BINDER_ELEMENT(glShaderSource),
        DSTUDIO_SET_BINDER_ELEMENT(glUniformMatrix2fv),
        DSTUDIO_SET_BINDER_ELEMENT(glUniform1ui),
        DSTUDIO_SET_BINDER_ELEMENT(glUniform4fv),
        DSTUDIO_SET_BINDER_ELEMENT(glUseProgram),
        DSTUDIO_SET_BINDER_ELEMENT(glVertexAttribDivisor),
        DSTUDIO_SET_BINDER_ELEMENT(glVertexAttribPointer),
        #ifdef DSTUDIO_DEBUG
        DSTUDIO_SET_BINDER_ELEMENT(glCheckFramebufferStatus),
        DSTUDIO_SET_BINDER_ELEMENT(glGetProgramInfoLog),
        DSTUDIO_SET_BINDER_ELEMENT(glGetProgramiv),
        DSTUDIO_SET_BINDER_ELEMENT(glGetShaderInfoLog),
        DSTUDIO_SET_BINDER_ELEMENT(glGetStringi),
        #endif
        {0} // Sentinel
    };
        
    int i = 0;
    while (binder[i].function_pointer != 0) {
        /* 
         * Dirty trick to save binary size: We assume that size of
         * void* equal size of long int. Since we can't dereference
         * void *, we're tricking the compiler to store function pointer.
         */
        *(long unsigned int*)(binder[i].function_pointer) = (long unsigned int) dlsym(libGL, binder[i].name);
        if (*(long unsigned int*) binder[i].function_pointer == 0) {
            printf("%s couldn't be loaded from libGL.so\n", binder[i].name);
            return -1;
        }
        i++;
    }

    #ifdef DSTUDIO_DEBUG   
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
