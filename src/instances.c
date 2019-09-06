#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>

#include <sys/stat.h>
#include <unistd.h>

#include "fileutils.h"
#include "instances.h"
#include "ui.h"

static struct stat st = {0};
static InstanceContext * current_active_instance = 0; 
static Instances * instances;

char * instances_directory = 0;

void exit_instances_thread() {
    sem_wait(&instances->ui->mutex);
    instances->ui->cut_thread = 1;
    sem_post(&instances->ui->mutex);
    
    char * instance_path = malloc(sizeof(char) * 128);
    explicit_bzero(instance_path, sizeof(char) * 128);
    sprintf(instance_path, "%s/%d", instances_directory, current_active_instance->identifier);
    unlink(instance_path);
}

void init_instances_ui(int lines_number, GLfloat pos_x, GLfloat pos_y) {
    //~ instances->ui->lines = malloc(sizeof(UIElements) * lines_number);
    //~ instances->ui->lines_number = lines_number;
    //~ Vec2 * scale_matrix = &instances->ui->scale_matrix[0];
    
    //~ scale_matrix[0].x = 0.5 * (((float) DSTUDIO_CHAR_TABLE_ASSET_WIDTH / (float) DSTUDIO_VIEWPORT_WIDTH) / DSTUDIO_CHAR_SIZE_DIVISOR);
    //~ scale_matrix[0].y = 0;
    //~ scale_matrix[1].x = 0;
    //~ scale_matrix[1].y = 0.5 * (((float) DSTUDIO_CHAR_TABLE_ASSET_HEIGHT / (float) DSTUDIO_VIEWPORT_HEIGHT) / DSTUDIO_CHAR_SIZE_DIVISOR);
    
    //~ for (int i = 0; i < lines_number; i++) {
        //~ init_text(
            //~ &instances->ui->lines[i],
            //~ 0,
            //~ 29,
            //~ DSTUDIO_CHAR_TABLE_SMALL_ASSET_PATH,
            //~ pos_x,
            //~ pos_y-i*(11.0/240.0),
            //~ scale_matrix
        //~ );
    //~ }
    //~ instances->ui->update = 1;
    //~ send_expose_event();
    //~ sem_init(&instances->ui->mutex, 0, 1);
    //~ instances->ui->ready = 1;
}

void new_instance(const char * given_directory, const char * process_name, Instances * given_instances) {
    //~ instances = given_instances;
    //~ unsigned int count = 0;
    //~ unsigned int last_id = 0;

    //~ DIR * dr = 0;
    //~ struct dirent *de;

    //~ int processes_count = count_process(process_name);
    //~ expand_user(&instances_directory, given_directory);
    //~ char * instance_filename_buffer = malloc(sizeof(char) * 128); 
    //~ explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
    //~ char string_representation_of_integer[4] = {0};
    //~ FILE * new_instance = 0;
    //~ if (stat(instances_directory, &st) == -1) {
        //~ // Permission error
        //~ if (errno == EACCES) {
            //~ printf("%s: %s\n", instances_directory, strerror(errno));
            //~ exit(-1);
        //~ }
        //~ recursive_mkdir(instances_directory);
    //~ }
    //~ if (processes_count > 1) {
        //~ count_instances(instances_directory, &count, &last_id);
        //~ strcat(instance_filename_buffer, instances_directory);
        //~ strcat(instance_filename_buffer, "/");
        //~ sprintf(string_representation_of_integer,"%d", last_id+1);
        //~ strcat(instance_filename_buffer, string_representation_of_integer);
        //~ new_instance = fopen(instance_filename_buffer, "w+");
        //~ fclose(new_instance);
        //~ exit(0);
    //~ }
    //~ else {
        //~ // Clear instances caches
        //~ dr = opendir(instances_directory);
        //~ while ((de = readdir(dr)) != NULL) {
            //~ strcat(instance_filename_buffer, instances_directory);
            //~ strcat(instance_filename_buffer, "/");
            //~ strcat(instance_filename_buffer, de->d_name);
            //~ if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
                //~ remove(instance_filename_buffer);
            //~ }
            //~ explicit_bzero(instance_filename_buffer, sizeof(char) * 128);
        //~ }
        //~ closedir(dr);
        //~ strcat(instance_filename_buffer, instances_directory);
        //~ strcat(instance_filename_buffer, "/1");
        //~ FILE * new_instance = fopen(instance_filename_buffer, "w+");
        //~ fclose(new_instance);
        //~ instances->contexts = malloc( sizeof(InstanceContext) );
        //~ if (instances->contexts) {
            //~ instances->count +=1;
            //~ instances->contexts[0].identifier = 1;
            //~ current_active_instance  = &instances->contexts[0];
            //~ strcpy(current_active_instance->name, "Instance 1");
        //~ }
    //~ }
    //~ free(instance_filename_buffer);
}

