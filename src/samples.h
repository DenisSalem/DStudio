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
