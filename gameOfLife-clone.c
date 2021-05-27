#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_ALLOC_BYTES 1024*128

typedef struct {
    unsigned int width;
    unsigned int height;

    unsigned char *leftGrid;
    unsigned char *rightGrid;
} Universe;

// function to get the right index in the table for each element
int getIndex(Universe* universe, unsigned int row, unsigned int col) {
    return row * universe->width + col;
}

Universe* createUniverse(unsigned int width, unsigned int height) {

    // checking for the maximum allocation size
    size_t taille = sizeof(unsigned char)*width*height + sizeof(Universe);
    if (taille >= MAX_ALLOC_BYTES) {
        fprintf(stderr, "trying to allocate %d bytes which is more than the maximum allocation size, the program might crash on some machines.\n", taille);
    }


    Universe* uni = (Universe*) calloc(1, sizeof(Universe));

    uni->width = width;
    uni->height = height;

    // now we create the list of all elements as a long array
    uni->leftGrid = calloc(width*height, sizeof(unsigned char));
    memset(uni->leftGrid, 0, sizeof(unsigned char) * width * height);

    uni->rightGrid = calloc(width*height, sizeof(unsigned char));
    memset(uni->rightGrid, 0, sizeof(unsigned char) * width * height);

    return uni;
}

// generate a universe with random values between 0 and 1
void randomizeUniverse(Universe* uni) {
    int i, j;

    // and now we fill the grid with data
    srand(time(NULL));
    for (i=0; i < uni->height; i++) {
        for (j = 0; j < uni->width; j++) {
            uni->leftGrid[getIndex(uni, i, j)] = rand() % 2;
        }
    }
}

void afficherUniverse(Universe* universe) {
    int i ,j;
    for (i = 0; i < universe->height; i++){
        for (j = 0; j < universe->width; j++)
            printf("%d ", universe->leftGrid[getIndex(universe, i, j)]);
        printf("\n");
    }
}

void freeUniverse(Universe* uni) {
    free(uni->leftGrid);
    free(uni->rightGrid);

    free(uni);
}

unsigned char nbNeighborsAlive(Universe* uni, unsigned int x, unsigned int y, int iteration) {

    unsigned char count = 0;
    int rows[] = {uni->height - 1, 0, 1};
    int cols[] = {uni->width - 1, 0, 1};
    int i, j;

    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            if (rows[i] == 0 && cols[j] == 0)
                continue;
            
            char row = (x + rows[i]) % uni->height;
            char col = (y + cols[j]) % uni->width;

            if (iteration % 2 == 0) {
                count += uni->leftGrid[getIndex(uni, row, col)];
            } else {
                count += uni->rightGrid[getIndex(uni, row, col)];
            }
        }
    }

    return count;
}

unsigned char updateValue(Universe* universe, unsigned int x, unsigned int y, int iteration) {
    unsigned char currentVal;
    if (iteration % 2 == 0) {
        currentVal = universe->leftGrid[getIndex(universe, x, y)];
    } else {
        currentVal = universe->rightGrid[getIndex(universe, x, y)];
    }
    unsigned char neighbors = nbNeighborsAlive(universe, x, y, iteration);

    if (currentVal == 1 && (neighbors == 2 || neighbors == 3)) return 1; // survival rule 1

    if (currentVal == 0 && neighbors == 3) return 1;  // survival rule 2

    return 0; // everything else dies

}

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

FILE* initTestLog(char method[]) {
    FILE* file = fopen(method, "w+");

    fprintf(file, "La solution %s:\n", method);
    fprintf(file, "time (s * 10^6)\titerations\n");

    return file;
}

void writeLog(FILE* log, long time_elapsed, unsigned int nb_iterations) {
    char time[16];
    sprintf(time, "%d", time_elapsed);

    int count = strlen(time), i;
    for (i = count; i < 15; i++) {
        time[i] = ' ';
    }
    time[15] = '\0';

    fprintf(log, "%s\t%d\n", time, nb_iterations);

}

void removeFile(char method[]) {
    remove(method);
}


void testPerformance(Universe* uni, void (*iteration)(Universe*, unsigned int), FILE* log) {

    struct timeval t_start, t_end;
    int i;

    

    unsigned int nb_iterations[] = {100, 500, 1000};
    int taille = 3;

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
        testPerformance(uni, iterationOpenMP);
    else if (!strcmp(method, "openmpi"))
        testPerformance(uni, iterationOpenMPI);
    else if (!strcmp(method, "hybrid"))
        testPerformance(uni, iterationHybrid);*/
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