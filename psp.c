#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h> // fila de mensagens
#include <signal.h>

//Bibliotecas do usuario
#include "tipos.h"
#include "error_handlers.h"

void substitui( );

int idshm_1, idshm_2; //gambiarra

int main(int argc, char** argv){
    frame_t *pshm_frms;
    unsigned int *pshm_cufrms;

    //Obte'm acesso a memoria compartilhada para se comunicar com o processo pai
    if( ( idshm_1 = shmget(0x116176, NUMERO_FRAMES*sizeof( frame_t ),  0x1ff) ) < 0 ){
        die("Erro na criacao da memoria compartilhada");
    }
    if( ( pshm_frms = shmat( idshm_1, NULL, 0 ) ) == ( frame_t*) -1 ){
        die("Erro no attach");
    }

    if( ( idshm_2 = shmget(0x116177, sizeof( unsigned int ), 0x1ff) ) < 0 ){
        die("Erro na criacao da memoria compartilhada");
    }
    if( ( pshm_cufrms = shmat( idshm_2, NULL, 0 ) ) == ( unsigned int*) -1 ){
        die("Erro no attach");
    }
    
    signal( SIGUSR1, substitui );

    while(1){
        //gasta cpu
    }
  
    return 0;
}

void substitui( ){
    unsigned int lused;   
    unsigned int start;
    frame_t *pshm_frms; 
    unsigned int *pshm_cufrms;

    if( ( pshm_frms = shmat( idshm_1, NULL, 0 ) ) == ( frame_t*) -1 ){
        die("Erro no attach");
    }
    if( ( pshm_cufrms = shmat( idshm_2, NULL, 0 ) ) == ( unsigned int*) -1 ){
        die("Erro no attach do substitui.");
    }

    while( *pshm_cufrms >= OCUPACAO_OK){
        lused = 0; //less used
        start = 0;
        for( int i = 0; i < NUMERO_FRAMES; i++ ){
            if( start == 0 && pshm_frms[i].rusedc == 0 ){
                continue;
            }
            else if(start == 0){ //seta pro primeiro nao zero encontrado
                start = 1;
                lused = i;
                continue;
            }
                    
            if( pshm_frms[i].rusedc == 0 ){
               continue; 
            } 
            if( pshm_frms[i].rusedc < pshm_frms[lused].rusedc ){
                lused = i;
            }
        
        }//fim do for
        
        //SEMA'FORO
        //Retira a pa'gina menos recentemente usada
        pshm_frms[lused].rusedc = 0; 
        (*pshm_cufrms)--;

    }//fim do while 
}
