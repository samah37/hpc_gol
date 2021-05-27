/*********************************************** Projet HPC 2CSIL 2020-2021 **************************************************/
/**************************** Membres de l'équipe: 
                                        -BOUDIS Madjid
                                        -DOUMI Athmane
                                        -OTSMANE Nabil
                                        -KANSAB Samah *********************************************************************/
                                        
                                        
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/********************* Constants **************************/
int gridSave[15][15];
//initial state
 int grid[15][15]=
{{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, 
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
int gridInit[15][15];

unsigned const int SIZE =15 ;
unsigned const int Iterrations[10] = {50, 100 , 120 , 800 , 25, 65, 110, 120 , 68, 120};
/***************************** Cette fonction génère la matrice initiale *******************************/
void FillInMatrix(){
    for (size_t y=0; y<SIZE; y++)
        for (size_t x=0; x<SIZE; x++)
            gridInit[y][x] = rand() % 2;
}

/****************************Cette fonction permet de cloner la meme matrice initiale pour chaque solution ******************/
void clone(){
    for (size_t y=0; y<SIZE; y++)
        for (size_t x=0; x<SIZE; x++)
            grid[y][x] = gridInit[y][x];
}

/**************Cette fonction retourne nombre de voisins vivants **************/
int ReturnNeighbors(int X, int Y){
    int N=0;
    if (X== SIZE) X= X-1;
    if (Y== SIZE) Y=Y-1;
    if (X== 0) X=X+1;
    if (Y==0) Y=Y+1;
    N= grid[Y+1][X-1] + grid[Y+1][X] + grid[Y+1][X+1] + grid[Y][X+1] + grid[Y][X-1] + grid[Y-1][X-1] + grid[Y-1][X] + grid[Y-1][X+1];
    return N;
}
/*****************TCette fonction retourne la nouvelle valeur d'une cellule[x][y]*************************/
int UpdatedCell(int x, int y, int currentVAl){
    int n = 0;
    if (currentVAl == 1){
        if ((ReturnNeighbors(x,y)==2) || (ReturnNeighbors(x,y)==3)) n=1;
        if ((ReturnNeighbors(x,y)>3) || (ReturnNeighbors(x,y)<2))  n=0;
    }
    else if (ReturnNeighbors(x,y)>= 3)  n=1;
    return n;
}
/************************************** Afficher une matrice***********************************************/
void display(){
    for (size_t y=0; y< SIZE;y++) {
        for (size_t x=0; x<SIZE; x++)
            printf("%d", grid[y][x]);
        printf("\n");
    }
    printf("\n");
}

/********************************** La solution séquentielle*************************************/
double SequentielSolution(int itr)
{
    clock_t start, end;
    double cpu_time_used;
    start = clock();
    
    for (size_t i=0; i< itr; i++) { 
        //printf("The iterration N: %d \n",i);

        for (size_t y=0; y<SIZE; y++)
            for (size_t x=0; x<SIZE; x++)
                gridSave[y][x] = UpdatedCell(x, y, grid[x][y]);


        for (size_t y=1; y< SIZE;y++) {
            for (size_t x=0; x<SIZE; x++) {
                grid[y][x] = gridSave[y][x];
                //printf("%d", grid[y][x]);
            }
            //printf("\n");
        }
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    return  cpu_time_used; 
} 


/*********************************Main**********************************************/
int main(){
    FILE* fichier = NULL;
    fichier = fopen("game.txt", "w+");
    
    if (fichier != NULL)
    {
        printf("Bienvenue dans le jeu de la vie \n");
        FillInMatrix();
        
        printf("La solution séquentielle: \n");
        fprintf(fichier, "La solution séquentielle: \n");
        fprintf(fichier, "seconds     iterrations \n");
        printf("-------------------------\n");
        clone();
        printf ("La matrice initiale est :\n");
        display();
        long global=0;
        for (size_t i=0; i<10; i++){
            global= global + SequentielSolution(Iterrations[i]);
            printf(" %f seconds pour %d itterations\n", SequentielSolution(Iterrations[i]), Iterrations[i]);
            fprintf(fichier, " %f   %d \n", SequentielSolution(Iterrations[i]), Iterrations[i]);
        }
        
        fclose(fichier);
        
    }
    else
    {
        
        printf("Impossible d'ouvrir le fichier game.txt");
    }

    getchar();
    return 0;
}
