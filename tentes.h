#include "lecture_plan.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HERBE -1 //Espace où il est impossible d'avoir une tente
#define VIDE 0 //Espace où il est possible d'avoir une tente
#define TENTE 1
#define ARBRE 2

#define LIGNE 8
#define COLONNE 8

int **creation_plateau(int x, int y);
void free_plateau(int **Plateau, int x);
void afficher_plateau(int **Plateau, int *nbArbreLignes, int *nbArbreColonnes, int x, int y);
void ajouter_vide(int **Plateau);
int **remplir_autour(int **Plateau, int x, int y);
bool peut_placer_tente(int **Plateau, int x, int y, int *nbTentesLignes, int *nbTentesColonnes);
bool backtrack(int **Plateau, int *nbTentesLignes, int *nbTentesColonnes, int x, int y);
int **resolution(int **Plateau, int *nbArbreLignes, int *nbArbreColonnes);
int main(void);
