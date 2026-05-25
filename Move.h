#ifndef MOVE_H_INCLUDED
#define MOVE_H_INCLUDED

#include "main.h"

// AUCUNE redéfinition de GameMove ici

// Déclarations des fonctions globales
void applyMove(int r, int c, int player);
GameMove computeBestMove();

#endif
