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

#ifndef DSTUDIO_AUDIO_API_H_INCLUDED
#define DSTUDIO_AUDIO_API_H_INCLUDED

#include "common.h"

typedef enum DStudioAudioAPIError_t {
    DSTUDIO_AUDIO_API_NO_ERROR,
    DSTUDIO_AUDIO_API_CLIENT_IS_NULL,
    DSTUDIO_AUDIO_API_CLIENT_CANNOT_CONNECT_TO_SERVER,
    DSTUDIO_AUDIO_API_CANNOT_ENABLE_CLIENT,
    DSTUDIO_AUDIO_API_OUTPUT_PORT_CANNOT_BE_CREATED
} DStudioAudioAPIError;

typedef struct OutputPort_t {
    void * left;
    void * right;
} OutputPort;

// Must be implemented for each kind of API
DStudioAudioAPIError init_audio_api_client();
DStudioAudioAPIError register_stereo_output_port(OutputPort * output_port, const char * left_port_name, const char * right_port_name);
DStudioAudioAPIError stop_audio_api_client();
#endif