void * update_instances(void * args) {
    //~ DIR * dr = 0;
    //~ struct dirent *de;
    //~ int fd = 0;
    //~ int wd = 0;
    //~ InstanceContext * saved_contexts = 0;
    //~ while(!instances->ui->ready) {
        //~ usleep(1000);
    //~ }
    
    //~ fd = inotify_init();
    //~ if (errno != 0 && fd == -1) {
        //~ printf("inotify_init(): failed\n");
        //~ exit(-1);
    //~ }
    //~ wd = inotify_add_watch(fd, instances_directory, IN_CREATE | IN_DELETE);
    //~ if (errno != 0 && wd == 0) {
        //~ printf("inotify_add_watch(): failed\n");
        //~ exit(-1);
    //~ }

	//~ struct inotify_event  * event = malloc(sizeof(struct inotify_event) + 16 + 1);

    //~ while(1) {
        //~ if(read(fd, event, sizeof(struct inotify_event) + 16 + 1) < 0 && errno != 0) {
            //~ continue;
        //~ }
        
        //~ sem_wait(&instances->ui->mutex);
        //~ if (instances->ui->cut_thread) {
            //~ sem_post(&instances->ui->mutex);
            //~ break;
        //~ }
        //~ sem_post(&instances->ui->mutex);
        
		//~ if (event->mask == IN_CREATE) {
            //~ instances->count++;
            //~ saved_contexts = instances->contexts;
            //~ instances->contexts = realloc(instances->contexts, sizeof(InstanceContext) * instances->count);
            
            //~ if (instances->contexts == NULL) {
                //~ instances->contexts = saved_contexts;
                //~ instances->count--;
                //~ continue;
            //~ }
            
            //~ explicit_bzero(&instances->contexts[instances->count-1], sizeof(InstanceContext));
            //~ current_active_instance = &instances->contexts[instances->count-1];
            //~ current_active_instance->identifier = 1;
            //~ strcat(current_active_instance->name, "Instance ");
            //~ strcat(current_active_instance->name, event->name);
            //~ if (instances->count > instances->ui->lines_number) {
                //~ instances->ui->window_offset++;
            //~ }
            //~ instances->ui->update = 1;

            //~ #ifdef DSTUDIO_DEBUG
			//~ printf("Create instance with id=%s. Allocated memory is now %ld.\n", event->name, sizeof(InstanceContext) * instances->count);
            //~ printf("Currents instances:\n");
            //~ for(int i = 0; i < instances->count; i++) {
                //~ printf("\t%s\n", instances->contexts[i].name);
            //~ }
            //~ #endif
        //~ }
		
		//~ else if (event->mask == IN_DELETE) {
            //~ instances->count--;
            //~ // Move context data in memory and then realloc
            //~ #ifdef DSTUDIO_DEBUG
			//~ printf("Remove instance with id=%s\n", event->name);
            //~ #endif
        //~ }
    //~ }
}

void * update_instances_text() {
    //~ int offset = instances->ui->window_offset;
    //~ for(int i = 0; i < instances->ui->lines_number; i++) {
        //~ if (i + offset < instances->count) {
            //~ //strcpy(instances->ui->lines[i].string_buffer, instances->contexts[i+offset].name);
            //~ update_text(&instances->ui->lines[i]);
        //~ }
        //~ else {
            //~ //strcpy(instances->ui->lines[i].string_buffer, "");
            //~ update_text(&instances->ui->lines[i]);
        //~ }
    //~ }
}
