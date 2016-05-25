#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

#include "error_handlers.h"
#include "tipos.h"

#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h> // fila de mensagens
#include <signal.h>

int main( int argc, char** argv ){
    int idshm_3;
    //int msqid_1;
    pid_t *pshm_pids;


    //Obtem memoria compartilhada
    //Guarda o frame
    if( ( idshm_3 = shmget(0x116178, ( CLIENTP_MAX + 2 )*sizeof( pid_t ), 0x1ff) ) < 0 ){
        die("Erro na obtencao da memoria compartilhada");
    }
    if( ( pshm_pids = shmat( idshm_3, NULL, 0 ) ) == ( pid_t*) -1 ){
        die("Erro no attach");
    }

    //SEMAFORO ?
    #ifdef DEBUG
        printf("\n pshm_pids[0] %d\n", pshm_pids[0]);
    #endif
    if( pshm_pids[0] != 0 ){
        kill( pshm_pids[1], SIGTERM ); 
    }
    else
        die("Erro no processo shutdown. Id do servidor nao colocado na memoria");

    return 0;
}
