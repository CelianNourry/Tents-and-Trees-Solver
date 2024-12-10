#include "lecture_plan.h"
#include "tentes.h"

//Créer le Plateau
int **creation_plateau(int x, int y) { 
    //Allocation de mémoire pour toutes les cases du Plateau 
    int **Plateau = (int **)malloc(x * sizeof(int *));
    if (Plateau == NULL) return NULL;
    for (int i = 0; i < x; i++) Plateau[i] = (int *)malloc(y * sizeof(int));

    //Remplis le Plateau d'herbe
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

void afficher_plateau(int **Plateau, int *nbTentesLignes, int *nbTentesColonnes, int x, int y) {
    if (Plateau == NULL) {
        printf("Erreur : le Plateau est NULL.\n");
        return;
    }
    printf("  ");
    for (int i = 0; i < LIGNE; i++) printf(" %d", nbTentesLignes[i]);
    printf("\n  ");
    for (int i = 0; i < LIGNE; i++) printf(" _");
    printf("\n");
    for (int i = 0; i < x; i++) {
        printf("%d ", nbTentesColonnes[i]);
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

/* Ajoute des espaces vides autour des arbres */
void ajouter_vide(int **Plateau) {
    for (int i = 0; i < LIGNE; i++) {
        for (int j = 0; j < COLONNE; j++) {
            if (Plateau[i][j] == ARBRE) {
                if (i > 0 && Plateau[i - 1][j] == HERBE) Plateau[i - 1][j] = VIDE;
                if (i < LIGNE - 1 && Plateau[i + 1][j] == HERBE) Plateau[i + 1][j] = VIDE;
                if (j > 0 && Plateau[i][j - 1] == HERBE) Plateau[i][j - 1] = VIDE;
                if (j < COLONNE - 1 && Plateau[i][j + 1] == HERBE) Plateau[i][j + 1] = VIDE;
            }
        }
    }
}

int vide_compte(int **Plateau, int x, bool LorR){
    int count = 0;
    if (LorR) {for (int i = 0; i < LIGNE; i++) if (Plateau[x][i] == VIDE) count++;} //Comptage d'espace vide sur les colonnes
    else {for (int i = 0; i < COLONNE; i++) if (Plateau[i][x] == VIDE) count++;} //Comptage d'espace vide sur les lignes
    return count;
}
/* Vérifie s'il y a une tente autour d'un point*/
int verifier_tente_autour(int **Plateau, int x, int y) {
    return
        (x > 0 && Plateau[x - 1][y] == TENTE) ||
        (x < LIGNE - 1 && Plateau[x + 1][y] == TENTE) ||
        (y > 0 && Plateau[x][y - 1] == TENTE) ||
        (y < COLONNE - 1 && Plateau[x][y + 1] == TENTE) ||
        (x > 0 && y > 0 && Plateau[x - 1][y - 1] == TENTE) ||
        (x > 0 && y < COLONNE - 1 && Plateau[x - 1][y + 1] == TENTE) ||
        (x < LIGNE - 1 && y > 0 && Plateau[x + 1][y - 1] == TENTE) ||
        (x < LIGNE - 1 && y < COLONNE - 1 && Plateau[x + 1][y + 1] == TENTE);
}

int **remplir_autour(int **Plateau, int x, int y) {
    if (x > 0 && Plateau[x - 1][y] == VIDE && verifier_tente_autour(Plateau, x - 1, y)) 
        Plateau[x - 1][y] = HERBE;
    if (x < LIGNE - 1 && Plateau[x + 1][y] == VIDE && verifier_tente_autour(Plateau, x + 1, y)) 
        Plateau[x + 1][y] = HERBE;
    if (y > 0 && Plateau[x][y - 1] == VIDE && verifier_tente_autour(Plateau, x, y - 1)) 
        Plateau[x][y - 1] = HERBE;
    if (y < COLONNE - 1 && Plateau[x][y + 1] == VIDE && verifier_tente_autour(Plateau, x, y + 1)) 
        Plateau[x][y + 1] = HERBE;

    // Diagonales
    if (x > 0 && y > 0 && Plateau[x - 1][y - 1] == VIDE && verifier_tente_autour(Plateau, x - 1, y - 1)) 
        Plateau[x - 1][y - 1] = HERBE;
    if (x > 0 && y < COLONNE - 1 && Plateau[x - 1][y + 1] == VIDE && verifier_tente_autour(Plateau, x - 1, y + 1)) 
        Plateau[x - 1][y + 1] = HERBE;
    if (x < LIGNE - 1 && y > 0 && Plateau[x + 1][y - 1] == VIDE && verifier_tente_autour(Plateau, x + 1, y - 1)) 
        Plateau[x + 1][y - 1] = HERBE;
    if (x < LIGNE - 1 && y < COLONNE - 1 && Plateau[x + 1][y + 1] == VIDE && verifier_tente_autour(Plateau, x + 1, y + 1)) 
        Plateau[x + 1][y + 1] = HERBE;

    return Plateau;
}


int **resolution(int **Plateau, int *nbTentesLignes, int *nbTentesColonnes){
    bool modifications = true;

    while (modifications) {
        modifications = false;
        for (int i = 0; i < LIGNE; i++){
            if (nbTentesLignes[i] == 0){ //S'il y a 0 tente sur une colonne, on remplis celle-ci d'herbre
                for (int j = 0; j < COLONNE; j++) if (Plateau[j][i] == VIDE) {Plateau[j][i] = HERBE; modifications = 1;}
            }
            else if (nbTentesLignes[i] == vide_compte(Plateau, i, false)){ //Si le nombre de case vide est égal au nombre de tentes restantes à placer, on les placent tous sans réfléchir
                for (int j = 0; j < COLONNE; j++) if (Plateau[j][i] == VIDE){
                    if (nbTentesColonnes[j] > 0){ //On place la tente que si c'est possible sur la colonne
                        Plateau[j][i] = TENTE;
                        remplir_autour(Plateau, j, i);
                        nbTentesLignes[i]--;
                        nbTentesColonnes[j]--;
                        modifications = true;
                    }
                }
            }
        }

        for (int i = 0; i < COLONNE; i++){
            if (nbTentesColonnes[i] == 0){
                for (int j = 0; j < LIGNE; j++) if (Plateau[i][j] == VIDE) {Plateau[i][j] = HERBE;}
            }
            else if (nbTentesColonnes[i] == vide_compte(Plateau, i, true)){
                for (int j = 0; j < LIGNE; j++) if (Plateau[i][j] == VIDE){
                    if (nbTentesLignes[j] > 0){
                        Plateau[i][j] = TENTE;
                        remplir_autour(Plateau, i, j);
                        nbTentesColonnes[i]--;
                        nbTentesLignes[j]--;
                        modifications = true;
                    }
                }
            }
        }
    }
    return Plateau;
}
int main(void) {
    int **Plateau = creation_plateau(LIGNE, COLONNE);
    int originalLignes[LIGNE] = {0};
    int originalColonnes[COLONNE] = {0};

    int nbTentesLignes[LIGNE];
    int nbTentesColonnes[COLONNE];

    if (!lire_fichier("facile0.pln", originalLignes, originalColonnes, Plateau, ARBRE)) {
        free_plateau(Plateau, LIGNE);
        return 1;
    }
    for (int i = 0; i < LIGNE; i++) nbTentesLignes[i] = originalLignes[i];
    for (int i = 0; i < COLONNE; i++) nbTentesColonnes[i] = originalColonnes[i];

    ajouter_vide(Plateau);
    Plateau = resolution(Plateau, nbTentesLignes, nbTentesColonnes);

    afficher_plateau(Plateau, originalLignes, originalColonnes, LIGNE, COLONNE);

    free_plateau(Plateau, LIGNE);
    return 0;
}
