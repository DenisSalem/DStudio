void count_instances(const char * directory, unsigned int * count, unsigned int * last_id);
unsigned int count_process(const char * process_name);
void expand_user(char ** dest, const char * directory);
double get_proc_memory_usage();
void recursive_mkdir(char * directory);
void set_physical_memory();
