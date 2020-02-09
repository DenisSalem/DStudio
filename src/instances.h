#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include "interactive_list.h"
#include "voices.h"

typedef struct InstanceContext_t {
    char name[33];
    int identifier;
    Voices voices;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
    unsigned int index;
    ThreadControl thread_control;
} Instances;

extern Instances g_instances;
extern InstanceContext * g_current_active_instance;
extern UIInteractiveList g_ui_instances;

void add_instance_file_descriptor();

void exit_instances_management_thread();

void init_instances_management_thread(
    UIElements * ui_elements,
    unsigned int lines_number,
    unsigned int string_size,
    GLfloat item_offset_y
);
void * instances_management_thread(void * args);

void new_instance(
    const char * given_directory,
    const char * process_name
);

unsigned int select_instance_from_list(unsigned int index);

void update_instances_ui_list();

void update_current_instance(unsigned int index);
#endif
