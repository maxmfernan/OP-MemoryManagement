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


