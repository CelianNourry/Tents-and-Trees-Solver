#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define HERBE -1 // Espace où il est impossible d'avoir une tente
#define VIDE 0 // Espace où il est possible d'avoir une tente
#define TENTE 1
#define ARBRE 2 

#define LIGNES 8
#define COLONNES 8

typedef struct {
    int **grille;
    int *tentesParLigne; // Nombre de tentes qui sont requise sur chaque ligne
    int *tentesParColonne; // Nombre de tentes qui sont requise sur chaque colonne

    int *tentesParLigneOriginal; // Nombre de tentes qui sont requise sur chaque ligne
    int *tentesParColonneOriginal; // Nombre de tentes qui sont requise sur chaque colonne
} Plateau;

int lire_fichier(const char *nom_fichier, Plateau *p);
Plateau *creation_plateau(void);
void free_plateau(Plateau *p);
void afficher_plateau(Plateau *p);
int resolution (Plateau *p);
int verif_solution(Plateau *p);
int backtrack(Plateau *p);
int backtrack_brut(Plateau *p);
int main(void);
