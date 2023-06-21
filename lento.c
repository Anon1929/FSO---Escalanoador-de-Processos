#include <stdio.h>
#include <sys/time.h>
 #include <unistd.h>

int main() {

    struct timeval begin, end;
    gettimeofday(&begin, 0);


    //printf("executando lento\n");
    for(long j=0;j<=18000000000;j++);


    // timer end
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double elapsed = seconds + microseconds*1e-6;

    int pid = getpid();
    
    printf("Processo PID [%d] terminou a execução em %.3f segundos.\n", pid, elapsed);
}
