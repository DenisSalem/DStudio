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

#include <GL/gl.h>
#include <dlfcn.h>

#define DSTUDIO_DEF_GL_FUN(ret, name, ...) typedef ret name##proc(__VA_ARGS__); name##proc * gl##name;

#define DSTUDIO_BIND_GL_FUN(name)                                 \
    gl##name = (name##proc *) dlsym(libGL, "gl" #name);           \
    if (!gl##name) {                                              \
        printf("gl" #name " couldn't be loaded from libGL.so\n"); \
        return -1;                                             \
    }
    
DSTUDIO_DEF_GL_FUN(void,            CompileShader,      GLuint shader)    
DSTUDIO_DEF_GL_FUN(GLuint,          CreateShader,       GLenum shaderType)

#ifdef DSTUDIO_DEBUG
DSTUDIO_DEF_GL_FUN(void,            GetShaderInfoLog,  GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
DSTUDIO_DEF_GL_FUN(const GLubyte *, GetStringi, GLenum name, GLuint index)
#endif

DSTUDIO_DEF_GL_FUN(void,            ShaderSource,       GLuint shader, GLsizei count, const GLchar **string, const GLint *length)     
DSTUDIO_DEF_GL_FUN(void,            UseProgram,         GLuint program)

int load_extensions();
