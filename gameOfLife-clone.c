#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <malloc.h>
#include <pthread.h>

#define MAX_ALLOC_BYTES 1024*128

typedef struct {
    unsigned int width;
    unsigned int height;

    unsigned char *grid;
    unsigned char *newGrid;
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


    Universe* uni = (Universe*) malloc(sizeof(Universe));

    uni->width = width;
    uni->height = height;

    // now we create the list of all elements as a long array
    uni->grid = malloc(sizeof(unsigned char)*width*height);
    memset(uni->grid, 0, sizeof(unsigned char) * width * height);

    uni->newGrid = NULL;

    return uni;
}

// generate a universe with random values between 0 and 1
void randomizeUniverse(Universe* uni) {
    int i, j;

    // and now we fill the grid with data
    srand(time(NULL));
    for (i=0; i < uni->height; i++) {
        for (j = 0; j < uni->width; j++) {
            uni->grid[getIndex(uni, i, j)] = rand() % 2;
        }
    }
}

void afficherUniverse(Universe* universe) {
    int i ,j;
    for (i = 0; i < universe->height; i++){
        for (j = 0; j < universe->width; j++)
            printf("%d ", universe->grid[getIndex(universe, i, j)]);
        printf("\n");
    }
}

void freeUniverse(Universe* uni) {
    free(uni->grid);

    if (uni->newGrid != NULL) {
        free(uni->newGrid);
    }

    free(uni);
}

unsigned char nbNeighborsAlive(Universe* uni, unsigned int x, unsigned int y) {

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

            count += uni->grid[getIndex(uni, row, col)];
        }
    }

    return count;
}

unsigned char updateValue(Universe* universe, unsigned int x, unsigned int y) {
    unsigned char currentVal = universe->grid[getIndex(universe, x, y)],
                  neighbors = nbNeighborsAlive(universe, x, y);

    if (currentVal == 1 && (neighbors == 2 || neighbors == 3)) return 1; // survival rule 1

    if (currentVal == 0 && neighbors == 3) return 1;  // survival rule 2

    return 0; // everything else dies

}

void abstractIteration(Universe* universe, void (*iteration)(Universe *)) {
    // create new grid for use in the iteration
    universe->newGrid = (unsigned char*)malloc(sizeof(unsigned char) * universe->width * universe->height);

    if (universe->newGrid == NULL) {
        printf("\ncouldn't create a grid with size: %d\n", sizeof(unsigned char) * universe->width * universe->height);
    }

    iteration(universe);    

    // exchange grids
    free(universe->grid);
    universe->grid = universe->newGrid;
    universe->newGrid = NULL;
}

int nb_threads = 1;
void iterationSequentiel(Universe *universe) {
    int i, j;
    
    for (i = 0; i < universe->height; i++) {
        for (j = 0; j < universe->width; j++) {
            int index = getIndex(universe, i, j);
            
            universe->newGrid[index] = updateValue(universe, i, j);
        }
    }
}

typedef struct {
    Universe* uni;
    int thread_num;
} pthread_param;

void *subIterationPthread(void* data) {
    pthread_param* param = (pthread_param*)data;

    int start_height = param->thread_num * param->uni->height / nb_threads;
    int end_height = (param->thread_num + 1) * param->uni->height / nb_threads;

    // verifie start and end heights
    //printf("thread %d:\n\tstart: %d,\n\tend: %d,\n\n", param->thread_num, start_height, end_height);

    int i, j;
    for (i = start_height; i < end_height; i++) {
        for (j = 0; j < param->uni->width; j++) {
            int index = getIndex(param->uni, i, j);

            param->uni->newGrid[index] = updateValue(param->uni, i, j);
        }
    }
    pthread_exit(NULL);
}

void iterationPthread(Universe *universe) {
    // TODO: try using the same threads for all iterations
    pthread_t threads[nb_threads];
    pthread_param* params = (pthread_param*) malloc(nb_threads * sizeof(pthread_param));

    int i;
    for (i = 0; i < nb_threads; i++) {
        params[i].thread_num = i;
        params[i].uni = universe;
        pthread_create(threads + i, NULL, subIterationPthread, &params[i]);
    }

    for (i = 0; i < nb_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    free(params);
}

void runIterations(Universe* uni, unsigned int nb_iter, void (*iteration)(Universe *)) {

    clock_t start, end;
    double time_elapsed;
    int i;

    printf("pour %d iterations: ", nb_iter);
    start = clock();

    for (i = 0; i < nb_iter; i++)
        abstractIteration(uni, iteration);
    
    end = clock();
    time_elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
    
    printf("%0.5lf\n", time_elapsed);
}

void testPerformance(Universe* uni, void (*iteration)(Universe*)) {

    int nb_iterations[] = {100, 500, 1000, 2000};
    int taille = 4, i;

    for (i = 0; i < taille; i++)
        runIterations(uni, nb_iterations[i], iteration);

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

    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    if (!strcmp(method, "sequentiel"))
        testPerformance(uni, iterationSequentiel);
    else if (!strcmp(method, "pthread"))
        testPerformance(uni, iterationPthread);
    /*else if (!strcmp(method, "openmp"))
        testPerformance(uni, iterationOpenMP);
    else if (!strcmp(method, "openmpi"))
        testPerformance(uni, iterationOpenMPI);
    else if (!strcmp(method, "hybrid"))
        testPerformance(uni, iterationHybrid);*/
    else {
        printf("unknown method: %s\n", method);
    }

    freeUniverse(uni);
}