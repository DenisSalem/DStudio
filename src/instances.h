#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include <semaphore.h>

#include "text.h"

char * instances_directory;

typedef struct InstanceContext_t {
    char name[33];
    int identifier;
} InstanceContext;

typedef struct UIInstances_t {
    UIText * lines;
    int lines_number;
    int window_offset;
    sem_t mutex;
    int cut_thread;
    int ready;
    int update;
    Vec2 scale_matrix[2];
} UIInstances;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
    UIInstances * ui;
} Instances;

void exit_instances_thread();
void init_instances_ui(int lines_number, unsigned int viewport_width, unsigned int viewport_height, GLfloat pos_x, GLfloat pos_y);
void new_instance(const char * given_directory, const char * process_name, Instances * instances);

// Periodically check if new instances were added
void * update_instances(void * args);
void * update_instances_text();
#endif
