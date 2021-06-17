#pragma once

#include <stdio.h>
#include <sys/time.h>
#include "types.h"
#include "log.h"
#include "mpi.h"

void testPerformanceMPI(Universe* uni, void (*iteration)(Universe*, unsigned int), FILE* log) {

    unsigned int nb_iterations[] = {1000};
    int taille = sizeof(nb_iterations) / sizeof(unsigned int);

    double start, end;
    int i;

    for (i = 0; i < taille; i++) {
        if (id == 0) {
            printf("pour %d iterations: ", nb_iterations[i]);
            start = MPI_Wtime();
        }
        
        iteration(uni, nb_iterations[i]);
        
        if (id == 0) {
            end = MPI_Wtime();
            double elapsed = end - start;

            printf("%.6f\n", elapsed);
            writeLog(log, elapsed, nb_iterations[i]);
        }
    }

}