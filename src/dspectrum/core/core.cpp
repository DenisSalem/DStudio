#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <GL/gl3w.h>
#include "core.hpp"

void SetString(char * destination, const char * string, int bufferLenght) {
  std::string str(string);
  std::size_t lenght = str.copy(destination,bufferLenght,0);
  destination[lenght] = 0;
}
void PrepareRender(RenderContext * renderContext) {

  glGenBuffers(1, &renderContext->IBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderContext->IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned short int), renderContext->vertexIndex, GL_STATIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenTextures(1, &renderContext->textureID);
  glBindTexture(GL_TEXTURE_2D, renderContext->textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, renderContext->width, renderContext->height, 0, GL_RGB, GL_UNSIGNED_BYTE, renderContext->texture);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glBindTexture(GL_TEXTURE_2D, 0);

  glGenBuffers(1, &renderContext->VBO);
  glBindBuffer(GL_ARRAY_BUFFER, renderContext->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(CoordinatesSet) * 4, 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(CoordinatesSet) * 4, &renderContext->verticesAttributes);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  glGenVertexArrays(1, &renderContext->VAO);
  glBindVertexArray(renderContext->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, renderContext->VBO);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), BUFFER_OFFSET_ZERO);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 5 * sizeof(GLfloat), (GLvoid *)(3 * sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  renderContext->vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
  renderContext->fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
  
  loadShader(&renderContext->vertexShader, renderContext->vertexShaderPath);
  loadShader(&renderContext->fragmentShader, renderContext->fragmentShaderPath);
  
  compileShader(renderContext->vertexShaderID, renderContext->vertexShader);
  compileShader(renderContext->fragmentShaderID, renderContext->fragmentShader);

  renderContext->programID = glCreateProgram();

  glAttachShader(renderContext->programID, renderContext->vertexShaderID);
  glAttachShader(renderContext->programID, renderContext->fragmentShaderID);

  glLinkProgram(renderContext->programID);

  glDeleteShader(renderContext->vertexShaderID);
  glDeleteShader(renderContext->fragmentShaderID);

  GLint Result = GL_FALSE;
  int InfoLogLength = 1024;
  char ProgramErrorMessage[1024] = {0};

  glGetProgramiv(renderContext->programID, GL_LINK_STATUS, &Result);
  glGetProgramiv(renderContext->programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
  glGetProgramInfoLog(renderContext->programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
  
  if (strlen(ProgramErrorMessage) != 0) 
    std::cout << ProgramErrorMessage << "\n";
  
}

void compileShader(GLuint shaderID, char * sourcePointer) {
  glShaderSource(shaderID, 1, &sourcePointer , NULL);
  glCompileShader(shaderID);

  GLint Result = GL_FALSE;
  int InfoLogLength = 1024;
  char shaderErrorMessage[1024] = {0};

  glGetShaderiv(shaderID, GL_COMPILE_STATUS, &Result);
	
  glGetShaderInfoLog(shaderID, InfoLogLength, NULL, shaderErrorMessage);
  if (strlen(shaderErrorMessage) != 0) 
    std::cout <<  shaderErrorMessage << "\n" ;
}

void loadShader(char ** shaderBuffer, const char * fileName) {
  FILE * shader = fopen (fileName, "r");
  (*shaderBuffer) = new char[1280];
  for (int i=0;i<1280;i++) {
    (*shaderBuffer)[i] = (unsigned char ) fgetc(shader);
    if ((*shaderBuffer)[i] == EOF) {
      (*shaderBuffer)[i] = '\0';
      break;
    }
  }
  fclose(shader);
}

void InitWindow(GLFWwindow ** window, int windowWidth, int windowHeight, const char * windowName) {
  if (!glfwInit()) {
    exit(-1);
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  (*window) = glfwCreateWindow(windowWidth, windowHeight, windowName, NULL, NULL);
  
  if (!(*window))  {
    exit(-2);
  }

  glfwMakeContextCurrent((*window));

  if(gl3wInit() != 0) {
    exit(-3);
  }
}
