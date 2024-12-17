#include "tentes.h"

//Fonction qui clear le terminal
void clear_terminal() {
    #ifdef _WIN32
        system("cls"); // Pour Windows
    #else
        system("clear"); // Pour Linux
    #endif
}

//Fonction pour lire une map du jeu de tentes
int lire_fichier(const char *nom_fichier, Plateau *Plateau) {
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
                Plateau -> tentesParLigne[i] = atoi(token);
                Plateau -> tentesParLigneOriginal[i] = Plateau -> tentesParLigne[i];
                //printf("ligne %d : %d\n", i, atoi(token));
                i++;
                token = strtok(NULL, ";");
            }
        } else if (lire_colonnes) {
            char *token = strtok(buffer, ";");
            int i = 0;
            while (token) {
                Plateau -> tentesParColonne[i] = atoi(token);
                Plateau -> tentesParColonneOriginal[i] = Plateau -> tentesParColonne[i];
                //printf("colonne %d : %d\n", i, atoi(token));
                i++;
                token = strtok(NULL, ";");
            }
        } else if (lire_arbres) {
            char *token = strtok(buffer, ";");
            while (token) {
                int x = atoi(token);
                token = strtok(NULL, ";");
                if (!token) break;
                int y = atoi(token);
                Plateau -> grille[y][x] = ARBRE;
                token = strtok(NULL, ";");
            }
        }
    }

    fclose(fichier);
    return 1;
}

/* Alloue de l'espace pour les cases du Plateau. Les cases sont initialisées avec de l'Herbe */
Plateau *creation_plateau(void) {
    Plateau *p = malloc(sizeof(Plateau));
    if (p == NULL) return NULL;

    // Allocation pour la grille
    p -> grille = malloc(LIGNES * sizeof(int *));
    if (p -> grille == NULL) {
        free(p);
        return NULL;
    }
    for (int i = 0; i < LIGNES; i++) {
        p -> grille[i] = malloc(COLONNES * sizeof(int));
        if (p -> grille[i] == NULL) {
            for (int j = 0; j < i; j++) free(p -> grille[j]);
            free(p -> grille);
            free(p);
            return NULL;
        }
    }

    // Allocation des tentes requise sur chaque cases
    p -> tentesParLigne = malloc(LIGNES * sizeof(int));
    p -> tentesParColonne = malloc(COLONNES * sizeof(int));
    if (p -> tentesParLigne == NULL || p -> tentesParColonne == NULL) {
        free(p -> grille);
        free(p);
        return NULL;
    }

    p -> tentesParLigneOriginal = malloc(LIGNES * sizeof(int));
    p -> tentesParColonneOriginal = malloc(COLONNES * sizeof(int));
    if (p -> tentesParLigneOriginal == NULL || p -> tentesParColonneOriginal == NULL) {
        free(p -> grille);
        free(p -> tentesParLigne);
        free(p -> tentesParColonne);
        free(p);
        return NULL;
    }

    // Initialisation du Plateau avec de l'Herbe sur chaque case
    for (int i = 0; i < LIGNES; i++) for (int j = 0; j < COLONNES; j++) p -> grille[i][j] = HERBE;

    return p;
}

/* Libère l'espace alloué au Plateau */
void free_plateau(Plateau *p) {
    if (p == NULL) return;

    for (int i = 0; i < LIGNES; i++) free(p -> grille[i]);
    free(p -> grille);
    free(p -> tentesParLigne);
    free(p -> tentesParColonne);
    free(p -> tentesParLigneOriginal);
    free(p -> tentesParColonneOriginal);

    free(p);
}

/* Affiche le Plateau */
void afficher_plateau(Plateau *p) {
    if (p == NULL) {
        printf("Erreur : le Plateau est NULL.\n");
        return;
    }
    printf("  ");
    for (int i = 0; i < LIGNES; i++) printf(" %d", p -> tentesParLigne[i]);
    printf("\n  ");
    for (int i = 0; i < LIGNES; i++) printf(" _");
    printf("\n");
    for (int i = 0; i < LIGNES; i++) {
        printf("%d ", p -> tentesParColonne[i]);
        printf("|");
        for (int j = 0; j < COLONNES; j++) {
            if (p  ->  grille[i][j] == VIDE) printf(" ");
            else if (p -> grille[i][j] == ARBRE) printf("\033[0;32mA\033[0m");
            else if (p -> grille[i][j] == HERBE) printf("\033[0;31mX\033[0m");
            else if (p -> grille[i][j] == TENTE) printf("\033[1;33mT\033[0m");
            printf("|");
        }
        printf("\n  |");
        for (int i = 0; i < LIGNES; i++) printf("_|");
        printf("\n");
    }
    printf("\n");
}

