typedef struct InstanceContext_t {
    const char * instance_name;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
} Instances;

void new_instance();

// Periodically check if new instances were added
void * update_instances(void * args);
