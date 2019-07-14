typedef struct InstanceContext_t {
    const char * instance_name;
} InstanceContext;

typedef struct Instances_t {
    InstanceContext * contexts;
} Instances;

void new_instance();
