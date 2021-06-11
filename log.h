#pragma once

#include <stdio.h>
#include <string.h>

FILE* initTestLog(char method[]) {
    FILE* file = fopen(method, "w+");

    fprintf(file, "La solution %s:\n", method);
    fprintf(file, "time (s * 10^6)\titerations\n");

    return file;
}

void writeLog(FILE* log, double time_elapsed, unsigned int nb_iterations) {
    char time[16];
    sprintf(time, "%.6f", time_elapsed);

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