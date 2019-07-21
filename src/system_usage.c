#include "system_usage.h"
#include <stdio.h>
void update_system_usage(SystemUsage * system_usage) {
    getrusage(RUSAGE_SELF,&system_usage->r_usage);
    // Print the maximum resident set size used (in kilobytes).
    printf("Memory usage: %ld kilobytes\n",system_usage->r_usage.ru_maxrss);
}
