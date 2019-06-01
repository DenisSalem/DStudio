#include <pthread.h>

#include "../common.h"
#include "ui.h"

int main(int argc, char ** argv) {
    UI ui;
    pthread_t ui_thread_id;
    
    RETURN_IF_FAILURE( pthread_create( &ui_thread_id, NULL, ui_thread, &ui) )
    RETURN_IF_FAILURE(pthread_join(ui_thread_id, NULL))
    
    return 0;
}
