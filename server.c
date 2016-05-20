#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

//#include <errno.h>
#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h> // fila de mensagens

//Bibliotecas do usuario
#include "tipos.h"
#include "error_handlers.h"

//Server utilities
void shutdownServer( int msqid, struct msqid_ds buff );

int main(int argc, char** argv){
    int s1_pid, s2_pid;
    //int idshm;
    //pid_t *pshm_int;
    int msqid_1;

    //Variaveis para remover a fila
    

    ////Cria estrutura para a mensagem interna a mensagem
    //typedef struct inner_msgF{
    //    pid_t pid;
    //    int pageN;
    //} i_msgF;
    ////Cria estrutura da mensagem enviada e recebida
    //typedef struct msg_wrapper{
    //    long msgtype;
    //    i_msgF mtext;
    //} msgW;

    msg_t *msgp = (msg_t*) malloc( sizeof(msg_t) );

    //Ciria fila de mensagens
    if( ( msqid_1 = msgget(0x116175, IPC_CREAT | 0x1ff) ) < 0 ){   
        die("Erro na criacao da fila de mensagens.");
    }

    ////Cria memoria compartilhada para se comunicar com os processos filhos
    //if( ( idshm = shmget(0x2222, sizeof( pid_t ), IPC_CREAT | 0x1ff) ) < 0 ){
    //    die("Erro na criacao da memoria compartilhada");
    //}
    //if( ( pshm_int = shmat( idshm, NULL, 0 ) ) == ( int*) -1 ){
    //    die("Erro no attach");
    //}
    
    //Coloco o id do pai para os filhos terem acesso
    //Nao preciso porque tenho getppid()
    
    //*pshm_int = getpid();

    //Cria Processo de alocacao de paginas (PAP)
    if( ( s1_pid = fork() ) < 0 ){
        die("Erro na criacao do processo");
    }
    //Processo filho 1 (PAP)
    if( s1_pid == 0 ){
        execl("./pap", "pap", NULL); 
    }

    // pai continua

   // //Cria Processo de substituicao de paginas (PSP)
   // if( ( s1_pid = fork() ) < 0 ){
   //     die("Erro na criacao do processo");
   // }
   // //Processo filho 2 (PSP)
   // if( s1_pid == 0 ){
   //     execl("./psp", "psp");
   // }


    //Espera ate ser assassinado
    while( 1 ){
        if( ( msgrcv( msqid_1, msgp, sizeof( i_msgF ), 0, 0 ) ) < 0 ){
            die("Erro na leitura da mensagem na fila");
        }
        
        printf("Cliente com id %ld e mensagem %d\n", msgp->mtext.pid, msgp->mtext.pageN)   
    }

    
    return 0;
}

void shutdownServer( int msqid, struct msqid_ds buff ){
    //deleta a fila
    msgctl( msqid_1, IPC_RMID, &buff );
    printf("Server murdered\n");

}

