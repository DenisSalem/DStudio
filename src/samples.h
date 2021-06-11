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

#include <stdint.h>

typedef enum SharedSampleError_t {
    DSTUDIO_SHARED_SAMPLE_NO_ERROR = 0,
    DSTUDIO_SHARED_SAMPLE_ALLOCATION_FAILED = 1,
    DSTUDIO_SHARED_SAMPLE_UNSUPPORTED_BPS = 2
} SharedSampleError;

typedef struct SharedSample_t {
    char * identifier;          // Absolute path of the sample.
    uint_fast8_t count;        // How many time the sample is referenced.
    uint_fast8_t channels;    
    uint_fast8_t bps;          
    uint_fast8_t error_code;
    uint_fast32_t rate;
    uint_fast32_t size;
    float *  left; 
    float *  right;
} SharedSample;

#endif
