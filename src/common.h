#ifndef DSTUDIO_COMMON_H_INCLUDED
#define DSTUDIO_COMMON_H_INCLUDED

#define DSTUDIO_RETURN_IF_FAILURE(value) \
    if ( (value) != 0 ) { \
        printf("!" #value "\n"); \
        return -1; \
    }

#define DSTUDIO_EXIT_IF_FAILURE(value) \
    if ((value) != 0) { \
        printf("!" #value "\n"); \
        exit(-1); \
    }

#define DSTUDIO_EXIT_IF_NULL(value) \
    if ((value) == 0) { \
        printf("0 == " #value "\n"); \
        exit(-1); \
    }

double get_timestamp();
    
#endif
