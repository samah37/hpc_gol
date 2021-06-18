int num, id;
#include <mpi.h>
#include "performance_mpi.h"
#include "game.h"
#include <unistd.h>


void iterationOpenMPI(Universe *uni, unsigned int nb_iter) {

    int start_height = id * uni->height / num;
    int end_height = (id + 1) * uni->height / num;

    int i, j, k;

    for (k = 0; k < nb_iter; k++) {
        for (i = start_height; i < end_height; i++) {
            for (j = 0; j < uni->width; j++) {
                int index = getIndex(uni, i, j);

                if (k % 2 == 0)
                    uni->rightGrid[index] = updateValue(uni, i, j, k);
                else
                    uni->leftGrid[index] = updateValue(uni, i, j, k);
            }
        }
        int count = uni->width * (uni->height/2), rcount, displs[] = { 0, getIndex(uni, uni->height/2, 0) };
        // if (k % 2 == 0) {
        //     MPI_Allgatherv(uni->rightGrid, count, MPI_CHAR, uni->rightGrid, &rcount, displs, MPI_INT, MPI_COMM_WORLD);
        // } else {
        //     MPI_Allgatherv(uni->leftGrid, count, MPI_INT, uni->leftGrid, &rcount, displs, MPI_INT, MPI_COMM_WORLD);
        // }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    
}

int main(int argc, char** argv) {
    int width, height;
    if (argc < 3) {
        printf("usage: %s {width} {height}\n", argv[0]);
        
        width = 512;
        height = 512;
    } else {
        width = atoi(argv[1]);
        height = atoi(argv[2]);
    }

    // int i = 0;
    // while (i == 0)
    //     sleep(5);


    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    char method[15] = {0};
    sprintf(method, "%s-%d", "openmpi", num);

    FILE* file = initTestLog(method);

    // MPI_Bcast(uni, sizeof(Universe), MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(uni->leftGrid, width * height, MPI_BYTE, 0, MPI_COMM_WORLD);
    MPI_Bcast(uni->rightGrid, width * height, MPI_BYTE, 0, MPI_COMM_WORLD);

    testPerformanceMPI(uni, iterationOpenMPI, file);

    MPI_Finalize();

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}