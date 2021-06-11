#include "performance.h"
#include "game.h"
#include <pthread.h>

int nb_threads = 1;

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

    char method[12];
    sprintf(method, "%s-%d", "pthread", nb_threads);

    FILE* file = initTestLog(method);

    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    testPerformance(uni, iterationPthread, file);

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}