#ifndef DSTUDIO_SAMPLES_H_INCLUDED
#define DSTUDIO_SAMPLES_H_INCLUDED

typedef enum SharedSampleError_t {
    DSTUDIO_SHARED_SAMPLE_NO_ERROR = 0,
    DSTUDIO_SHARED_SAMPLE_ALLOCATION_FAILED = 1 
} SharedSampleError;

typedef struct SharedSample_t {
    char * identifier;          // Absolute path of the sample.
    unsigned char count;        // How many time the sample is referenced.
    unsigned char is_stereo;    
    unsigned char bps;          
    unsigned char error_code;
    unsigned rate;
    long unsigned size;
    union {
        short * left16;
        int *   left24;
        void *  left; // Generic member for allocation
        
    };
    union {
        short * right16;
        int *   right24;
        void *  right; // Generic member for allocation
    };
} SharedSample;

#endif
