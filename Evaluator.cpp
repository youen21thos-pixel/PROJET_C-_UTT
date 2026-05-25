#include "Evaluator.h"

// Évalue une mini-grille locale
int Evaluator::evalMini(const Board& board, int mr, int mc) const {
    int b[3][3];

    // On extrait la mini-grille
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            b[r][c] = board.getCell(mr * 3 + r, mc * 3 + c);

    int score = 0;

    // Centre
    if (b[1][1] == 1) score += 4;
    if (b[1][1] == -1) score -= 4;

    // Coins
    int corners[4][2] = { {0,0},{0,2},{2,0},{2,2} };
    for (auto &co : corners) {
        int v = b[co[0]][co[1]];
        if (v == 1) score += 2;
        else if (v == -1) score -= 2;
    }

    // Fonction interne pour évaluer une ligne
    auto lineScore = [&](int v1, int v2, int v3) {
        int cnt1 = (v1 == 1) + (v2 == 1) + (v3 == 1);
        int cntM1 = (v1 == -1) + (v2 == -1) + (v3 == -1);

        if (cnt1 > 0 && cntM1 == 0) {
            if (cnt1 == 2) return 8;
            if (cnt1 == 1) return 3;
        }
        if (cntM1 > 0 && cnt1 == 0) {
            if (cntM1 == 2) return -8;
            if (cntM1 == 1) return -3;
        }
        return 0;
    };

    // Lignes / colonnes / diagonales
    for (int i = 0; i < 3; i++) {
        score += lineScore(b[i][0], b[i][1], b[i][2]);
        score += lineScore(b[0][i], b[1][i], b[2][i]);
    }
    score += lineScore(b[0][0], b[1][1], b[2][2]);
    score += lineScore(b[0][2], b[1][1], b[2][0]);

    return score;
}

// Score d'une ligne macro
int Evaluator::macroLineScore(int a, int b, int c) const {
    int vals[3] = { a, b, c };
    int cnt1 = 0, cntM1 = 0;

    for (int i = 0; i < 3; i++) {
        if (vals[i] == 1) cnt1++;
        else if (vals[i] == -1) cntM1++;
    }

    if (cnt1 > 0 && cntM1 == 0) {
        if (cnt1 == 2) return 120;
        if (cnt1 == 1) return 40;
    }
    if (cntM1 > 0 && cnt1 == 0) {
        if (cntM1 == 2) return -120;
        if (cntM1 == 1) return -40;
    }

    return 0;
}

// Score global du plateau
int Evaluator::evaluate(const Board& board) const {
    int mw = board.macroWinner();

    // Victoire immédiate
    if (mw == 1)  return 100000;
    if (mw == -1) return -100000;

    auto moves = board.getLegalMoves();
    if (moves.empty() || board.macroFull()) return 0;

    // Poids des cases macro
    static const int macroWeight[3][3] = {
        { 6, 4, 6 },
        { 4, 8, 4 },
        { 6, 4, 6 }
    };

    int score = 0;

    // Score des macro-cases gagnées
    for (int mr = 0; mr < 3; mr++) {
        for (int mc = 0; mc < 3; mc++) {
            int v = board.getMacro(mr, mc);
            if (v == 1)  score += macroWeight[mr][mc] * 40;
            if (v == -1) score -= macroWeight[mr][mc] * 40;
        }
    }

    // Lignes macro
    for (int i = 0; i < 3; i++) {
        score += macroLineScore(board.getMacro(i,0), board.getMacro(i,1), board.getMacro(i,2));
        score += macroLineScore(board.getMacro(0,i), board.getMacro(1,i), board.getMacro(2,i));
    }
    score += macroLineScore(board.getMacro(0,0), board.getMacro(1,1), board.getMacro(2,2));
    score += macroLineScore(board.getMacro(0,2), board.getMacro(1,1), board.getMacro(2,0));

    // Score des mini-grilles non finies
    for (int mr = 0; mr < 3; mr++)
        for (int mc = 0; mc < 3; mc++)
            if (!board.miniFinished(mr, mc) && board.getMacro(mr, mc) == 0)
                score += evalMini(board, mr, mc);

    // Bonus si la mini-grille imposée est intéressante
    int tr, tc;
    board.getTarget(tr, tc);
    if (tr != -1 && board.getMacro(tr, tc) == 0)
        score += 10;

    return score;
}
