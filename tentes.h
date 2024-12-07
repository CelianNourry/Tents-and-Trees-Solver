#include "lecture_plan.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define VIDE 0
#define HERBE -1
#define ARBRE -2
#define TENTE 1

#define LIGNE 8
#define COLONNE 8

int **creation_plateau(int x, int y);
void free_plateau(int **Plateau, int x);
void afficher_plateau(int **Plateau, int x, int y);
int main(void);
