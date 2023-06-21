#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdlib.h>

typedef struct queue{
    int size;
    int begin;
    int fila[99];
    bool exists;
    bool fantasma;
}Queue;

enum speed{
    lento,
    normal,
    rapido,
};

int main(){

    int pid, idshm;
    int pid_child[4];
    Queue child_filas[4];
    for(int i=0;i<4;i++){
        child_filas[i].size=0;
        child_filas[i].begin=0;
        child_filas[i].exists=false;
        child_filas[i].fantasma=false;
    }

    FILE *file;
    char line[100];
    
    file = fopen("teste.txt", "r"); 

    if (file == NULL) {
        printf("Failed to open the file.\n");
    }
    
    int count =0;
    enum speed vel =0;

    while (fgets(line, sizeof(line), file)!=NULL ) {
        int len = strlen(line);
        if (len > 0 && line[len-1] == '\n')
            line[len-1] = 0;

        if (strcmp(line,"normal") == 0){
            vel = normal;
        }
        if (strcmp(line,"lento")==0){
            vel = lento;
        }
        if (strcmp(line,"rapido")==0){
            vel = rapido;
        }
        child_filas[count%4].fila[child_filas[count%4].size]=vel;
        child_filas[count%4].size++; 
        count++;       
    }
    for(int i=0;i<4;i++){
        for(int j=0;j<child_filas[i].size;j++){
            printf("processo %d - %d\n", i, child_filas[i].fila[j]);
        }
    }
    printf("\n");
    int status;
    int initial_count = count;
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    while(count>0){
        for(int i=0;i<4;i++){
            if(child_filas[i].exists == false && child_filas[i].size != 0){
                child_filas[i].exists = true;
                pid = fork();
                if(pid == 0){
                    switch(child_filas[i].fila[child_filas[i].begin]){
                        case lento:
                            printf("Proc %d - PID [%d]: executando lento\n", i, getpid());
                            execl("lento", "lento", (char *)NULL);
                            break;
                        case normal:
                            printf("Proc %d - PID [%d]: executando normal\n", i, getpid());
                            execl("normal", "normal", (char *)NULL);
                            break;
                        case rapido:
                            printf("Proc %d - PID [%d]: executando rapido\n", i, getpid());
                            execl("rapido", "rapido", (char *)NULL);
                            break;
                    }
                }
                else{
                    pid_child[i]=pid;
                }
            }
        }
        pid = wait(&status);
        for(int i=0;i<4;i++){
            if(pid_child[i]==pid){
                child_filas[i].exists = false;
                child_filas[i].begin++;
                child_filas[i].size--;
                count--;
                if(child_filas[i].size == 0){
                    if (!child_filas[i].fantasma) {
                        child_filas[i].fantasma = true;
                        printf("Proc %d iniciou o modo roubo de processos\n", i);
                    }
                    for (int j = 0; j < 4; j++) {
                        if (child_filas[j].size > 1 && !child_filas[j].fantasma) {
                            child_filas[i].size++;
                            child_filas[i].fila[child_filas[i].size] = child_filas[j].fila[child_filas[j].size];
                            child_filas[i].begin = child_filas[i].size;
                            child_filas[j].size--;
                            printf("Proc %d roubou do processo %d\n", i, j);
                            break;
                        }
                    }
                }

                break;
            }
        }

    }

    // timer end
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;
    printf("Turnarround time: %.3f segundos.\n", elapsed);
    
}