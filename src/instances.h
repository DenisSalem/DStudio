#ifndef DSTUDIO_INSTANCES_H_INCLUDED
#define DSTUDIO_INSTANCES_H_INCLUDED

#include "interactive_list.h"
#include "text.h"
#include "voices.h"

typedef struct InstanceContext_t {
    char name[33];
    int identifier;
    Voices voices;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int index;
    unsigned int count;
} Instances;

extern Instances g_instances;
extern InstanceContext * g_current_active_instance;
extern UIInteractiveList g_ui_instances;
extern InteractiveListContext g_instances_list_context;

void exit_instances_thread();

char * get_instance_target_name(unsigned int index);

void init_instances_ui(
    UIElements * lines,
    unsigned int max_lines_number,
    unsigned int string_size,
    UIElementSettingParams * params
);

void new_instance(
    const char * given_directory,
    const char * process_name
);

void scroll_instances(void * args);
void select_instance_from_list(unsigned int index);

// Periodically check if new instances were added
void update_current_instance(unsigned int index);
void * update_instances(void * args);
void update_instances_text();
#endif
