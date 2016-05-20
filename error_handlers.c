#include "error_handlers.h"

void die( const char* err_msg ){
    if( errno )
        perror( err_msg );
    else
        printf("Error: %s\n", err_msg);

    exit(1);
}
