#include <stdio.h>
#include <time.h>

int main(void){

    clock_t t;
    t = clock();

    for(long i = 0 ; i < 1000000000; i ++);


    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;

    printf("Time measured: %.3f seconds.\n", time_taken);


return 0;
};