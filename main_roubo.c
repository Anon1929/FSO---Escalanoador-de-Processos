#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/msg.h>

enum speed
{
    lento,
    normal,
    rapido,
};

typedef struct
{
    long tipo;
    char msg[1];
} mensagem;

long getType(long i)
{
    return (long)((i % 4) + 1) * 100l;
}

int randInt(int min, int max)
{
    return min + rand() % (max - min + 1);
}

int main()
{
    int msg_id, pid, status, count = 0;
    FILE *file;
    char line[100];

    // Criar fila de mensagens
    if (msg_id = msgget(0x1223, IPC_CREAT | 0666) == -1)
    {
        perror("msgget");
        exit(1);
    }

    file = fopen("teste.txt", "r");
    if (file == NULL)
    {
        printf("Failed to open the file.\n");
    }

    while (fgets(line, sizeof(line), file) != NULL)
    {
        int len = strlen(line);
        enum speed vel = 0;

        if (len > 0 && line[len - 1] == '\n')
        {
            line[len - 1] = 0;
        }

        if (strcmp(line, "normal") == 0)
        {
            vel = normal;
        }
        if (strcmp(line, "lento") == 0)
        {
            vel = lento;
        }
        if (strcmp(line, "rapido") == 0)
        {
            vel = rapido;
        }

        // mandar mensagem
        mensagem msg;
        long type = getType(count % 4);
        msg.tipo = type;
        msg.msg[0] = vel;
        printf("Fila %ld recebe processo %s\n", msg.tipo, line);
        if (msgsnd(msg_id, &msg, sizeof(msg.msg), IPC_NOWAIT) == -1)
        {
            perror("Error sending message to queue");
            exit(1);
        }

        count++;
    }

    // timer start
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    for (int i = 0; i < 4; i++)
    {
        int fantasma = 0;
        pid = fork();
        if (pid == 0)
        {
            printf("Auxiliar %d criado\n", i);
            while (true)
            {
                mensagem msg;
                if (!fantasma)
                {
                    long type = getType(i);
                    msg.tipo = type;
                    if (msgrcv(msg_id, &msg, sizeof(msg.msg), type, IPC_NOWAIT) == -1)
                    {
                        printf("Auxiliar %d inicou o modo roubo de processo\n", i);
                        fantasma = 1;
                        continue;
                    }
                }
                else
                {
                    // Gera um array aleatória entre 0 e 3
                    int indices[] = {0, 1, 2, 3};
                    for (int i = 0; i < 4; i++)
                    {
                        int j = randInt(0, 3);
                        int temp = indices[i];
                        indices[i] = indices[j];
                        indices[j] = temp;
                    }
                    // Testa todos os indices do array, checando se tem mensagem. Se não tiver, finaliza o processo. Se tiver, rouba
                    for (int j = 0; j < 4; j++)
                    {
                        long type = getType(indices[j]);
                        msg.tipo = type;
                        if (msgrcv(msg_id, &msg, sizeof(msg.msg), type, IPC_NOWAIT) == -1)
                        {
                            if (j == 3)
                            {
                                printf("Auxiliar %d terminou a execução\n", i);
                                exit(1);
                            }
                        }
                        else
                        {   
                            printf("Auxiliar %d roubou do auxiliar %d\n", i, type/100 -1);
                            break;
                        }
                    }
                }

                int processVel = (int)msg.msg[0];
                int pidProg = fork();
                if (pidProg == 0)
                {
                    switch (processVel)
                    {
                    case lento:
                        printf("Auxiliar %d - PID [%d]: executando lento\n", i, getpid());
                        execl("lento", "lento", (char *)NULL);
                        // sleep(3);
                        break;
                    case normal:
                        printf("Auxiliar %d - PID [%d]: executando normal\n", i, getpid());
                        execl("normal", "normal", (char *)NULL);
                        // sleep(2);
                        break;
                    case rapido:
                        printf("Auxiliar %d - PID [%d]: executando rapido\n", i, getpid());
                        execl("rapido", "rapido", (char *)NULL);
                        // sleep(1);
                        break;
                    }
                    exit(-1);
                }
                wait(&status);
            }
        }
    }

    // espera que todos os auxiliares terminem
    for (int i = 0; i < 4; i++)
    {
        wait(&status);
    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("Turnaround time: %.3f segundos.\n", elapsed);
    exit(0);
}