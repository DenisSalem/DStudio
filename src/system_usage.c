/*
 * Copyright 2019 Denis Salem
 *
 * This file is part of DStudio.
 *
 * DStudio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DStudio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with DStudio. If not, see <http://www.gnu.org/licenses/>.
*/

#include <time.h>
#include <unistd.h>

#include "extensions.h"
#include "fileutils.h"
#include "system_usage.h"
#include "window_management.h"

void init_system_usage_ui(UISystemUsage * system_usage, UIElements * text_cpu, UIElements * text_mem) {
    system_usage->ui_text_cpu = text_cpu;
    system_usage->ui_text_mem = text_mem;
    //~ init_text(
        //~ &system_usage->ui_text_cpu,
        //~ 0,
        //~ 6,
        //~ DSTUDIO_CHAR_TABLE_ASSET_PATH,
        //~ gl_x + ((GLfloat) (texture_system_usage_width+10) / ((GLfloat) DSTUDIO_VIEWPORT_WIDTH)),
        //~ gl_y + 0.027083,
        //~ NULL
    //~ );
    //~ init_text(
        //~ &system_usage->ui_text_mem,
        //~ 0,
        //~ 6,
        //~ DSTUDIO_CHAR_TABLE_ASSET_PATH,
        //~ gl_x + ((GLfloat) (texture_system_usage_width+10) / ((GLfloat) DSTUDIO_VIEWPORT_WIDTH)),
        //~ gl_y - 0.027083,
        //~ system_usage->ui_text_cpu.scale_matrix
    //~ );
    
    sem_init(&system_usage->mutex, 0, 1);
    system_usage->ready = 1;
}

void * update_system_usage(void * args) {
    //~ SystemUsage * system_usage = (SystemUsage *) args;
    //~ while(!system_usage->ui->ready) {
        //~ usleep(1000);
    //~ }
    
    //~ char * cpu_usage_string_value = 0;
    //~ char * mem_usage_string_value = 0;
    
    //~ Vec4 * ui_text_cpu_offset_buffer = system_usage->ui->ui_text_cpu.instance_offsets_buffer;
    //~ Vec4 * ui_text_mem_offset_buffer = system_usage->ui->ui_text_mem.instance_offsets_buffer;
    
    //~ unsigned int * ui_text_cpu_actual_size = &system_usage->ui->ui_text_cpu.actual_string_size;
    //~ unsigned int * ui_text_mem_actual_size = &system_usage->ui->ui_text_mem.actual_string_size;
    
    //~ GLuint ui_text_offset_buffer_object = system_usage->ui->ui_text_cpu.instance_offsets;
    //~ GLuint ui_mem_offset_buffer_object = system_usage->ui->ui_text_mem.instance_offsets;
    
    //~ cpu_usage_string_value = system_usage->ui->ui_text_cpu.string_buffer;
    //~ mem_usage_string_value = system_usage->ui->ui_text_mem.string_buffer;

    //~ while (1) {
        //~ clock_t cpu_time = clock();
        //~ usleep(250000);
        //~ sem_wait(&system_usage->ui->mutex);
        
        //~ if (system_usage->ui->cut_thread) {
            //~ sem_post(&system_usage->ui->mutex);
            //~ break;
        //~ }
        //~ system_usage->cpu_usage = (((double) (clock() - cpu_time) / (double) CLOCKS_PER_SEC) / 0.25) * 100.0;
        //~ system_usage->mem_usage = get_proc_memory_usage();

        //~ if (system_usage->cpu_usage != -1) {
            //~ sprintf(cpu_usage_string_value, "%0.1f%%", system_usage->cpu_usage);
        //~ }
        //~ if (system_usage->mem_usage != -1) {
            //~ sprintf(mem_usage_string_value, "%0.1f%%", system_usage->mem_usage);
        //~ }
        
        //~ send_expose_event();
        //~ system_usage->ui->update = 1;
        //~ sem_post(&system_usage->ui->mutex);
        
        //~ get_proc_memory_usage();
    //~ }
    
    //~ return NULL;
}
