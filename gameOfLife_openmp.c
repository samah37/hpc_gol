#include "performance.h"
#include "game.h"
#include <omp.h>

int nb_threads = 1;

void iterationOpenMP(Universe *universe, unsigned int nb_iter){
	int t_id , start , finish ;
	#pragma omp parallel private(t_id,start,finish) num_threads(nb_threads)
	{
		t_id=omp_get_thread_num();
		//le début et la fin de chaque thread
		start = t_id * (universe->height / nb_threads);
		finish = start + (universe->height / nb_threads);

		for(int iter=0 ; iter<nb_iter;iter++){
			for(int i = start;i<finish;i++){
				for(int j=0 ;j<universe->width;j++){
					int index = getIndex(universe,i,j);
					if(iter%2 == 0)
						universe->rightGrid[index] = updateValue(universe,i,j,iter);
					else
						universe->leftGrid[index] = updateValue(universe,i,j,iter);
				}
			}
			#pragma omp barrier
		}
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

	char method[12];
    sprintf(method, "%s-%d", "openmp", nb_threads);

    FILE* file = initTestLog(method);

    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    testPerformance(uni, iterationOpenMP, file);

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}