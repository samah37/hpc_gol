int num, id;
#include <mpi.h>
#include "performance_mpi.h"
#include "game.h"


void iterationOpenMPI(Universe *uni, unsigned int nb_iter) {

    int start_height = id * uni->height / num;
    int end_height = (id + 1) * uni->height / num;

    int i, j, k;

    for (k = 0; k < nb_iter; k++) {
        int count = uni->width * uni->height * 2 + 2 * sizeof(unsigned int), rcount, displs;
        MPI_Allgatherv(uni, count, MPI_CHAR, uni, &rcount, &displs, MPI_CHAR, MPI_COMM_WORLD);
        printf("%d, %d\n", rcount, displs);
        for (i = start_height; i < end_height; i++) {
            for (j = 0; j < uni->width; j++) {
                int index = getIndex(uni, i, j);

                if (k % 2 == 0)
                    uni->rightGrid[index] = updateValue(uni, i, j, k);
                else
                    uni->leftGrid[index] = updateValue(uni, i, j, k);
            }
        }
        
    }
    
}

int main(int argc, char** argv) {
    
    if (argc < 3) {
        printf("usage: %s {width} {height}\n", argv[0]);
        exit(1);
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);


    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    char method[12];
    sprintf(method, "%s-%d", "openmpi", num);

    FILE* file = initTestLog(method);

    MPI_Bcast(uni, 2 * width * height + 2 * sizeof(unsigned int), MPI_CHAR, 0, MPI_COMM_WORLD);

    testPerformanceMPI(uni, iterationOpenMPI, file);

    MPI_Finalize();

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}