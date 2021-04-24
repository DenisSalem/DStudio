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

#include <FLAC/stream_decoder.h>

#include "common.h"
#include "flac.h"

void (*s_client_error_callback)(const char * message) = 0;
unsigned int s_decode_flac_goes_wrong = 1;

FLAC__StreamDecoderWriteStatus write_callback(
    const FLAC__StreamDecoder *decoder,
    const FLAC__Frame *frame,
    const FLAC__int32 * const buffer[],
    void * client_data
) {
    (void) decoder;
    (void) frame;
    (void) buffer;
    SharedSample * shared_sample =  (SharedSample *) client_data; 
    s_decode_flac_goes_wrong = 0;

    if (shared_sample->error_code) {
        s_decode_flac_goes_wrong = 1;
        return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
    }
    
    unsigned sample_index_offset = frame->header.number.sample_number;
    float * left = shared_sample->left;
    float * right = shared_sample->right;
    float max_value = (float) ((1 << shared_sample->bps) -1 );
    unsigned char channels = shared_sample->channels;
    
    for (unsigned i = 0; i < frame->header.blocksize; i++) {
        left[sample_index_offset+i] = (float) (buffer[0][i]) / max_value;
        if (channels == 2) {
            right[sample_index_offset+i] = (float) (buffer[1][i]) / max_value;
        }
    }
    
    return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
	(void)decoder;

    SharedSample * shared_sample = (SharedSample *) client_data;

	if(metadata->type == FLAC__METADATA_TYPE_STREAMINFO) {
		shared_sample->bps = metadata->data.stream_info.bits_per_sample;
        shared_sample->size = metadata->data.stream_info.total_samples;
        shared_sample->rate = metadata->data.stream_info.sample_rate;
        switch (metadata->data.stream_info.channels) {
            case 1:
            case 2:
                shared_sample->channels = metadata->data.stream_info.channels;
                break;
                
            default:
                shared_sample->error_code = DSTUDIO_SHARED_SAMPLE_ALLOCATION_FAILED;
                s_client_error_callback("Audio file has more than two channels.");
                return;
        }
        unsigned size = shared_sample->size * sizeof(float);
        
        shared_sample->left = dstudio_alloc(
            size,
            DSTUDIO_FAILURE_IS_NOT_FATAL
        );
        
        if (shared_sample->channels == 2) {
            shared_sample->right = dstudio_alloc(
                size,
                DSTUDIO_FAILURE_IS_NOT_FATAL
            );
        }
        
        if (shared_sample->left == NULL || (shared_sample->channels == 2 && shared_sample->right == NULL)) {
            s_client_error_callback("Memory allocation failed.");
            shared_sample->error_code = DSTUDIO_SHARED_SAMPLE_ALLOCATION_FAILED;
        }

        #ifdef DSTUDIO_DEBUG
		fprintf(stderr, "sample rate    : %u Hz\n", shared_sample->rate);
		fprintf(stderr, "stereo       : %u\n", shared_sample->is_stereo);
		fprintf(stderr, "bits per sample: %u\n", shared_sample->bps);
		fprintf(stderr, "total samples  : %lu\n", shared_sample->size);
        #endif
	}
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
	(void)decoder;
    (void)client_data;
    if (s_client_error_callback) {
        s_client_error_callback(FLAC__StreamDecoderErrorStatusString[status]);
    }
}


int load_flac(
    FILE * file,
    void (*client_error_callback)(const char * message),
    SharedSample * shared_sample
) {
    s_client_error_callback = client_error_callback;
	FLAC__StreamDecoder *decoder = 0;
	FLAC__StreamDecoderInitStatus init_status;
    FLAC__bool decode_status = 0;
    
    DSTUDIO_RETURN_IF_FAILURE((decoder = FLAC__stream_decoder_new()) == NULL)
	(void) FLAC__stream_decoder_set_md5_checking(decoder, true);

	init_status = FLAC__stream_decoder_init_FILE(decoder, file, write_callback, metadata_callback, error_callback, shared_sample);
    if(init_status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
        client_error_callback(FLAC__StreamDecoderInitStatusString[init_status]);
        // TODO: GOES TO LOG
		fprintf(stderr, "ERROR: initializing decoder: %s\n", FLAC__StreamDecoderInitStatusString[init_status]);
        FLAC__stream_decoder_delete(decoder);	
        return init_status;
    }
        
    decode_status = FLAC__stream_decoder_process_until_end_of_stream(decoder);
    
    //~ FLAC__StreamDecoderState state = FLAC__stream_decoder_get_state(decoder);
    
    FLAC__stream_decoder_finish(decoder);
    FLAC__stream_decoder_delete(decoder);
    
    if (s_decode_flac_goes_wrong)
        return 0;
    s_decode_flac_goes_wrong = 1;
    return decode_status;
}