/* Remplacer les cases d'HERBES autour des ARBRES par du VIDE */
void remplir_vide_autour_arbre(Plateau *p){
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (p -> grille[i][j] == ARBRE) {
                if (i > 0 && p -> grille[i - 1][j] == HERBE) p -> grille[i - 1][j] = VIDE;
                if (i < LIGNES - 1 && p -> grille[i + 1][j] == HERBE) p -> grille[i + 1][j] = VIDE;
                if (j > 0 && p -> grille[i][j - 1] == HERBE) p -> grille[i][j - 1] = VIDE;
                if (j < COLONNES - 1 && p -> grille[i][j + 1] == HERBE) p -> grille[i][j + 1] = VIDE;
            }
        }
    }
}

/* Renvoie 1 si une tente est adjacente au point, sinon 0 */
int peut_enlever_herbe(Plateau *p, int i, int j){
    if (i > 0 && j > 0 && p -> grille[i - 1][j - 1] == TENTE) return 0;
    if (i > 0 && p -> grille[i - 1][j] == TENTE) return 0;
    if (i > 0 && j < COLONNES - 1 && p -> grille[i - 1][j + 1] == TENTE) return 0;
    if (j > 0 && p -> grille[i][j - 1] == TENTE) return 0;
    if (j < COLONNES - 1 && p -> grille[i][j + 1] == TENTE) return 0;
    if (i < LIGNES - 1 && j > 0 && p->grille[i + 1][j - 1] == TENTE) return 0;
    if (i < LIGNES - 1 && p -> grille[i + 1][j] == TENTE) return 0;
    if (i < LIGNES - 1 && j < COLONNES - 1 && p -> grille[i + 1][j + 1] == TENTE) return 0;

    return 1;   
}

/* Fonction qui vérifie si une tente peut être placée en (i, j) */
int peut_placer_tente(Plateau *p, int i, int j) {
    // Vérifier que la case donnée est VIDE
    if (p -> grille[i][j] != VIDE) return 0;

    // Vérifier qu'il reste des tentes à placer sur la ligne et la colonne
    if (p -> tentesParColonne[i] <= 0 || p -> tentesParLigne[j] <= 0) return 0;

    // Vérification de présence d'une tente adjacente à la case (i, j)
    if (i > 0 && j > 0 && p -> grille[i - 1][j - 1] == TENTE) return 0;
    if (i > 0 && p -> grille[i - 1][j] == TENTE) return 0;
    if (i > 0 && j < COLONNES - 1 && p -> grille[i - 1][j + 1] == TENTE) return 0;
    if (j > 0 && p -> grille[i][j - 1] == TENTE) return 0;
    if (j < COLONNES - 1 && p -> grille[i][j + 1] == TENTE) return 0;
    if (i < LIGNES - 1 && j > 0 && p->grille[i + 1][j - 1] == TENTE) return 0;
    if (i < LIGNES - 1 && p -> grille[i + 1][j] == TENTE) return 0;
    if (i < LIGNES - 1 && j < COLONNES - 1 && p -> grille[i + 1][j + 1] == TENTE) return 0;

    // Vérification de la présence d'un ARBRE autour de la case (i, j). S'il n'y en a pas, on ne place pas la tente
    if ((i == 0 || p -> grille[i - 1][j] != ARBRE) &&
        (i == LIGNES - 1 || p -> grille[i + 1][j] != ARBRE) &&
        (j == 0 || p -> grille[i][j - 1] != ARBRE) &&
        (j == COLONNES - 1 || p -> grille[i][j + 1] != ARBRE))
    return 0;

    return 1;
}

