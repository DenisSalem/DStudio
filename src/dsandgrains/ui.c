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

#include "../extensions.h"
#include "ui.h"

void init_background(Background * background) {
    GLuint * vertex_indexes = background->vertex_indexes;
    DSTUDIO_SET_VERTEX_INDEXES
        
    VertexAttributes * vertexes_attributes = background->vertexes_attributes;
    vertexes_attributes[0].x = -1.0f;
    vertexes_attributes[0].y =  1.0f;
    vertexes_attributes[1].x = -1.0f;
    vertexes_attributes[1].y = -1.0f;
    vertexes_attributes[2].x =  1.0f;
    vertexes_attributes[2].y =  1.0f;
    vertexes_attributes[3].x =  1.0f;
    vertexes_attributes[3].y = -1.0f;
    
    DSTUDIO_SET_S_T_COORDINATES
    
    background->index_buffer_object = 0;
    glGenBuffers(1, &background->index_buffer_object);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background->index_buffer_object);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * sizeof(GLuint), vertex_indexes, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
   
    GLuint * vertex_buffer_object_p = &background->vertex_buffer_object;
    glGenBuffers(1, vertex_buffer_object_p);
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer_object_p);
        glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttributes) * 4, vertexes_attributes, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    get_png_pixel("../assets/dsandgrains_background.png", &background->texture);
    
    glGenTextures(1, &background->texture_id);
    glBindTexture(GL_TEXTURE_2D, background->texture_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, DSANDGRAINS_VIEWPORT_WIDTH, DSANDGRAINS_VIEWPORT_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, background->texture);
        glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);


    glGenVertexArrays(1, &background->vertex_array_object);
    glBindVertexArray(background->vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, background->vertex_buffer_object);         
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void render_background(Background * background, GLint program_id) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program_id);
        glBindTexture(GL_TEXTURE_2D, background->texture_id);
            glBindVertexArray(background->vertex_array_object);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, background->index_buffer_object);
                    glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, (GLvoid *) 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}
