#include <stdio.h>
int main(){
    printf("------------------------------\n");
    printf("%s%23s%24s\n", "ID", "pagina", "Tempo de referencia");
    printf("%-17d", 123456);
    printf("%2s%-3d", " ", 12);
    printf("%8s%-3d", " ", 123);
}
