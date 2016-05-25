#define STD_LIBS
#include <stdio.h>
#include <stdlib.h>

//#include <errno.h>
#include <unistd.h> //execl() etc
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h> // fila de mensagens
#include <signal.h>

//Bibliotecas do usuario
#include "tipos.h"
#include "error_handlers.h"

////Server utilities 
//#define NUMERO_FRAMES 10
//#define MAX_OCUPACAO 9
//#define OCUPACAO_OK 8
//#define CLIENTP_MAX 10 //a professora disse que seria 5. CLIENT PROCESS

//Server utilities
void shutdownServer( );

int pageAllocator(  msg_t *msg, pf_his_t *clt_pfh, unsigned int *gen_pfh, frame_t *pshm_frms, \
unsigned int *pshm_cufrms );

int s1_pid; //

//id das memorias compartilhadas e da fila de mensagem
int idshm_1, idshm_2, idshm_3;
int msqid_1;
//variaveis para registro do historio de page fault
frame_t *pshm_frms;
unsigned int *pshm_cufrms; //counter used frames

int main(int argc, char** argv){
    //int s1_pid; //

    //Varia'veis relacionadas a memo'ria compartilhada compartilhada
    //int idshm_1, idshm_2, idshm_3;
    //frame_t *pshm_frms;
    //unsigned int *pshm_cufrms; //counter used frames
    pid_t *pshm_pids;

    //Vari'veis relacionadas a fila de mensagem
    //int msqid_1;
    msg_t *msgp = (msg_t*) malloc( sizeof(msg_t) );

    //Varia'veis de histo'rico.
    //Serao responsa'veis por contar os page fault gerais e por pa'gina.
    pf_his_t clients_pfh[CLIENTP_MAX]; // clients page fault history
    unsigned int general_pfh = 0;
    //inicializacao
    for( int i = 0 ; i < CLIENTP_MAX; i++){
        clients_pfh[i].pid = 0;
        clients_pfh[i].pf_counter = 0;
    }
    

    ////Varia'veis para os frames
    //frame_t frames[NUMERO_FRAMES]; 
    //unsigned int cused_frames = 0; //counter used frames

    //Ciria fila de mensagens
    if( ( msqid_1 = msgget(0x116170, IPC_CREAT | 0x1ff) ) < 0 ){   
        die("Erro na criacao da fila de mensagens.");
    }

    //Cria memoria compartilhada para se comunicar com os processos filhos
    //Guarda o frame
    if( ( idshm_1 = shmget(0x116176, NUMERO_FRAMES*sizeof( frame_t ), IPC_CREAT | 0x1ff) ) < 0 ){
        die("Erro na criacao da memoria compartilhada");
    }
    if( ( pshm_frms = shmat( idshm_1, NULL, 0 ) ) == ( frame_t*) -1 ){
        die("Erro no attach");
    }
    //Guarda o cused_frames
    if( ( idshm_2 = shmget(0x116177, sizeof( unsigned int ), IPC_CREAT | 0x1ff) ) < 0 ){
        die("Erro na criacao da memoria compartilhada");
    }
    if( ( pshm_cufrms = shmat( idshm_2, NULL, 0 ) ) == ( unsigned int*) -1 ){
        die("Erro no attach");
    }
    //Guarda id de todo mundo que morrera com o shutdown                                           
    //Na primeira posicao guarda a quantidade de elemntos validos no vetor
   if( ( idshm_3 = shmget(0x116178, ( CLIENTP_MAX + 2 )*sizeof( pid_t ), IPC_CREAT | 0x1ff) ) < 0 ){
        die("Erro na criacao da memoria compartilhada");
    }
    if( ( pshm_pids = shmat( idshm_1, NULL, 0 ) ) == ( pid_t*) -1 ){
        die("Erro no attach");
    }
    for( int i = 0; i <= CLIENTP_MAX; i++){
        pshm_pids[i]=0;
    } 
    //SEMAFORO
    pshm_pids[0]++;
    pshm_pids[pshm_pids[0]] = getpid();
    
    #ifdef DEBUG
        printf("\npshm_pids %d\n", pshm_pids[0]);
    #endif

    //signal para terminar tudo
    signal( SIGTERM, shutdownServer ); 

    #ifdef DEBUG
        printf("msqid %d\n", msqid_1);
        printf("idshm_1 %d\n", idshm_1);
        printf("idshm_2 %d\n", idshm_2);
    #endif
    //Zera os contadores que indicam uso recente do frame.
    //Zera o indicador de uso
    for( int i = 0 ; i < NUMERO_FRAMES; i++ ){
        //pshm_frms[i].used = 0;
        pshm_frms[i].pid = 0;
        pshm_frms[i].rusedc = 0; 
    }

    //Cria Processo de substituicao de pa'gina (PSP)
    if( ( s1_pid = fork() ) < 0 ){
        die("Erro na criacao do processo");
    }
    //Processo filho 1 (PSP)
    if( s1_pid == 0 ){
        execl("./psp", "psp", NULL); 
    }

    // pai continua


    //Espera ate ser assassinado
    {
        //INCOMPLETO
        unsigned int aux_idx;
        unsigned int comutador;
        unsigned int achei; //1 verdade
        while( 1 ){
            aux_idx = 0;
            comutador = 0;
            achei = 0; //1 verdade

            if( ( msgrcv( msqid_1, msgp, sizeof( i_msgF ), 0, 0 ) ) < 0 ){
                die("Erro na leitura da mensagem na fila");
            }
            //configura o histo'rico
            #ifdef DEBUG
                printf("Mensagem recebida: id %d pag %d\n", msgp->msgtype, msgp->mtext.pageN);
            #endif
            for( int i = 0; i < CLIENTP_MAX; i++ ){
                #ifdef DEBUG
                    //printf( "Id cliente %d ", clients_pfh[i].pid );
                    //printf( "Contador de uso %d\n", clients_pfh[i].pf_counter );
                #endif
                if( comutador == 0 && clients_pfh[i].pid == 0 ){//encontrei o primeiro vazio
                    aux_idx = i;
                    comutador = 1;
                }
                else if( clients_pfh[i].pid == msgp->mtext.pid ){
                         aux_idx = i;
                         achei = 1;
                         break;
                     }
            }

            if( achei == 0 ){//o id do cliente nao constava no historico
                clients_pfh[aux_idx].pid = msgp->mtext.pid;
            }
            #ifdef DEBUG
                //printf("Cliente ja no historico\n");
                //printf( "Id cliente %d ", clients_pfh[aux_idx].pid );
                //printf( "Contador de uso %d\n", clients_pfh[aux_idx].pf_counter );
            #endif
            //Para iteracao seguinte
            achei = 0;
            comutador = 0;

            switch( pageAllocator( msgp, clients_pfh, &general_pfh, pshm_frms, pshm_cufrms) ){
                case 0:
                    #ifdef DEBUG
                        printf("Caso 0 do switch\n");
                    #endif
                    break;
                case 1:
                    clients_pfh[aux_idx].pf_counter++;      
                    general_pfh++;
                    break;
                case 2:
                    #ifdef DEBUG
                        printf("Caso 2 do switch\n");
                    #endif
                    clients_pfh[aux_idx].pf_counter++;      
                    kill( s1_pid, SIGUSR1 ); //acorda o PSP 
                    break;
                default:
                    die("Erro no pageAllocator. Valor de retorno nao esperado.\n");
            }
            
            //printf("Cliente com id %d e mensagem %d\n", msgp->mtext.pid, msgp->mtext.pageN);
        }
    }
    
    return 0;
}

