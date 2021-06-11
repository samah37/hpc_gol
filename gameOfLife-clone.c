#include "performance.h"
#include "game.h"
#include <pthread.h>
#include <omp.h>
#include <mpi.h>

int nb_threads = 1;
void iterationSequentiel(Universe *universe, unsigned int nb_iter) {
    int k, i, j;
    
    for (k = 0; k < nb_iter; k++) {
        for (i = 0; i < universe->height; i++) {
            for (j = 0; j < universe->width; j++) {
                int index = getIndex(universe, i, j);
                
                if (k % 2 == 0) {
                    universe->rightGrid[index] = updateValue(universe, i, j, k);
                } else {
                    universe->leftGrid[index] = updateValue(universe, i, j, k);
                }
            }
        }
    }
}

typedef struct {
    Universe* uni;
    int thread_num;
    unsigned int nb_iter;
    pthread_barrier_t* barr;
} pthread_param;

void *subIterationPthread(void* data) {
    pthread_param* param = (pthread_param*)data;

    int start_height = param->thread_num * param->uni->height / nb_threads;
    int end_height = (param->thread_num + 1) * param->uni->height / nb_threads;

    // verifie start and end heights
    //printf("thread %d:\n\tstart: %d,\n\tend: %d,\n\n", param->thread_num, start_height, end_height);

    int i, j, k;
    for (k = 0; k < param->nb_iter; k++) {
        for (i = start_height; i < end_height; i++) {
            for (j = 0; j < param->uni->width; j++) {
                int index = getIndex(param->uni, i, j);

                if (k % 2 == 0)
                    param->uni->rightGrid[index] = updateValue(param->uni, i, j, k);
                else
                    param->uni->leftGrid[index] = updateValue(param->uni, i, j, k);
            }
        }

        int bn = pthread_barrier_wait(param->barr);
        if(bn != 0 && bn != PTHREAD_BARRIER_SERIAL_THREAD){
            printf("Could not wait on barrier\n");
            exit(-1);
        }
        
    }
    pthread_exit(NULL);
}

void iterationPthread(Universe *universe, unsigned int nb_iter) {
    // TODO: try using the same threads for all iterations
    pthread_t threads[nb_threads];
    pthread_param* params = (pthread_param*) calloc(nb_threads, sizeof(pthread_param));

    pthread_barrier_t barrier;

    if(pthread_barrier_init(&barrier, NULL, nb_threads)){
        printf("Could not create a barrier\n");
        exit(1);
    }

    int i;
    for (i = 0; i < nb_threads; i++) {
        params[i].thread_num = i;
        params[i].uni = universe;
        params[i].nb_iter = nb_iter;
        params[i].barr = &barrier;
        pthread_create(threads + i, NULL, subIterationPthread, &params[i]);
    }

    for (i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_barrier_destroy(&barrier);
    free(params);
}

void iterationOpenMPI(Universe *uni, unsigned int nb_iter) {
    int num, id;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int start_height = id * uni->height / num;
    int end_height = (id + 1) * uni->height / num;

    // omp_set_num_threads(4);

    int i, j, k;
    double start, end;

    if (id == 0)
        start = MPI_Wtime();
    
    for (k = 0; k < nb_iter; k++) {
        // #pragma omp parallel for private(i, j)
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
    if (id == 0) {
        end = MPI_Wtime();
        double elapsed = end - start;

        printf("time passed: %.5f", elapsed);
    }
    MPI_Finalize();
}

void iterationHybrid(Universe *uni, unsigned int nb_iter) {
    int num, id, provided;

    MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);

    if (provided != MPI_THREAD_MULTIPLE) {
        printf("provided: %d\n", provided);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &id);

    int start_height = id * uni->height / num;
    int end_height = (id + 1) * uni->height / num;

    omp_set_num_threads(nb_threads);

    int i, j, k;
    double start, end;

    if (id == 0)
        start = MPI_Wtime();
    
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
    if (id == 0) {
        end = MPI_Wtime();
        double elapsed = end - start;

        printf("time passed: %.5f", elapsed);
    }
    MPI_Finalize();
}

int main(int argc, char** argv) {
    char method[20] = "sequentiel";

    if (argc < 3) {
        printf("usage: %s {width} {height} [method] [nb_threads]\n", argv[0]);
        exit(1);
    }

    if (argc >= 4)
        strncpy(method, argv[3], 19);

    if (argc == 5)
        nb_threads = atoi(argv[4]);

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    FILE* file = initTestLog(method);

    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    if (!strcmp(method, "sequentiel"))
        testPerformance(uni, iterationSequentiel, file);
    else if (!strcmp(method, "pthread"))
        testPerformance(uni, iterationPthread, file);
    /*else if (!strcmp(method, "openmp"))
        testPerformance(uni, iterationOpenMP);*/
    else if (!strcmp(method, "openmpi"))
        iterationOpenMPI(uni, 2000);
    else if (!strcmp(method, "hybrid"))
        testPerformance(uni, iterationHybrid, file);
    else {
        printf("unknown method: %s\n", method);
        fclose(file);
        file = NULL;
        remove(method);
    }

    if (file != NULL)
        fclose(file);
    freeUniverse(uni);
}