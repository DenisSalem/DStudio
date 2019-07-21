#include <sys/resource.h>

typedef struct SystemUsage_t {
    struct rusage r_usage;
} SystemUsage;

void update_system_usage(SystemUsage * system_usage);
