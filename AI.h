#ifndef AI_H
#define AI_H

#include "Board.h"
#include "Evaluator.h"

// Classe IA : gère minimax et choisit le meilleur coup
class AI {
private:
    Board* board;        // Référence au plateau
    Evaluator evaluator; // Pour évaluer les positions
    int maxDepth;        // Profondeur de recherche

    int minimax(int depth, int alpha, int beta, bool isMax);

public:
    AI(Board* b);        // Constructeur
    GameMove computeBestMove(); // Retourne le meilleur coup
};

#endif
