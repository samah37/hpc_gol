#pragma once

#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include "types.h"

#define MAX_ALLOC_BYTES 1024*128

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
    uni->leftGrid = (unsigned char*)calloc(width*height, sizeof(unsigned char));
    memset(uni->leftGrid, 0, sizeof(unsigned char) * width * height);

    uni->rightGrid = (unsigned char*)calloc(width*height, sizeof(unsigned char));
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
            
            unsigned int row = (x + rows[i]) % uni->height;
            unsigned int col = (y + cols[j]) % uni->width;

            if (row < 0)
                row += uni->height;
            if (col < 0) {
                col += uni->width;
                printf("%d, %d\n", row, col);
            }

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