#ifndef IPC_LIBS
#include <unistd.h> //execl() etc
#include <sys/types.h>
#endif 

//Server utilities 
#define NUMERO_FRAMES 10
#define MAX_OCUPACAO 9
#define OCUPACAO_OK 8
#define CLIENTP_MAX 10 //a professora disse que seria 5. CLIENT PROCESS

//Cria estrutura para a mensagem interna a mensagem
    typedef struct inner_msgF{
        pid_t pid;
        int pageN;
    } i_msgF;

//Cria estrutura da mensagem enviada e recebida
    typedef struct msg_wrapper{
        long msgtype;
        i_msgF mtext;
    } msg_t;

//Estruturas para o histo'rico dos page faults
    typedef struct pf_history{
        pid_t pid;
        unsigned int pf_counter;
    } pf_his_t;

//Estrutura para cada frame
    typedef struct frame{
        //unsigned int used; // 1 verdade 0 falso 
        pid_t pid;
        unsigned int page;
        unsigned int rusedc; //recently used counter
    } frame_t;

