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
#include "ui.h"

typedef enum DStudioAudioAPIRequest_t {
    DSTUDIO_AUDIO_API_REQUEST_NO_DATA_PROCESSING,
    DSTUDIO_AUDIO_API_ACK_NO_DATA_PROCESSING,
    DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING,
    DSTUDIO_AUDIO_API_ACK_DATA_PROCESSING,
} DStudioAudioAPIRequest;

typedef enum DStudioAudioAPIError_t {
    DSTUDIO_AUDIO_API_NO_ERROR,
    DSTUDIO_AUDIO_API_CLIENT_IS_NULL,
    DSTUDIO_AUDIO_API_CLIENT_CANNOT_CONNECT_TO_SERVER,
    DSTUDIO_AUDIO_API_CANNOT_ENABLE_CLIENT,
    DSTUDIO_AUDIO_API_AUDIO_PORT_CANNOT_BE_CREATED,
    DSTUDIO_AUDIO_API_MIDI_PORT_CANNOT_BE_CREATED,
    DSTUDIO_AUDIO_API_AUDIO_PORT_RENAMING_FAILED
} DStudioAudioAPIError;

typedef enum DStudioAudioAPIMidiCaptureState_t {
    DSTUDIO_AUDIO_API_MIDI_CAPTURE_NONE = 0,
    DSTUDIO_AUDIO_API_MIDI_CAPTURE_WAIT_FOR_TARGET = 1,
    DSTUDIO_AUDIO_API_MIDI_CAPTURE_WAIT_FOR_INPUT = 2,
} DStudioAudioAPIMidiCaptureState;

typedef struct AudioPort_t {
    void * left;
    void * right;
    void * midi;
} AudioPort;

typedef struct VoiceContext_t VoiceContext;

void dstudio_audio_api_request(DStudioAudioAPIRequest request);

// TODO: rename with dstudio_audio_api prefix
// Must be implemented for each kind of API
unsigned int dstudio_audio_api_voice_has_midi_input();
DStudioAudioAPIError init_audio_api_client(void (*client_process_callback)(VoiceContext * voice, float * out_left, float * out_right, unsigned int frame_size));
DStudioAudioAPIError register_midi_port(AudioPort * output_port, const char * port_name);
DStudioAudioAPIError register_stereo_output_port(AudioPort * output_port, const char * left_port_name, const char * right_port_name);
DStudioAudioAPIError stop_audio_api_client();
DStudioAudioAPIError rename_active_context_audio_port(); 


void trigger_midi_capture(UIElements * self);

extern unsigned int g_midi_capture_state;
extern UIElements * g_midi_ui_element_target;

#endif
