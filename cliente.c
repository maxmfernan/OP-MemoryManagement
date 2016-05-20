#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

//#include <errno.h>
#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> // memoria compartilhada
#include <sys/msg.h> // fila de mensagens

//Bibliotecas do usuario
#include "tipos.h"
#include "error_handlers.h"

int main(int argc, char **argv){
    int msqid_1;

    msg_t msg;

    //Obtem id da fila de mensagens
    if( ( msqid_1 = msgget(0x116175, 0x1ff) ) < 0 ){   
        die("Erro na obtencao do id da fila de mensagens.");
    }

    printf("Id da fila %d\n", msqid_1);

    //Digite uma mensagem
    printf("Digite uma mensagem: ");
    scanf("%d", &(msg.mtext.pageN) );
    msg.mtext.pid = getpid(); //id deste processo
    msg.msgtype = getpid();

    if( ( msgsnd( msqid_1, &msg, sizeof( i_msgF), 0 ) ) < 0 ){
        die("Erro no envio da mensagem.\n");
    }
    //while(1){}
    
}
