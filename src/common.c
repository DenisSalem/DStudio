#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

GLint scissor_x, scissor_y = 0;
GLsizei scissor_width, scissor_height = 0;

static long unsigned int * s_allocation_register = 0;
static unsigned int s_allocation_register_index = 0;
static unsigned int s_allocation_register_size = DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
static sem_t s_alloc_register_mutex = {0};

void * dstudio_alloc(unsigned int buffer_size) {
    sem_wait(&s_alloc_register_mutex);
    if (s_allocation_register_index >= s_allocation_register_size) {
        s_allocation_register = realloc(s_allocation_register, sizeof(long unsigned int) * (s_allocation_register_size + DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        explicit_bzero(&s_allocation_register[s_allocation_register_size], sizeof(long unsigned int) * DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE);
        s_allocation_register_size += DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register[s_allocation_register_index] = (long unsigned int) malloc(buffer_size);
    DSTUDIO_EXIT_IF_NULL(s_allocation_register[s_allocation_register_index])
    explicit_bzero((void *) s_allocation_register[s_allocation_register_index], buffer_size);
    sem_post(&s_alloc_register_mutex);
    return (void *) s_allocation_register[s_allocation_register_index++];
}

void dstudio_cut_thread(ThreadControl * thread_control) {
    sem_wait(&thread_control->mutex);
    thread_control->cut_thread = 1;
    sem_post(&thread_control->mutex);
} 

void dstudio_free(void * buffer) {
    sem_wait(&s_alloc_register_mutex);
    
    if (buffer == 0) {
        for (unsigned int i = 0; i < s_allocation_register_index; i++) {
            free((void *)s_allocation_register[i]);
        }
        free(s_allocation_register);
        return;
    }
    
    for (unsigned int i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (long unsigned int) buffer) {
            free((void *)s_allocation_register[i]);
            s_allocation_register[i] = 0;
            for (unsigned int j = i+1; j < s_allocation_register_size; j++) {
                if (s_allocation_register[j] == 0) {
                    break;
                }
                s_allocation_register[j-1] = s_allocation_register[j];
            }
            break;
        }
    }
    if (s_allocation_register_index < s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE) {
        s_allocation_register = realloc(s_allocation_register, sizeof(long unsigned int) * (s_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(s_allocation_register)
        s_allocation_register_size -= DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    s_allocation_register_index--;
    sem_post(&s_alloc_register_mutex);
    return;
}

void dstudio_init_memory_management() {
    sem_init(&s_alloc_register_mutex, 0, 1);
    s_allocation_register = malloc(sizeof(long unsigned int) * s_allocation_register_size);
    DSTUDIO_EXIT_IF_NULL(s_allocation_register)
    explicit_bzero(s_allocation_register, sizeof(long unsigned int) * s_allocation_register_size);
}

void * dstudio_realloc(void * buffer, unsigned int new_size) {
    if (buffer == NULL) {
        return dstudio_alloc(new_size);
    }
    void * new_buffer = 0;
    sem_wait(&s_alloc_register_mutex);
    for (unsigned int i = 0; i < s_allocation_register_size; i++) {
        if(s_allocation_register[i] == (long unsigned int) buffer) {
            new_buffer = realloc(buffer, new_size);
            if (!new_buffer) {
                sem_post(&s_alloc_register_mutex);
                return NULL;
            }
            s_allocation_register[i] = (long unsigned int) new_buffer;
            break;
        }
    }
    sem_post(&s_alloc_register_mutex);
    return new_buffer;
}

double get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    return (double) (timestamp.tv_sec * 1000 + timestamp.tv_nsec / 1000000) / 1000.0;
}
