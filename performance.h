#pragma once

#include <stdio.h>
#include <sys/time.h>
#include "types.h"
#include "log.h"

void testPerformance(Universe* uni, void (*iteration)(Universe*, unsigned int), FILE* log) {

    struct timeval t_start, t_end;
    int i;

    unsigned int nb_iterations[] = {2000};
    int taille = 1;

    for (i = 0; i < taille; i++) {
        printf("pour %d iterations: ", nb_iterations[i]);
        gettimeofday(&t_start, NULL); // ------start timer
        
        iteration(uni, nb_iterations[i]);
        
        gettimeofday(&t_end, NULL);  // ---------stop timer
    
        long time_elapsed = (t_end.tv_sec * 1e6 + t_end.tv_usec) - (t_start.tv_sec * 1e6 + t_start.tv_usec);
        
        printf("%ld\n", time_elapsed);

        writeLog(log, time_elapsed, nb_iterations[i]);
    }
}