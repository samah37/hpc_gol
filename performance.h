#pragma once

#include <stdio.h>
#include <sys/time.h>
#include "types.h"
#include "log.h"

void testPerformance(Universe* uni, void (*iteration)(Universe*, unsigned int), FILE* log) {

    unsigned int nb_iterations[] = {100, 500, 1000};
    int taille = sizeof(nb_iterations) / sizeof(unsigned int);

    struct timeval t_start, t_end;
    int i;

    for (i = 0; i < taille; i++) {
        printf("pour %d iterations: ", nb_iterations[i]);
        gettimeofday(&t_start, NULL); // ------start timer
        
        iteration(uni, nb_iterations[i]);
        
        gettimeofday(&t_end, NULL);  // ---------stop timer
    
        long time_elapsed = (t_end.tv_sec * 1e6 + t_end.tv_usec) - (t_start.tv_sec * 1e6 + t_start.tv_usec);
        double elapsed = time_elapsed / 1e6;
        
        printf("%.6f\n", elapsed);

        writeLog(log, elapsed, nb_iterations[i]);
    }
}