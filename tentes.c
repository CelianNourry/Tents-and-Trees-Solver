#include "tentes.h"

/* Fonction qui clear le terminal */
void clear_terminal() {
    #ifdef _WIN32
        system("cls"); // Pour Windows
    #else
        system("clear"); // Pour Linux
    #endif
}

/*
    Lit un fichier pour en extraire les infromations nécessaires d'un jeu de Tentes.
    Format d'un fichier vailde :
        - doit commencer par "debutPlan"
        - si "lignes" est lu :
            lit les tentes nécéssaires sur chaque ligne du jeu. Le séparateur pour chaque nouvelle ligne est ";"
        - si "colonnes" est lu :
            lit les tentes nécéssaires sur chaque colonne du jeu. Le séparateur pour chaque nouvelle colonne est ";"
        - si "arbres" est lu :
            lit les coordonées (x, y) de chaque arbre. Le séparateur entre chaque coordonéee x et y est ";"
        - doit terminer par "finPlan"

*/
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

        if (strncmp(buffer, "arbres", 6) == 0) {
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

/* Alloue de l'espace pour les cases du Plateau. Les cases sont initialisées avec du VIDE */
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

    // Initialisation du Plateau avec du VIDE sur chaque case
    for (int i = 0; i < LIGNES; i++) for (int j = 0; j < COLONNES; j++) p -> grille[i][j] = VIDE;

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

/* Retourne un Plateau qui est la copie de celui mit en argument */
Plateau *copie_plateau(const Plateau *p) {
    if (p == NULL) return NULL;

    Plateau *copie = creation_plateau();
    if (copie == NULL) return NULL;

    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            copie -> grille[i][j] = p -> grille[i][j];
        }
    }

    for (int i = 0; i < LIGNES; i++) {
        copie -> tentesParLigne[i] = p -> tentesParLigne[i];
        copie -> tentesParLigneOriginal[i] = p -> tentesParLigneOriginal[i];
    }

    for (int j = 0; j < COLONNES; j++) {
        copie -> tentesParColonne[j] = p -> tentesParColonne[j];
        copie -> tentesParColonneOriginal[j] = p -> tentesParColonneOriginal[j];
    }

    return copie;
}

/* Remplace les informations du Plateau destination par celles du Plateau source */
void restaurer_plateau(Plateau *dest, const Plateau *src) {
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            dest -> grille[i][j] = src -> grille[i][j];
        }
    }

    for (int i = 0; i < LIGNES; i++) {
        dest -> tentesParLigne[i] = src->tentesParLigne[i];
        dest -> tentesParLigneOriginal[i] = src -> tentesParLigneOriginal[i];
    }

    for (int j = 0; j < COLONNES; j++) {
        dest -> tentesParColonne[j] = src -> tentesParColonne[j];
        dest -> tentesParColonneOriginal[j] = src -> tentesParColonneOriginal[j];
    }
}

