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
#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "audio_api.h"
#include "info_bar.h"
#include "instances.h"
#include "voices.h"

void * s_audio_api_client = 0;
DStudioAudioAPIRequest s_audio_api_request_state = DSTUDIO_AUDIO_API_REQUEST_NO_DATA_PROCESSING;

void dstudio_audio_api_request(DStudioAudioAPIRequest request) {
    if (s_audio_api_client == NULL)
        return;
        
    s_audio_api_request_state = request;
    DStudioAudioAPIRequest ack = request == DSTUDIO_AUDIO_API_REQUEST_DATA_PROCESSING ? DSTUDIO_AUDIO_API_ACK_DATA_PROCESSING : DSTUDIO_AUDIO_API_ACK_NO_DATA_PROCESSING;
    while (s_audio_api_request_state != ack) {
        usleep(500); // 0.5ms
    }
}

void trigger_midi_capture(UIElements * self) {
    (void) self;
    if(!dstudio_audio_api_voice_has_midi_input()) {
        update_info_text("The current active voice has no midi input.");
    }
    else {
        update_info_text("Move a knob or a slider to select midi target.");
        g_midi_capture_state = DSTUDIO_AUDIO_API_MIDI_CAPUTRE_WAIT_FOR_TARGET;
    }
    return;
}

void (*s_client_process)(VoiceContext * voice, float * out_left, float * out_right, unsigned int nframes); 

unsigned int g_midi_capture_state = DSTUDIO_AUDIO_API_MIDI_CAPTURE_NONE;

#ifdef DSTUDIO_USE_JACK_AUDIO_CONNECTION_KIT
    #include "jack.c"
#endif
