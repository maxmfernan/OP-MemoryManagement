#ifndef IPC_LIBS
#include <sys/types.h>
#include <sys/ipc.h>
#endif

#ifndef IPC_SEM
#include <sys/sem.h>
#endif

#include "error_handlers.h"

//O sema'foro deve ser inicializado com 1.
//O servidro deve ser o responsa'vel pela inicializacao.

typedef union semun{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
} semun_t;

struct sembuf operacao[2];//ate' duas operacoes
int idsem;
int p_sem(unsigned short semn){
    operacao[0].sem_num = semn;
    operacao[0].sem_op = -1;//bloquei recurso
    operacao[0].sem_flg = SEM_UNDO;
    if ( semop(idsem, operacao, 1) < 0)
        die("Erro no semaforo. P");
    
    return 0;
}

int v_sem(unsigned short semn){
    operacao[0].sem_num = semn;
    operacao[0].sem_op = 1;//libera recurso
    operacao[0].sem_flg = SEM_UNDO;
    if ( semop(idsem, operacao, 1) < 0)
        die("Erro no semaforo. V");

    return 0;
}
