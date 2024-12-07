#include "lecture_plan.h"

//Fonction pour lire une map du jeu de tentes
int lire_fichier(const char *nom_fichier, int *lignes, int *colonnes, int **Plateau, int ID_ARBRE) {
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir le fichier.\n");
        return -1;
    }

    char buffer[256];
    bool lire_lignes = false, lire_colonnes = false, lire_arbres = false;

    while (fgets(buffer, sizeof(buffer), fichier)) {
        if (strncmp(buffer, "debutPlan", 9) == 0) continue;
        if (strncmp(buffer, "finPlan", 7) == 0) break;

        if (strncmp(buffer, "lignes", 6) == 0) {
            lire_lignes = true;
            lire_colonnes = lire_arbres = false;
            continue;
        }

        if (strncmp(buffer, "colonnes", 8) == 0) {
            lire_colonnes = true;
            lire_lignes = lire_arbres = false;
            continue;
        }

        if (strncmp(buffer, "arbres:", 7) == 0) {
            lire_arbres = true;
            lire_lignes = lire_colonnes = false;
            continue;
        }

        if (lire_lignes) {
            char *token = strtok(buffer, ";");
            int i = 0;
            while (token) {
                lignes[i++] = atoi(token);
                token = strtok(NULL, ";");
            }
        } else if (lire_colonnes) {
            char *token = strtok(buffer, ";");
            int i = 0;
            while (token) {
                colonnes[i++] = atoi(token);
                token = strtok(NULL, ";");
            }
        } else if (lire_arbres) {
            char *token = strtok(buffer, ";");
            while (token) {
                int x = atoi(token);
                token = strtok(NULL, ";");
                if (!token) break;
                int y = atoi(token);
                Plateau[y][x] = ID_ARBRE;
                token = strtok(NULL, ";");
            }
        }
    }

    fclose(fichier);
    return 1;
}