#include "performance.h"
#include "game.h"

int nb_threads = 1;

void iterationSequentiel(Universe *universe, unsigned int nb_iter){
	int t_id , start , finish ;
	//le début et la fin de chaque thread
	start = 0;
	finish = universe->height;

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
	}
}

int main(int argc, char** argv) {
    
    if (argc < 3) {
        printf("usage: %s {width} {height}\n", argv[0]);
        exit(1);
    }

    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

	char method[12];
    sprintf(method, "%s", "sequentiel");

    FILE* file = initTestLog(method);

    Universe* uni = createUniverse(width, height);
    randomizeUniverse(uni);

    testPerformance(uni, iterationSequentiel, file);

    if (file != NULL)
        fclose(file);

    freeUniverse(uni);
}