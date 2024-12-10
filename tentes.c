#include "lecture_plan.h"
#include "tentes.h"

//Créer le plateau
int **creation_plateau(int x, int y) { 
    //Allocation de mémoire pour toutes les cases du plateau 
    int **Plateau = (int **)malloc(x * sizeof(int *));
    if (Plateau == NULL) return NULL;
    for (int i = 0; i < x; i++) Plateau[i] = (int *)malloc(y * sizeof(int));

    //Remplis le plateau d'herbe
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            Plateau[i][j] = HERBE; // Zone vide
        }
    }
    return Plateau;
}

void free_plateau(int **Plateau, int x) {
    for (int i = 0; i < x; i++) free(Plateau[i]);
    free(Plateau);
}

void afficher_plateau(int **Plateau, int *nbArbreLignes, int *nbArbreColonnes, int x, int y) {
    if (Plateau == NULL) {
        printf("Erreur : le plateau est NULL.\n");
        return;
    }
    printf("  ");
    for (int i = 0; i < LIGNE; i++) printf(" %d", nbArbreLignes[i]);
    printf("\n  ");
    for (int i = 0; i < LIGNE; i++) printf(" _");
    printf("\n");
    for (int i = 0; i < x; i++) {
        printf("%d ", nbArbreColonnes[i]);
        printf("|");
        for (int j = 0; j < y; j++) {
            if (Plateau[i][j] == VIDE) printf(" ");
            else if (Plateau[i][j] == ARBRE) printf("A");
            else if (Plateau[i][j] == HERBE) printf("X");
            else if (Plateau[i][j] == TENTE) printf("T");
            printf("|");
        }
        printf("\n  |");
        for (int i = 0; i < LIGNE; i++) printf("_|");
        printf("\n");
    }
    printf("\n");
}

/* Ajoute des espaces vides autour des tentes */
void ajouter_vide(int **Plateau, int x, int y) {
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            if (Plateau[i][j] == ARBRE) {
                if (i > 0 && Plateau[i - 1][j] == HERBE) Plateau[i - 1][j] = VIDE;
                if (i < x - 1 && Plateau[i + 1][j] == HERBE) Plateau[i + 1][j] = VIDE;
                if (j > 0 && Plateau[i][j - 1] == HERBE) Plateau[i][j - 1] = VIDE;
                if (j < y - 1 && Plateau[i][j + 1] == HERBE) Plateau[i][j + 1] = VIDE;
            }
        }
    }
}

int main(void) {
    int **plateau = creation_plateau(LIGNE, COLONNE);
    int nbArbreLignes[LIGNE] = {0};
    int nbArbreColonnes[COLONNE] = {0};

    if (!lire_fichier("plan.pln", nbArbreLignes, nbArbreColonnes, plateau, ARBRE)) {
        free_plateau(plateau, LIGNE);
        return 1;
    }
    ajouter_vide(plateau, LIGNE, COLONNE);

    afficher_plateau(plateau, nbArbreLignes, nbArbreColonnes, LIGNE, COLONNE);

    free_plateau(plateau, LIGNE);
    return 0;
}
