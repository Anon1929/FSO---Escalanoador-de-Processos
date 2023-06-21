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
            line[len - 1] = 0;
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
        msg.tipo = (long)(count % 4 + 1);
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
        pid = fork();
        if (pid == 0)
        {
            printf("Auxiliar %d criado\n", i);
            while (true)
            {
                mensagem msg;
                msg.tipo = i + 1;
                if (msgrcv(msg_id, &msg, sizeof(msg.msg), i + 1, IPC_NOWAIT) == -1)
                {
                    printf("Auxiliar %d terminou\n", i);
                    exit(1);
                }
                int processVel = (int)msg.msg[0];
                int pidProg = fork();
                if (pidProg == 0)
                {
                    // switch
                    switch (processVel)
                    {
                    case lento:
                        printf("Auxiliar %d - PID [%d]: executando lento\n", i, getpid());
                        execl("lento", "lento", (char *)NULL);
                        break;
                    case normal:
                        printf("Auxiliar %d - PID [%d]: executando normal\n", i, getpid());
                        execl("normal", "normal", (char *)NULL);
                        break;
                    case rapido:
                        printf("Auxiliar %d - PID [%d]: executando rapido\n", i, getpid());
                        execl("rapido", "rapido", (char *)NULL);
                        break;
                    }
                }
                wait(&status);
            }
        }
    }

    for (int i = 0; i < 4; i++)
    {
        // espera que todos os auxiliares terminem
        wait(&status);
    }

    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds * 1e-6;

    printf("Turnaround time: %.3f segundos.\n", elapsed);
    exit(0);
}