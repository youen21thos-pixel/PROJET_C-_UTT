#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "Board.h"

// Classe qui calcule la valeur d'un plateau
class Evaluator {
private:
    int evalMini(const Board& board, int mr, int mc) const; // Score d'une mini-grille
    int macroLineScore(int a, int b, int c) const;          // Score d'une ligne macro

public:
    int evaluate(const Board& board) const; // Score global
};

#endif
