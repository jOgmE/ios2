/*IOS projekt 2 header file
 *
 * @author: Norbert Pocs (xpocsn00)
 * @date: 21.04.2020
 */

#ifndef __PROJ_2__
#define __PROJ_2__

#include <semaphore.h>
#include <stdio.h>

void doing_judging();

typedef struct sh_var{
    long *a;
    int *i;
    int *ne;
    int *nc;
    int *finished;
    FILE *f;
} sh_var;

typedef struct semaphores{
    sem_t *sh_mutex;
    sem_t *no_judge;
    sem_t *confirmed;
    sem_t *all_signed_in;
} semaphores;

#endif /*__PROJ_2__*/
