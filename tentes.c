#include "lecture_plan.h"
#include "tentes.h"

//Créer le plateau
int **creation_plateau(int x, int y) { 
    //Allocation de mémoire pour toutes les cases du plateau 
    int **Plateau = (int **)malloc(x * sizeof(int *));
    if (Plateau == NULL) return NULL;
    for (int i = 0; i < x; i++) Plateau[i] = (int *)malloc(y * sizeof(int));

    //Remplis le plateau d'espace vide
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            Plateau[i][j] = VIDE; // Zone vide
        }
    }
    return Plateau;
}

void free_plateau(int **Plateau, int x) {
    for (int i = 0; i < x; i++) free(Plateau[i]);
    free(Plateau);
}

void afficher_plateau(int **Plateau, int x, int y) {
    if (Plateau == NULL) {
        printf("Erreur : le plateau est NULL.\n");
        return;
    }
    printf(" _ _ _ _ _ _ _ _\n");
    for (int i = 0; i < x; i++) {
        printf("|");
        for (int j = 0; j < y; j++) {
            if (Plateau[i][j] == VIDE) printf(" ");
            else if (Plateau[i][j] == ARBRE) printf("A");
            else if (Plateau[i][j] == HERBE) printf("X");
            else if (Plateau[i][j] == TENTE) printf("T");
            printf("|");
        }
        printf("\n|_|_|_|_|_|_|_|_|\n");
    }
    printf("\n");
}

int main(void) {
    int **plateau = creation_plateau(LIGNE, COLONNE);
    int lignes[LIGNE] = {0};
    int colonnes[COLONNE] = {0};

    if (!lire_fichier("plan.pln", lignes, colonnes, plateau, ARBRE)) {
        free_plateau(plateau, LIGNE);
        return 1;
    }

    afficher_plateau(plateau, LIGNE, COLONNE);

    free_plateau(plateau, LIGNE);
    return 0;
}
