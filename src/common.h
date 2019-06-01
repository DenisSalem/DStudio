#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#define RETURN_IF_FAILURE(value) \
    if ( value != 0 ) { \
        printf("!" #value "\n"); \
        return -1; \
    }

#define EXIT_IF_FAILURE(value) \
    if (value != 0) { \
        printf("!" #value "\n"); \
        exit(-1); \
    }
    
#define EXIT_IF_FAILURE_GLFW_TERMINATE(value) \
    if (value != 0) { \
        printf("!" #value "\n"); \
        glfwTerminate(); \
        exit(-1); \
    }
    
#endif