/* Affiche le Plateau */
void afficher_plateau(Plateau *p) {
    if (p == NULL) {
        printf("Erreur : le Plateau est NULL.\n");
        return;
    }
    printf("  ");
    for (int i = 0; i < LIGNES; i++) printf(" %d", p -> tentesParLigneOriginal[i]);
    printf("\n  ");
    for (int i = 0; i < LIGNES; i++) printf(" _");
    printf("\n");
    for (int i = 0; i < LIGNES; i++) {
        printf("%d ", p -> tentesParColonneOriginal[i]);
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

/*
    Vérifie si une TENTE peut être placée sur la case (i, j) du Plateau
    La case (i, j) doit :
        - être VIDE
        - avoir encore de la place pour une nouvelle TENTE
        - ne pas avoir de case adjacente étant une TENTE 
        - avoir une case adjacente étant un ARBRE

    Retourne :
        - 0 : Si les conditions sont respectées
        - 1 : Si les conditions ne sont pas respectées
    
*/
int peut_placer_tente(Plateau *p, int i, int j) {
    // Vérifier que la case donnée est VIDE
    if (p -> grille[i][j] != VIDE) return 0;

    // Vérifier qu'il reste des tentes à placer sur la colonne
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

/*
    Place une TENTE sur la case (i, j) du Plateau
    La case (i, j) doit être :
        - Entre valide (i entre 0 et #LIGNES, j entre 0 et #COLONNES)

    Si une case adjacente (horizontale, verticale, diagonale) est VIDE, place de l'HERBE sur celle-ci
*/
void placer_tente(Plateau *p, int i, int j) {
    // Place une TENTE dans la case (i, j)
    p -> grille[i][j] = TENTE;

    // On décrémente le nombre de tente requises sur la ligne/colonne
    p -> tentesParColonne[i]--;
    p -> tentesParLigne[j]--;

    // Placement d'HERBE dans les cases adjacentes à la case (i, j)
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

/*
    Vérifie si toutes les lignes et les colonnes du Plateau sont à 0. Cela valide la solution si c'est le cas.
    Retourne :
        - 1 : Si c'est le cas
        - 0 : Si ce n'est pas le cas
*/
int verif_solution(Plateau *p) {
    for (int i = 0; i < LIGNES; i++) if (p -> tentesParLigne[i] != 0) return 0;
    for (int j = 0; j < COLONNES; j++) if (p -> tentesParColonne[j] != 0) return 0;
    return 1;
}

/*
    Applique une résolution logique au Plateau
    - Si le nombre de TENTES requises sur une ligne/colonne est égal à 0, on met de l'HERBE tout le long de celle-ci
    - Si le nombre de case VIDE est égal au nombre de TENTES restantes à placer sur une ligne/colonne, tentative de placer une tente
    - Si un seul espace VIDE est disponible autour d'un ARBRE, tentative de placer une TENTE sur celui-ci

    Tant qu'une modification est apportée au Plateau, on continue fait tourner la fonction
    Retourne :
        - 1 : Si la solution est trouvée
        - 0 : Si la solution n'est pas trouvée
*/
int resolution_logique(Plateau *p){
    int modifications = 1;
    while (modifications){
        modifications = 0;
        for (int i = 0; i < LIGNES; i++) {
            int countVide = 0;

            // Si aucune tente n'est requise sur une ligne, on met de l'HERBE tout le long de celle-ci
            if (p -> tentesParLigne[i] == 0) for (int j = 0; j < COLONNES; j++) if (p -> grille[j][i] == VIDE) p -> grille[j][i] = HERBE;
            
            // Compter le nombre de VIDE sur la ligne
            for (int j = 0; j < COLONNES; j++) {
                if (p -> grille[j][i] == VIDE) {
                    countVide++;
                }
            }

            // Si le nombre de VIDE est égal au nombre de tente à placer sur la LIGNE
            if (countVide == p -> tentesParLigne[i]) {
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

            if (p -> tentesParColonne[i] == 0) for (int j = 0; j < LIGNES; j++) if (p -> grille[i][j] == VIDE) p -> grille[i][j] = HERBE;
            
            for (int j = 0; j < LIGNES; j++) {
                if (p -> grille[i][j] == VIDE) {
                    countVide++;
                }
            }
            
            if (countVide == p -> tentesParColonne[i]) {
                for (int j = 0; j < COLONNES; j++) {
                    if (peut_placer_tente(p, i, j)) {
                        placer_tente(p, i, j);
                        modifications = 1;
                    }
                }
            }
        }
        
        // S'il y a seulement un espace vide autour d'un ARBRE, on y place une TENTE
        for (int i = 0; i < LIGNES; i++){
            for (int j = 0; j < COLONNES; j++){
                if (p -> grille[i][j] == ARBRE){
                    int espaces_vides = 0;
                    int direction_i = -1, direction_j = -1;

                    if (i > 0 && p -> grille[i - 1][j] == VIDE) { espaces_vides++; direction_i = i - 1; direction_j = j;}
                    if (i < LIGNES - 1 && p -> grille[i + 1][j] == VIDE) { espaces_vides++; direction_i = i + 1; direction_j = j;}
                    if (j > 0 && p -> grille[i][j - 1] == VIDE) { espaces_vides++; direction_i = i; direction_j = j - 1;}
                    if (j < COLONNES - 1 && p -> grille[i][j + 1] == VIDE) { espaces_vides++; direction_i = i; direction_j = j + 1;}
                    
                    if (espaces_vides == 1) {
                        if (peut_placer_tente(p, direction_i, direction_j)) {
                            placer_tente(p, direction_i, direction_j);
                            modifications = 1;
                        }
                    }
                }
            }
        }
        
    }

    if (verif_solution(p)) return 1;
    return 0;
}

/*
    Parcours toutes les cases du Plateau. Si on peut placer une TENTE sur l'une d'elle, on sauvegarde le Plateau avant de le faire.
    Ensuite, on fait resolution_logique() du nouveau Plateau.
    Puis on rappelle recursivement la fonction backtrack() pour qu'elle puisse placer des nouvelles tentes.
    Si on trouve une solution on retourne 1. Sinon, on restaure l'ancienne sauvegarde du Plateau.
    Retourne :
        - 1 : Si la solution est trouvée
        - 0 : Si la solution n'est pas trouvée
*/
int backtrack(Plateau *p, bool slowMode) {
    // Si la solution est trouvée, on arrête
    if (verif_solution(p)) return 1;
    if (slowMode){
        clear_terminal();
        afficher_plateau(p);
        sleep(1);
    }

    // Parcourir toutes les cases du plateau
    for (int i = 0; i < LIGNES; i++) {
        for (int j = 0; j < COLONNES; j++) {
            // Essayer de placer une tente
            if (peut_placer_tente(p, i, j)) {
                Plateau *backup = copie_plateau(p);
                placer_tente(p, i, j);
                if (slowMode){
                    clear_terminal();
                    afficher_plateau(p);
                    sleep(1);
                }
                resolution_logique(p);
                if (slowMode){
                    clear_terminal();
                    afficher_plateau(p);
                    sleep(1);
                }

                if (backtrack(p, slowMode)) return 1;

                // Annuler le placement si ça ne mène pas à une solution
                restaurer_plateau(p, backup);
                free_plateau(backup);

                if (slowMode){
                    clear_terminal();
                    afficher_plateau(p);
                    sleep(1);
                }
            }
        }
    }

    return 0;
}

/* Fonction permettant de jouer de façon interactive au jeu de Tentes, sans résolution */
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
                }
                
                else {
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
                }
                
                else {
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
                }
                
                else {
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
                }
                
                else {
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
                }
                
                else {
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
                printf("Choix invalide.\n");
                sleep(2);
                break;
        }

        afficher_plateau(p); // Afficher l'état actuel du plateau après chaque action
    }
}

int main(void) {
    Plateau *p = creation_plateau();
    int choix;

    // Lecture du fichier pour récupérer les informations du Plateau
    if (!lire_fichier("intermediaire_8x8.pln", p)) {
        free_plateau(p);
        return 1;
    }

    printf("Bienvenue dans le jeu des tentes !\n");
    printf("Options disponibles :\n");
    printf("1 : Jouer\n");
    printf("2 : Résoudre par logique\n");
    printf("3 : Résoudre par backtrack\n");
    printf("4 : Quitter\n");

    printf("\nEntrez votre choix : ");
    scanf("%d", &choix);
    clear_terminal();

    switch (choix) {
        case 1:
            jouer(p);
            break;

        case 2:
            resolution_logique(p)
            ? printf("Solution trouvée :\n\n")
            : printf("Aucune solution possible.\n\n");
            afficher_plateau(p);
            return 0;

        case 3:
            int choixModeInt;
            bool choixMode;
            clear_terminal();
            printf("Voulez-vous faire le backtrack en mode lent ?\n");
            printf("1 - Oui\n");
            printf("2 - Non\n");
            printf("\nEntrez votre choix : ");
            scanf("%d", &choixModeInt);
            switch (choixModeInt) {
                case 1:
                    choixMode = true;
                    break;

                case 2:
                    choixMode = false;
                    break;
                
                default:
                    printf("Choix invalide. Veuillez entrer 1 ou 2\n\n");
                    break;

            }
            clear_terminal();
            backtrack(p, choixMode)
            ? printf("Solution trouvée :\n\n")
            : printf("Aucune solution possible.\n\n");
            afficher_plateau(p);
            return 0;

        case 4:
            printf("Vous avez quitté le programme.\n");
            free_plateau(p);
            return 0;

        default:
            printf("Choix invalide. Veuillez entrer 1, 2, 3 ou 4.\n\n");
            break;
    }
    
    free_plateau(p);
    return 0;
}
