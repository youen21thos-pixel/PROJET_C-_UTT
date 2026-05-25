#include "AI.h"

// Constructeur : on stocke le plateau
AI::AI(Board* b) : board(b), maxDepth(3) {}

// Minimax avec élagage alpha-beta
int AI::minimax(int depth, int alpha, int beta, bool isMax) {
    int mw = board->macroWinner();

    // Fin de partie ou profondeur atteinte
    if (mw != 0 || depth == 0)
        return evaluator.evaluate(*board);

    auto moves = board->getLegalMoves();
    if (moves.empty())
        return evaluator.evaluate(*board);

    int best = isMax ? -1000000 : 1000000;

    for (auto &m : moves) {
        Board backup = *board; // On sauvegarde l'état

        board->applyMove(m.row, m.col, isMax ? 1 : -1);
        int val = minimax(depth - 1, alpha, beta, !isMax);

        *board = backup; // On restaure

        if (isMax) {
            if (val > best) best = val;
            if (best > alpha) alpha = best;
        } else {
            if (val < best) best = val;
            if (best < beta) beta = best;
        }

        if (beta <= alpha) break; // Coupure alpha-beta
    }

    return best;
}

// Choisit le meilleur coup
GameMove AI::computeBestMove() {
    auto moves = board->getLegalMoves();
    if (moves.empty()) return { 0, 0 };

    // Ajuste la profondeur selon le nombre de coups possibles
    int nb = moves.size();
    if (nb <= 9)      maxDepth = 5;
    else if (nb <= 20) maxDepth = 4;
    else              maxDepth = 3;

    GameMove best = moves[0];
    int bestScore = -1000000;

    for (auto &m : moves) {
        Board backup = *board;

        board->applyMove(m.row, m.col, 1);
        int score = minimax(maxDepth - 1, -1000000, 1000000, false);

        *board = backup;

        if (score > bestScore) {
            bestScore = score;
            best = m;
        }
    }

    return best;
}
