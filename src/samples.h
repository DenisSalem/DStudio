#ifndef DSTUDIO_SAMPLES_H_INCLUDED
#define DSTUDIO_SAMPLES_H_INCLUDED

typedef struct SharedSample_t {
    char * identifier; // Absolute path of the sample.
    unsigned int count; // How many time the sample is referenced.
    unsigned int is_stereo;
    int * left;
    int * right;
} SharedSample;

#endif