//codigo de retorno:
//0 - pa'gina no frame. Sem page fault. Alocacao ja' feita.
//1 - page fault com frames livres e prontos para uso. Allocacao ja' feita.
//2 - page fault sem frames pronto para uso. Chamar o PSP.
int pageAllocator(msg_t *msg, pf_his_t *clt_pfh, unsigned int *gen_pfh, frame_t *pshm_frms, \
unsigned int *pshm_cufrms){

    unsigned int aux_idx = 0; // artifi'cio para nao precisar percorrer o vertor de frames 2 vezes
    unsigned int comutador = 0;
    
    //SEMA'FORO
    //Verifica se a pagina requisitada est'a alocada
    //Se o contador de recentemente usado for zero significa que o frame est'a livre.
    //Preciso verificar todo o vertor de frames
    for(int i = 0; i < NUMERO_FRAMES; i++){
        #ifdef DEBUG
            //printf("\nDentro do alocador\n");
            //printf("Vetor de frames\n");
            //printf( "Id cliente %d ", pshm_frms[i].pid );
            //printf( "Contador de uso %d\n", pshm_frms[i].rusedc );
        #endif

        //se 0 significa que nunca foi usado e portando e um espaco livre
        if( pshm_frms[i].rusedc != 0 ){//frame ocupado?
            if( msg->msgtype == pshm_frms[i].pid && msg->mtext.pageN == pshm_frms[i].page ){
                //incremento o contador de pagina recentemente usada
                pshm_frms[i].rusedc++;
                //pagina no frame
                #ifdef DEBUG
                    printf("\nRetornando do alocador com valor 0\n");
                    printf( "Nao houve page fault.\n " );
                    printf( "Indice no vetor de frames com a pagina %d\n",i );
                #endif

                return 0; 
            }
        }
        else if( comutador == 0 ){
            aux_idx = i; //encontrei um frame desocupado
            comutador = 1;
            #ifdef DEBUG
                //printf("\nAlocador\n");
                //printf( "Indice livre no fetor %d\n", aux_idx );
            #endif

        }
    } 
    //Se saiu a pagina nao est'a mapeada
    #ifdef DEBUG
        printf("Valor do pshm_cufrms %d", *pshm_cufrms);
    #endif
    if( *pshm_cufrms < MAX_OCUPACAO ){
        //Se entrou e' porque tem espaco no vertor de frames
        //O espaco livre est'a indicado por aux_idx.
        pshm_frms[aux_idx].pid = msg->msgtype; 
        pshm_frms[aux_idx].page = msg->mtext.pageN; 
        pshm_frms[aux_idx].rusedc++; //contador de pagina recentemente usada
        (*pshm_cufrms)++;
        #ifdef DEBUG
                printf("\nAlocador retorna 1\n");
                printf("Deu pagefault mas o sub nao e chamado\n ");
                printf("Indice no vetor de frames %d\n", aux_idx);
                printf("Valores da mensagem:\n");
                printf("Id %d Pagina %d\n", msg->msgtype, msg->mtext.pageN);
                printf("Valores no frame:\n");
                printf("Id %d Pagina %d\n", pshm_frms[aux_idx].pid, pshm_frms[aux_idx].page);
        #endif

        return 1;
    }
    
    return 2;
}

void shutdownServer( ){
    struct msqid_ds buff; 
    struct shmid_ds shm_buff;
    
    //Mata o filho
    kill( SIGKILL, s1_pid );

    printf("------------------------------\n");
    printf("%s%23s%24s\n", "ID", "pagina", "Tempo de referencia");
    for(int i = 0; i < NUMERO_FRAMES; i++){
        if( pshm_frms[i].rusedc == 0 ){
            printf("%-17s\n", "livre");
            continue;
        }
        else
            printf("%-17d", pshm_frms[i].pid);

        printf("%2s%-3d", " ", pshm_frms[i].page);
        printf("%8s%-3d\n", " ", pshm_frms[i].rusedc);
    }
    //deleta a fila
    msgctl( msqid_1, IPC_RMID, &buff );
    //deleta as memorias compartilhadas
    shmctl( idshm_1, IPC_RMID, &shm_buff); 
    shmctl( idshm_2, IPC_RMID, &shm_buff); 
    shmctl( idshm_3, IPC_RMID, &shm_buff); 
    
    printf("Server murdered\n");
    kill(0); //se mata
    
    return;
}

