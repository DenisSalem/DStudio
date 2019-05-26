#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef _DSTUDIO_CORE_
#define _DSTUDIO_CORE_

#define BUFFER_OFFSET_ZERO 0

/*
 * Really? Do I need to explain this?
 */

typedef struct _RGB_ {
  unsigned char Red;
  unsigned char Green;
  unsigned char Blue;
} RGBValues;

/*
 * Will holds data related to vertex such as space coordinates and texture coordinates. 
 */

typedef struct _COORDS_ {
  GLfloat x;
  GLfloat y;
  GLfloat z;
  GLfloat s;
  GLfloat t;
} CoordinatesSet;

/* 
 * Will hold data associated with a given render context
 * for each objet we want to render on screen. 
 */

typedef struct _RENDER_CONTEXT_ {
  GLuint IBO;
  GLuint VBO;
  GLuint VAO;
  unsigned int width;
  unsigned int height;
  GLuint textureID;
  RGBValues * texture;
  GLuint vertexShaderID;
  GLuint fragmentShaderID;
  char fragmentShaderPath[64];
  char vertexShaderPath[64];
  GLuint programID;
  char * vertexShader;
  char * fragmentShader;
  CoordinatesSet verticesAttributes[4];
  unsigned short int vertexIndex[6];

} RenderContext;

/*
 * A quite simple method for setting string into buffer.
 */

void SetString(char * destination, const char * string, int bufferLenght);

/*
 * Will initiate a given render context.
 * Each render context is associated with one element displayed
 * on the screen, most likely a fixed or drawn on the fly sprite.
*/

void PrepareRender(RenderContext * renderContext);

/*
 * Open and read into shader sources to copy its content into mem buffer.
 */

void loadShader(char ** shaderBuffer, const char * fileName);

/*
 * Well, once shaders are into memory they desperately wait for compilation and linking. Sound's hot right?
 */

void compileShader(GLuint shaderID, char * sourcePointer);

/*
 * Before space and time, God first init It's OpenGL context. As a good believer I follow It's teaching.
 */

void InitWindow(GLFWwindow ** window, int windowWidth, int windowHeight, const char * windowName);

#endif
