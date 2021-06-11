int num, id;
#include <omp.h>
#include <mpi.h>
#include "performance_mpi.h"
#include "game.h"

int nb_threads = 1;


void iterationHybrid(Universe *uni, unsigned int nb_iter) {

    int start_height = id * uni->height / num;
    int end_height = (id + 1) * uni->height / num;

    omp_set_num_threads(nb_threads);

    int i, j, k;
    
    for (k = 0; k < nb_iter; k++) {
        #pragma omp parallel for private(i, j)
        for (i = start_height; i < end_height; i++) {
            for (j = 0; j < uni->width; j++) {
                int index = getIndex(uni, i, j);

                if (k % 2 == 0)
                    uni->rightGrid[index] = updateValue(uni, i, j, k);
                else
                    uni->leftGrid[index] = updateValue(uni, i, j, k);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
}

int main(int argc, char** argv) {
    
    if (argc < 3) {
        printf("usage: %s {width} {height} [nb_threads]\n", argv[0]);
        exit(1);
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    if (argc == 4) {
        nb_threads = atoi(argv[3]);
    }


    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    int provided;

    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);

    if (provided != MPI_THREAD_MULTIPLE) {
        printf("provided: %d\n", provided);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    
    char method[12];
    sprintf(method, "%s-%d-%d", "hybrid", num, nb_threads);

    FILE* file = initTestLog(method);

    testPerformanceMPI(uni, iterationHybrid, file);

    MPI_Finalize();

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}