#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include <semaphore.h>

#include "text.h"

typedef struct InstanceContext_t {
    const char * instance_name;
    int identifier;
} InstanceContext;

typedef struct UIInstances_t {
    UIText * lines;
    sem_t mutex;
    int cut_thread;
    int ready;
} UIInstances;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
    UIInstances * ui;
} Instances;

void init_instances_ui();
void new_instance();

// Periodically check if new instances were added
void * update_instances(void * args);

#endif
