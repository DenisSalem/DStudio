#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"

GLint scissor_x, scissor_y = 0;
GLsizei scissor_width, scissor_height = 0;

static long unsigned int * g_allocation_register = 0;
static unsigned int g_allocation_register_index = 0;
static unsigned int g_allocation_register_size = DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
sem_t g_alloc_register_mutex = {0};

void dstudio_cut_thread(ThreadControl * thread_control) {
    sem_wait(&thread_control->mutex);
    thread_control->cut_thread = 1;
    sem_post(&thread_control->mutex);
} 

void * dstudio_alloc(unsigned int buffer_size) {
    sem_wait(&g_alloc_register_mutex);
    if (g_allocation_register == 0) {
        g_allocation_register = malloc(sizeof(long unsigned int) * g_allocation_register_size);
        DSTUDIO_EXIT_IF_NULL(g_allocation_register)
        explicit_bzero(g_allocation_register, sizeof(long unsigned int) * g_allocation_register_size);
    }
    if (g_allocation_register_index >= g_allocation_register_size) {
        g_allocation_register = realloc(g_allocation_register, sizeof(long unsigned int) * (g_allocation_register_size + DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(g_allocation_register)
        explicit_bzero(&g_allocation_register[g_allocation_register_size], sizeof(long unsigned int) * DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE);
        g_allocation_register_size += DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    g_allocation_register[g_allocation_register_index] = (long unsigned int) malloc(buffer_size);
    DSTUDIO_EXIT_IF_NULL(g_allocation_register[g_allocation_register_index])
    explicit_bzero((void *) g_allocation_register[g_allocation_register_index], buffer_size);
    sem_post(&g_alloc_register_mutex);
    return (void *) g_allocation_register[g_allocation_register_index++];
}

void dstudio_free(void * buffer) {
    sem_wait(&g_alloc_register_mutex);
    
    if (buffer == 0) {
        for (unsigned int i = 0; i < g_allocation_register_size; i++) {
            free((void *)g_allocation_register[i]);
        }
        free(g_allocation_register);
        return;
    }
    
    for (unsigned int i = 0; i < g_allocation_register_size; i++) {
        if(g_allocation_register[i] == (long unsigned int) buffer) {
            free((void *)g_allocation_register[i]);
            g_allocation_register[i] = 0;
            for (unsigned int j = i+1; j < g_allocation_register_size; j++) {
                if (g_allocation_register[j] == 0) {
                    break;
                }
                g_allocation_register[j-1] = g_allocation_register[j];
            }
            break;
        }
    }
    if (g_allocation_register_index < g_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE) {
        g_allocation_register = realloc(g_allocation_register, sizeof(long unsigned int) * (g_allocation_register_size - DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE));
        DSTUDIO_EXIT_IF_NULL(g_allocation_register)
        g_allocation_register_size -= DSTUDIO_ALLOCATION_REGISTER_CHUNK_SIZE;
    }
    g_allocation_register_index--;
    sem_post(&g_alloc_register_mutex);
    return;
}

double get_timestamp() {
    struct timespec timestamp;
    clock_gettime(CLOCK_REALTIME, &timestamp);
    return (double) (timestamp.tv_sec * 1000 + timestamp.tv_nsec / 1000000) / 1000.0;
}