void placer_tente(Plateau *p, int i, int j) {
    if (i < 0 || i >= LIGNES || j < 0 || j >= COLONNES) return;

    p -> grille[i][j] = TENTE;

    // On décrémente le nombre de tente requises sur une ligne/colonne
    p -> tentesParColonne[i]--;
    p -> tentesParLigne[j]--;

    // Vérification de présence d'une tente adjacente à la case (i, j)
    if (i > 0 && j > 0 && p -> grille[i - 1][j - 1] == VIDE) p -> grille[i - 1][j - 1] = HERBE;
    if (i > 0 && p -> grille[i - 1][j] == VIDE) p -> grille[i-1][j] = HERBE;
    if (i > 0 && j < COLONNES - 1 && p -> grille[i - 1][j + 1] == VIDE) p -> grille[i - 1][j + 1] = HERBE;
    if (j > 0 && p -> grille[i][j - 1] == VIDE) p -> grille[i][j - 1] = HERBE;
    if (j < COLONNES - 1 && p -> grille[i][j + 1] == VIDE) p -> grille[i][j + 1] = HERBE;
    if (i < LIGNES - 1 && j > 0 && p -> grille[i + 1][j - 1] == VIDE) p -> grille[i + 1][j - 1] = HERBE;
    if (i < LIGNES - 1 && p -> grille[i + 1][j] == VIDE) p -> grille[i + 1][j] = HERBE;
    if (i < LIGNES - 1 && j < COLONNES - 1 && p -> grille[i + 1][j + 1] == VIDE) p -> grille[i + 1][j + 1] = HERBE;

    return;
}

void enlever_tente(Plateau *p, int i, int j) {
    if (i < 0 || i >= LIGNES || j < 0 || j >= COLONNES) return;

    p -> grille[i][j] = VIDE;

    // On décrémente le nombre de tente requises sur une ligne/colonne
    p -> tentesParColonne[i]++;
    p -> tentesParLigne[j]++;

    // Vérification de présence d'une tente adjacente à la case (i, j)
    if (i > 0 && j > 0 && p -> grille[i - 1][j - 1] == HERBE && peut_enlever_herbe(p, i - 1, j - 1))
        p -> grille[i - 1][j - 1] = VIDE;
    if (i > 0 && p -> grille[i - 1][j] == HERBE && peut_enlever_herbe(p, i - 1, j))
        p -> grille[i - 1][j] = VIDE;
    if (i > 0 && j < COLONNES - 1 && p -> grille[i - 1][j + 1] == HERBE && peut_enlever_herbe(p, i - 1, j + 1))
        p -> grille[i - 1][j + 1] = VIDE;
    if (j > 0 && p -> grille[i][j - 1] == HERBE && peut_enlever_herbe(p, i, j - 1))
        p -> grille[i][j - 1] = VIDE;
    if (j < COLONNES - 1 && p -> grille[i][j + 1] == HERBE && peut_enlever_herbe(p, i, j + 1))
        p -> grille[i][j + 1] = VIDE;
    if (i < LIGNES - 1 && j > 0 && p -> grille[i + 1][j - 1] == HERBE && peut_enlever_herbe(p, i + 1, j - 1))
        p -> grille[i + 1][j - 1] = VIDE;
    if (i < LIGNES - 1 && p -> grille[i + 1][j] == HERBE && peut_enlever_herbe(p, i + 1, j))
        p -> grille[i + 1][j] = VIDE;
    if (i < LIGNES - 1 && j < COLONNES - 1 && p -> grille[i + 1][j + 1] == HERBE && peut_enlever_herbe(p, i + 1, j + 1))
        p -> grille[i + 1][j + 1] = VIDE;

    return;
}

int resolution (Plateau *p){
    int modifications = 1;
    while (modifications){
        modifications = 0;
        for (int i = 0; i < LIGNES; i++) {
            int countVide = 0;
            
            // Compter le nombre de VIDE sur la ligne
            for (int j = 0; j < COLONNES; j++) {
                if (p -> grille[j][i] == VIDE) {
                    countVide++;
                }
            }
            // Si le nombre de VIDE est égal au nombre de tente à placer sur la LIGNE
            if (countVide == p -> tentesParLigne[i] || p -> tentesParLigne[i] == 0) {
                for (int j = 0; j < COLONNES; j++) {
                    if (peut_placer_tente(p, j, i)) { 
                        placer_tente(p, j, i); // On place les tentes sur toutes les cases vides
                        modifications = 1;
                    }
                }
            }
        }

        // La même chose pour les colonnes
        for (int i = 0; i < COLONNES; i++) {
            int countVide = 0;
            
            for (int j = 0; j < LIGNES; j++) {
                if (p -> grille[i][j] == VIDE) {
                    countVide++;
                }
            }
            
            if (countVide == p -> tentesParColonne[i] || p -> tentesParColonne[i] == 0) {
                for (int j = 0; j < COLONNES; j++) {
                    if (peut_placer_tente(p, i, j)) {
                        placer_tente(p, i, j);
                        modifications = 1;
                    }
                }
            }
        }
    }

    return 1;
}

