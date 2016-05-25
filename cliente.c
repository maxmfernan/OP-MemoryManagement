#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

#include <string.h>

//#include <errno.h>
#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h> // memoria compartilhada
#include <sys/msg.h> // fila de mensagens

//Bibliotecas do usuario
#include "tipos.h"
#include "error_handlers.h"

#define RPNUM_MAX 50
void referencia_pagina(msg_t msg, int msqid_1);

int main(int argc, char **argv){
    int msqid_1; //id da primeira fila
    int idshm_3; //id da memoria compartilhada
    pid_t *pshm_pids;
    FILE *input_fp; //stream do arquivo de entrada. p de pointer
    //char *input_path; //caminho do arquivo de entrada
    char r_pages[RPNUM_MAX]; //vetor com as pa'ginas requisitadas
    int aux_idx = 0; //i'ndice de prop'osito geral usado para iterar sobre algumas EDs.
    msg_t msg; //mensagem que ser'a enviada para o servidor
    //char aux_c; //caractere auxiliar usado para guardar o valor lido do arquivo

    if( argc != 2 ){
        die("Erro no nu'mero de argumentos.\n O segundo argumento"\
        "e' o caminho para o arquivo de entrada" );
    }
    if( ( input_fp = fopen( argv[1], "rt") ) == NULL ){
        die("Erro na aberura do arquivo.");
    }
   
    
    while( ( r_pages[aux_idx] = fgetc( input_fp ) ) != EOF ){
        //Suponho que o fomarto do arquivo de entrada est'a correto.
        if( ( r_pages[aux_idx] == ',' || r_pages[aux_idx] == ',' || r_pages[aux_idx] == '\n' \
            || r_pages[aux_idx] == '\r')  ){ //vai que algue'm queira rodar no windows
            continue;
        }
        aux_idx++;
        if( aux_idx == 49 ){
            break;
        }
    }
    r_pages[aux_idx] = '\0'; //so pra saber onde parar qnd for ler os dados
    aux_idx = 0;

   //Obtem id da fila de mensagens
    if( ( msqid_1 = msgget(0x116170, 0x1ff) ) < 0 ){   
        die("Erro na obtencao do id da fila de mensagens.");
    }
    //Guarda id de todo mundo que morrera com o shutdown                                           
    //Na primeira posicao guarda a quantidade de elemntos validos no vetor
    if( ( idshm_3 = shmget(0x116176, ( CLIENTP_MAX + 2 )*sizeof( pid_t ), 0x1ff) ) < 0 ){
        die("Erro na obtencao da memoria compartilhada");
    }
    if( ( pshm_pids = shmat( idshm_3, NULL, 0 ) ) == ( pid_t*) -1 ){
        die("Erro no attach");
    }
    //SEMAFORO
    pshm_pids[0]++;
    pshm_pids[pshm_pids[0]] = getpid();

    while( r_pages[aux_idx] != '\0' ){
        msg.msgtype = getpid(); //valor diferente de zero
        msg.mtext.pid = getpid();
        msg.mtext.pageN =  r_pages[aux_idx] - '0'; //a little bit of hackish
        aux_idx++;
        referencia_pagina( msg, msqid_1 ); 
    } 
    aux_idx = 0;

   while(1){} 
}

void referencia_pagina(msg_t msg, int msqid_1){
    if( ( msgsnd( msqid_1, &msg, sizeof( i_msgF), 0 ) ) < 0 ){
        die("Erro no envio da mensagem.\n");
    }
}
