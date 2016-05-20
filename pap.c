#include <stdio.h>
#include <sys/types.h>
#include <signal.h>

#include <unistd.h>

int main(int argc, char** argv){
    int num; 
    pid_t ppid;
    
    //Obtem o id do pai
    ppid = getppid();

    while(1){
        printf("Digite 5: ");
        scanf("%d", &num);
        if( num == 5 ){
            kill( ppid, SIGKILL );
            break;
        }
    }
    
    printf("PAP saiu.\n");

    return 0;
}