int verif_solution(Plateau *p) {
    for (int i = 0; i < LIGNES; i++) if (p->tentesParLigne[i] != 0) return 0;
    for (int j = 0; j < COLONNES; j++) if (p->tentesParColonne[j] != 0) return 0;
    return 1;
}

int backtrack(Plateau *p) {
    // Si la solution est trouvée, on arrête
    if (verif_solution(p)) return 1;

    // Parcourir toutes les cases du plateau
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            // Essayer de placer une tente
            if (peut_placer_tente(p, i, j)) {
                placer_tente(p, i, j);

                if (backtrack(p)) return 1;

                // Annuler le placement si ça ne mène pas à une solution
                enlever_tente(p, i, j);
            }
        }
    }
    return 0; // Aucune solution trouvée
}

int backtrack_brut(Plateau *p) {
    if (verif_solution(p)) {
        return 1; // Solution trouvée
    }

    // Parcourir toutes les cases du plateau
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            if (p->grille[i][j] == VIDE) {
                p->grille[i][j] = TENTE;
                p->tentesParLigne[i]--;
                p->tentesParColonne[j]--;

                // On essaie d'autres placements
                if (backtrack(p)) return 1;

                // Annuler le placement si ça ne mène pas à une solution
                p->grille[i][j] = VIDE;
                p->tentesParLigne[i]++;
                p->tentesParColonne[j]++;
            }
        }
    }
    return 0;
}

