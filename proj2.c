/*IOS projekt2
 * synchronization problem: The Faneuil Hall Problem
 *
 * @Author: Norbert Pocs (xpocsn00)
 * @date: 21.04.2020
 */

#include "proj2.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h> //for shared memory (mmap)
#include <sys/types.h> //these 2 for fork
#include <unistd.h>

int main(int argc, char *argv[]){
    //parsing arguments
    if(argc != 6){
        //error wrong number of parameters
        fprintf(stderr, "Wrong number of parameters given.\n");
        exit(-1);
    }

    //program parameters
    int pi;
    int ig;
    int jg;
    int it;
    int jt;

    //save program arguments into variables
    if(sscanf(argv[1], "%d", &pi) == EOF || (pi < 1)){
        fprintf(stderr, "argument PI is not correct\n");
        exit(-1);
    }
    if(sscanf(argv[2], "%d", &ig) == EOF || (ig < 0 || ig > 2000)){
        fprintf(stderr, "argument IG is not correct\n");
        exit(-1);
    }
    if(sscanf(argv[3], "%d", &jg) == EOF || (jg < 0 || jg > 2000)){
        fprintf(stderr, "argument JG is not correct\n");
        exit(-1);
    }
    if(sscanf(argv[4], "%d", &it) == EOF || (it < 0 || it > 2000)){
        fprintf(stderr, "argument IT is not correct\n");
        exit(-1);
    }
    if(sscanf(argv[5], "%d", &jt) == EOF || (jt < 0 || jt > 2000)){
        fprintf(stderr, "argument JT is not correct\n");
        exit(-1);
    }

    //shared variables
    long *a = mmap(NULL, sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *i = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *ne = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *nc = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);

    //generating processes
    if(fork() == 0){
        //child - judge
        doing_judging();
    }else{
        //parent
        if(fork() == 0){
            //child - IMM generator
            printf("ig = %d\n", ig);
        }else{
            //parent
        }
    }

    //wait for processes to finish
    wait(NULL);
    wait(NULL);

    //freeing shared variables
    munmap(a, sizeof(long));
    munmap(i, sizeof(int));
    munmap(ne, sizeof(int));
    munmap(nc, sizeof(int));

    return 0;
}

void doing_judging(){
    printf("judging\n");
}
