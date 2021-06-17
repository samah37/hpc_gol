#pragma once

/* Univers du jeu game of life */
/**
 * cette structure contient le width, le height et deux tableaux de taille width * height
 * 
 * on a decider d'utiliser deux grid au lieu de 1 pour eviter les permutations a la fin de l'iteration.
 * 
 * par exemple, pour la premiere iteration on utilise les donnees du "leftGrid" pour modifier "rightGrid"
 * et dans la 2eme iteration on utilise les donnees du "rightGrid" pour modifier "leftGrid" et ainsi de suite.
 **/
typedef struct {
    unsigned int width;
    unsigned int height;

    unsigned char *leftGrid;
    unsigned char *rightGrid;
} Universe;