void jouer(Plateau *p) {
    int choix, x, y;
    clear_terminal();
    afficher_plateau(p);

    printf("Options disponibles :\n");
    printf("1 : Placer une tente\n");
    printf("2 : Placer de l'herbe\n");
    printf("3 : Vérifier si la solution est résolue\n");
    printf("4 : Quitter\n");

    while (1) {
        clear_terminal();
        afficher_plateau(p);

        printf("Options disponibles :\n");
        printf("1 : Placer une tente\n");
        printf("2 : Enlever une tente\n");
        printf("3 : Placer de l'herbe\n");
        printf("4 : Enlever une herbe\n");
        printf("5 : Vérifier si la solution est résolue\n");
        printf("6 : Quitter\n");
        printf("\nEntrez votre choix : ");
        scanf("%d", &choix);

        switch (choix) {
            case 1:
                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position X (0-%d) : ", COLONNES - 1);
                scanf("%d", &x);
                sleep(1);

                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position Y (0-%d) : ", LIGNES - 1);
                scanf("%d", &y);
                sleep(1);

                clear_terminal();

                if (x >= 0 && x < COLONNES && y >= 0 && y < LIGNES && p -> grille[y][x] == VIDE) {
                    p -> grille[y][x] = TENTE;
                    p -> tentesParLigne[y]--;
                    p -> tentesParColonne[x]--;
                    printf("Tente placée en (%d, %d)\n", x, y);
                    sleep(2);
                    clear_terminal();
                } else {
                    printf("Placement invalide. Case occupée ou coordonnées incorrectes.\n");
                    sleep(2);
                    clear_terminal();
                }
                break;

            case 2:
                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position X (0-%d) : ", COLONNES - 1);
                scanf("%d", &x);
                sleep(1);

                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position Y (0-%d) : ", LIGNES - 1);
                scanf("%d", &y);
                sleep(1);

                clear_terminal();

                if (x >= 0 && x < COLONNES && y >= 0 && y < LIGNES && p -> grille[y][x] == TENTE) {
                    p -> grille[y][x] = VIDE;
                    p -> tentesParLigne[y]++;
                    p -> tentesParColonne[x]++;
                    printf("Tente enlevée en (%d, %d)\n", x, y);
                    sleep(2);
                    clear_terminal();
                } else {
                    printf("Placement invalide. Case occupée ou coordonnées incorrectes.\n");
                    sleep(2);
                    clear_terminal();
                }
                break;

            case 3:
                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position x (0-%d) : ", COLONNES - 1);
                scanf("%d", &x);
                sleep(1);

                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position y (0-%d) : ", LIGNES - 1);
                scanf("%d", &y);
                sleep(1);

                clear_terminal();

                if (x >= 0 && x < COLONNES && y >= 0 && y < LIGNES && p -> grille[y][x] == VIDE ) {
                    p -> grille[y][x] = HERBE;
                    printf("Herbe placée en (%d, %d)\n", x, y);
                    sleep(2);
                    clear_terminal();;
                } else {
                    printf("Placement invalide. Case occupée ou coordonnées incorrectes.\n");
                    sleep(2);
                    clear_terminal();
                }
                break;

            case 4:
                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position X (0-%d) : ", COLONNES - 1);
                scanf("%d", &x);
                sleep(1);

                clear_terminal();
                afficher_plateau(p);
                printf("Entrez la position X (0-%d) : ", LIGNES - 1);
                scanf("%d", &y);
                sleep(1);

                clear_terminal();

                if (x >= 0 && x < COLONNES && y >= 0 && y < LIGNES && p -> grille[y][x] == HERBE ) {
                    p -> grille[y][x] = HERBE;
                    printf("Herbe enlevée en (%d, %d)\n", x, y);
                    sleep(2);
                    clear_terminal();;
                } else {
                    printf("Placement invalide. Case occupée ou coordonnées incorrectes.\n");
                    sleep(2);
                    clear_terminal();
                }
                break;

            case 5:
            clear_terminal();
                if (verif_solution(p)) {
                    printf("Félicitations, vous avez compléter le puzzle !\n");
                    sleep(2);
                    return;
                } else {
                    printf("La solution n'est pas encore résolue.\n");
                    sleep(2);
                }
                break;

            case 6:
                clear_terminal();
                printf("Merci d'avoir joué. À bientôt !\n");
                sleep(2);
                return;

            default:
                clear_terminal();
                printf("Choix invalide. Veuillez entrer 1, 2, 3 ou 4.\n");
                sleep(2);
                break;
        }

        afficher_plateau(p); // Afficher l'état actuel du plateau après chaque action
    }
}

int main(void) {
    clear_terminal();
    Plateau *p = creation_plateau();
    int choix;

    // Lecture du fichier pour récupérer les informations du Plateau
    if (!lire_fichier("facile0.pln", p)) {
        free_plateau(p);
        return 1;
    }

    printf("Bienvenue dans le jeu des tentes !\n");
    printf("Options disponibles :\n");
    printf("1 : Jouer\n");
    printf("2 : Résoudre par logique\n");
    printf("3 : Résoudre par backtrack\n");
    printf("4 : Résoudre par backtrack brut\n");
    printf("5 : Quitter\n");

    printf("\nEntrez votre choix : ");
    scanf("%d", &choix);
    clear_terminal();

    switch (choix) {
        case 1:
            // On remplit toutes les cases de vide
            for (int i = 0; i < LIGNES; i++) {
                for (int j = 0; j < COLONNES; j++) {
                    if (p -> grille[i][j] != ARBRE) p -> grille[i][j] = VIDE;
                }
            }
            jouer(p);
            break;

        case 2:
            remplir_vide_autour_arbre(p);
            resolution(p)
            ? printf("Solution trouvée :\n\n")
            : printf("Aucune solution possible.\n\n");
            afficher_plateau(p);
            return 0;

        case 3:
            remplir_vide_autour_arbre(p);
            backtrack(p)
            ? printf("Solution trouvée :\n\n")
            : printf("Aucune solution possible.\n\n");
            afficher_plateau(p);
            return 0;
        
        case 4:
            backtrack_brut(p)
            ? printf("Solution trouvée :\n\n")
            : printf("Aucune solution possible.\n\n");
            afficher_plateau(p);
            return 0;
        
        case 5:
            printf("Vous avez quitté le programme.\n\n");
            free_plateau(p);
            return 0;

        default:
            printf("Choix invalide. Veuillez entrer 1, 2, 3, 4 ou 5.\n\n");
            break;
    }
    free_plateau(p);
    return 0;
}
