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
    UIElements * lines;
    unsigned int lines_number;
    unsigned int string_size;
    int window_offset;
    sem_t mutex;
    int cut_thread;
    int ready;
    int update;
} UIInstances;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
    UIInstances * ui;
} Instances;

void exit_instances_thread();
void init_instances_ui(UIElements * lines, unsigned int lines_number, unsigned int string_size);
void new_instance(const char * given_directory, const char * process_name, Instances * instances);

// Periodically check if new instances were added
void * update_instances(void * args);
void * update_instances_text();
#endif
