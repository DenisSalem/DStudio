typedef struct InstanceContext_t {
    const char * instance_name;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
    unsigned int count;
} Instances;

void new_instance();
