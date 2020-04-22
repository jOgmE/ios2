/*IOS projekt2
 * synchronization problem: The Faneuil Hall Problem
 *
 * @Author: Norbert Pocs (xpocsn00)
 * @date: 21.04.2020
 */

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h> //sem
#include <sys/mman.h> //for shared memory (mmap)
#include <sys/types.h> //fork
#include <unistd.h> //fork
#include <sys/wait.h> //wait
#include "proj2.h"


void enter_2_judge(int jt, sh_var *shared_vars, semaphores *sems){
    sem_wait(sems->no_judge);
    printf("no judge\n");
    sem_wait(sems->sh_mutex);
    printf("Mutex\n");
    //write arriving to output
    fprintf(shared_vars->f, "%ld:  JUDGE  enters:  %d:  %d:  %d\n", *(shared_vars->a), \
            *(shared_vars->ne), *(shared_vars->nc), *(shared_vars->nb));
    (*(shared_vars->a))++;
    *(shared_vars->judge) = 1;
    
    printf("j checking signed\n");
    //check signed
    if(*(shared_vars->ne) > *(shared_vars->nc)){
        fprintf(shared_vars->f, "%ld:  JUDGE  waits for imm:  %d:  %d:  %d\n", \
                *(shared_vars->a), *(shared_vars->ne), *(shared_vars->nc), \
                *(shared_vars->nb));
        (*(shared_vars->a))++;
        //sem
        sem_post(sems->sh_mutex);
        sem_wait(sems->all_signed_in);
    }

    printf("j confirms\n");
    //--confirmation--
    fprintf(shared_vars->f, "%ld:  JUDGE  starts confirmation:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    (*(shared_vars->a))++;
    if(jt > 0){
        usleep(rand() % (jt+1));
    }

    //setting counters
    *(shared_vars->nc) = 0;
    *(shared_vars->ne) = 0;

    //printing output
    fprintf(shared_vars->f, "%ld:  JUDGE  ends confirmation:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    (*(shared_vars->a))++;
    //sem
    sem_post(sems->confirmed);

    printf("j ended confirmation\n");
    //--leave--
    if(jt > 0){
        usleep(rand() % (jt+1));
    }
    fprintf(shared_vars->f, "%ld:  JUDGE  leaves:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    (*(shared_vars->a))++;

    *(shared_vars->judge) = 0;

    //sem
    sem_post(sems->sh_mutex);
    sem_post(sems->no_judge);
}

void imm(int it, sh_var *shared_vars, semaphores *sems){
    //writing output
    sem_wait(sems->sh_mutex);
    //increment immigrants counter
    (*(shared_vars->i))++;
    fprintf(shared_vars->f, "%ld:  IMM %d:  starts\n", *(shared_vars->a), *(shared_vars->i));
    (*(shared_vars->a))++;
    sem_post(sems->sh_mutex);

    //enter the room
    sem_wait(sems->no_judge);
    //entered
    (*(shared_vars->ne))++;
    (*(shared_vars->nb))++;
    fprintf(shared_vars->f, "%ld:  IMM %d:  enters:  %d:  %d:  %d\n", *(shared_vars->a), \
            *(shared_vars->i), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    printf("imm entered\n");
    sem_post(sems->no_judge);
    //checking in
    sem_wait(sems->sh_mutex);
    (*(shared_vars->nc))++;
    fprintf(shared_vars->f, "%ld:  IMM %d:  checks:  %d:  %d:  %d\n", *(shared_vars->a), \
            *(shared_vars->i), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    printf("imm checked in\n");
    if(*(shared_vars->judge) == 1 && *(shared_vars->ne) == *(shared_vars->nc)){
        sem_post(sems->all_signed_in);
    }
    sem_post(sems->sh_mutex);
    //waiting for confirmation
    sem_wait(sems->confirmed);
    //certificate
    fprintf(shared_vars->f, "%ld:  IMM %d:  wants certificate:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->i), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    printf("imm wants ce\n");
    if(it > 0){
        usleep(rand() % it);
    }
    fprintf(shared_vars->f, "%ld:  IMM %d:  got certificate:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->i), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    printf("imm got ce\n");
    //leaving
    sem_wait(sems->no_judge);
    (*(shared_vars->nb))--;
    fprintf(shared_vars->f, "%ld:  IMM %d:  leaves:  %d:  %d:  %d\n", \
            *(shared_vars->a), *(shared_vars->i), *(shared_vars->ne), *(shared_vars->nc), \
            *(shared_vars->nb));
    printf("imm leaves\n");
    (*(shared_vars->finished))++;
    sem_post(sems->no_judge);

    //kill
    exit(0);
}

void imm_generator(int pi, int ig, int it, sh_var *shared_vars, semaphores *sems){
    for(int i=0; i<pi; i++){
        if(ig > 0){
            usleep(rand() % (ig+1));
        }
        if(fork() == 0){
            printf("new imm created\n");
            imm(it, shared_vars, sems);
        }
    }
}

int main(int argc, char *argv[]){
    //parsing arguments
    if(argc != 6){
        //error wrong number of parameters
        fprintf(stderr, "Wrong number of parameters given.\n");
        exit(1);
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
        exit(1);
    }
    if(sscanf(argv[2], "%d", &ig) == EOF || (ig < 0 || ig > 2000)){
        fprintf(stderr, "argument IG is not correct\n");
        exit(1);
    }
    if(sscanf(argv[3], "%d", &jg) == EOF || (jg < 0 || jg > 2000)){
        fprintf(stderr, "argument JG is not correct\n");
        exit(1);
    }
    if(sscanf(argv[4], "%d", &it) == EOF || (it < 0 || it > 2000)){
        fprintf(stderr, "argument IT is not correct\n");
        exit(1);
    }
    if(sscanf(argv[5], "%d", &jt) == EOF || (jt < 0 || jt > 2000)){
        fprintf(stderr, "argument JT is not correct\n");
        exit(1);
    }

    //creating/cleaning the output file
    FILE *f = mmap(NULL, sizeof(FILE), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    f = fopen("proj2.out", "w");
    if(!f){
        fprintf(stderr, "Can't write output file\n");
        exit(1);
    }

    //shared variables
    long *a = mmap(NULL, sizeof(long), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *i = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *ne = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *nc = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *nb = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *finished = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    int *judge = mmap(NULL, sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    *a = 1;
    *i = 0;
    *ne = 0;
    *nc = 0;
    *nb = 0;
    *judge = 0;
    *finished = 0; //how many processess were finished the process of getting a CE

    sh_var shared_vars = {.a=a, .i=i, .ne=ne, .nc=nc, .nb=nb, .finished=finished, .f=f, \
                            .judge=judge};

    //semaphores
    sem_t *sh_mutex = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, \
            MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    sem_t *no_judge = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, \
            MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    sem_t *confirmed = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, \
            MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    sem_t *all_signed_in = mmap(NULL, sizeof(sem_t), PROT_READ|PROT_WRITE, \
            MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    //initializing them
    sem_init(sh_mutex, 1, 1);
    sem_init(no_judge, 1, 1);
    sem_init(confirmed, 1, 0);
    sem_init(all_signed_in, 1, 0);
    //saving them into struct
    semaphores sems = {.sh_mutex=sh_mutex, .no_judge=no_judge, .confirmed=confirmed, \
        .all_signed_in= all_signed_in};

    //generating processes
    pid_t pid;
    if((pid = fork()) == 0){
        printf("this is judge\n");
        //child - judge
        while(*finished != pi){
            if(jg > 0){
                usleep(rand() % (jg+1));
            }
            printf("judge enters\n");
            enter_2_judge(jt, &shared_vars, &sems);
            printf("judge leaves\n");
        }
        sem_wait(sems.sh_mutex);
        fprintf(shared_vars.f, "%ld:  JUDGE:  finishes\n", *(shared_vars.a));
        (*(shared_vars.a))++;
        sem_post(sems.sh_mutex);
        //kill judge
        exit(0);
    }else if(pid == -1){
        fprintf(stderr, "forking failed\n");
        
        //freeing shared variables
        munmap(a, sizeof(long));
        munmap(i, sizeof(int));
        munmap(ne, sizeof(int));
        munmap(nc, sizeof(int));
        munmap(nb, sizeof(int));
        munmap(finished, sizeof(int));
        munmap(judge, sizeof(int));
        sem_destroy(sems.sh_mutex);
        munmap(sems.sh_mutex, sizeof(sem_t));
        sem_destroy(sems.no_judge);
        munmap(sems.no_judge, sizeof(sem_t));
        sem_destroy(sems.confirmed);
        munmap(sems.confirmed, sizeof(sem_t));
        sem_destroy(sems.all_signed_in);
        munmap(sems.all_signed_in, sizeof(sem_t));
        fclose(f);
        munmap(f, sizeof(FILE));
        exit(1);
    }else{
        //parent
        if((pid = fork()) == 0){
            //child - IMM generator
            imm_generator(pi, ig, it, &shared_vars, &sems);
            for(int i=0; i<pi;i++){
                wait(NULL);
            }
            //killing generator
            exit(0);
        }else if(pid == -1){
            fprintf(stderr, "forking failed\n");
            
            //freeing shared variables
            munmap(a, sizeof(long));
            munmap(i, sizeof(int));
            munmap(ne, sizeof(int));
            munmap(nc, sizeof(int));
            munmap(nb, sizeof(int));
            munmap(finished, sizeof(int));
            munmap(judge, sizeof(int));
            sem_destroy(sems.sh_mutex);
            munmap(sems.sh_mutex, sizeof(sem_t));
            sem_destroy(sems.no_judge);
            munmap(sems.no_judge, sizeof(sem_t));
            sem_destroy(sems.confirmed);
            munmap(sems.confirmed, sizeof(sem_t));
            sem_destroy(sems.all_signed_in);
            munmap(sems.all_signed_in, sizeof(sem_t));
            fclose(f);
            munmap(f, sizeof(FILE));
            exit(1);
        }else{
            //parent

            //wait for processes to finish
            wait(NULL);
            wait(NULL);
        }
    }

    //freeing shared variables
    munmap(a, sizeof(long));
    munmap(i, sizeof(int));
    munmap(ne, sizeof(int));
    munmap(nc, sizeof(int));
    munmap(nb, sizeof(int));
    munmap(finished, sizeof(int));
    munmap(judge, sizeof(int));
    sem_destroy(sems.sh_mutex);
    munmap(sems.sh_mutex, sizeof(sem_t));
    sem_destroy(sems.no_judge);
    munmap(sems.no_judge, sizeof(sem_t));
    sem_destroy(sems.confirmed);
    munmap(sems.confirmed, sizeof(sem_t));
    sem_destroy(sems.all_signed_in);
    munmap(sems.all_signed_in, sizeof(sem_t));
    fclose(f);
    munmap(f, sizeof(FILE));

    return 0;
}
