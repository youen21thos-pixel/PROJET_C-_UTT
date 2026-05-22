#include <iostream>
#include <vector>
#include "main.h"

static int board[9][9] = {};
static int macro[3][3] = {};
static int targetR = -1, targetC = -1;

int miniWinner(int mr, int mc) {
    int b[3][3];
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            b[r][c] = board[mr * 3 + r][mc * 3 + c];

    // Lignes / colonnes
    for (int i = 0; i < 3; i++) {
        if (b[i][0] && b[i][0] == b[i][1] && b[i][1] == b[i][2]) return b[i][0];
        if (b[0][i] && b[0][i] == b[1][i] && b[1][i] == b[2][i]) return b[0][i];
    }
    // Diagonales
    if (b[0][0] && b[0][0] == b[1][1] && b[1][1] == b[2][2]) return b[0][0];
    if (b[0][2] && b[0][2] == b[1][1] && b[1][1] == b[2][0]) return b[0][2];

    return 0;
}

int macroWinner() {
    // Lignes / colonnes sur la macro-grille
    for (int i = 0; i < 3; i++) {
        if (macro[i][0] && macro[i][0] == macro[i][1] && macro[i][1] == macro[i][2]) return macro[i][0];
        if (macro[0][i] && macro[0][i] == macro[1][i] && macro[1][i] == macro[2][i]) return macro[0][i];
    }
    // Diagonales macro
    if (macro[0][0] && macro[0][0] == macro[1][1] && macro[1][1] == macro[2][2]) return macro[0][0];
    if (macro[0][2] && macro[0][2] == macro[1][1] && macro[1][1] == macro[2][0]) return macro[0][2];

    return 0;
}

bool miniFinished(int mr, int mc) {
    if (macro[mr][mc]) return true; // déjà gagnée
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (!board[mr * 3 + r][mc * 3 + c]) return false;
    return true; // pleine => finie (nulle)
}

bool macroFull() {
    for (int mr = 0; mr < 3; mr++)
        for (int mc = 0; mc < 3; mc++)
            if (!miniFinished(mr, mc)) return false;
    return true;
}

void applyMove(int r, int c, int player) {
    board[r][c] = player;

    int mr = r / 3, mc = c / 3;
    int w = miniWinner(mr, mc);
    if (w != 0) {
        macro[mr][mc] = w; // 1 ou -1 uniquement
    }

    int nr = r % 3, nc = c % 3;
    if (miniFinished(nr, nc)) {
        targetR = -1;
        targetC = -1;
    } else {
        targetR = nr;
        targetC = nc;
    }
}

std::vector<GameMove> getLegal() {
    std::vector<GameMove> moves;

    auto addMini = [&](int mr, int mc) {
        if (miniFinished(mr, mc)) return;
        for (int lr = 0; lr < 3; lr++)
            for (int lc = 0; lc < 3; lc++)
                if (!board[mr * 3 + lr][mc * 3 + lc])
                    moves.push_back({ mr * 3 + lr, mc * 3 + lc });
    };

    if (targetR != -1) {
        addMini(targetR, targetC);
    } else {
        for (int mr = 0; mr < 3; mr++)
            for (int mc = 0; mc < 3; mc++)
                addMini(mr, mc);
    }

    return moves;
}

// Heuristique pour une mini-grille locale (non finie)
int evalMini(int mr, int mc) {
    int b[3][3];
    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            b[r][c] = board[mr * 3 + r][mc * 3 + c];

    int score = 0;

    // Centre et coins
    if (b[1][1] == 1) score += 4;
    if (b[1][1] == -1) score -= 4;

    int corners[4][2] = { {0,0},{0,2},{2,0},{2,2} };
    for (auto &co : corners) {
        int v = b[co[0]][co[1]];
        if (v == 1) score += 2;
        else if (v == -1) score -= 2;
    }

    auto lineScore = [&](int v1, int v2, int v3) {
        int sum = v1 + v2 + v3;
        int cnt1 = (v1 == 1) + (v2 == 1) + (v3 == 1);
        int cntM1 = (v1 == -1) + (v2 == -1) + (v3 == -1);
        if (cnt1 > 0 && cntM1 == 0) {
            if (cnt1 == 2) return 8;   // deux en ligne pour nous
            if (cnt1 == 1) return 3;
        }
        if (cntM1 > 0 && cnt1 == 0) {
            if (cntM1 == 2) return -8; // deux en ligne pour eux
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

int evaluate() {
    int mw = macroWinner();
    if (mw == 1)  return 100000;
    if (mw == -1) return -100000;

    // Nulle globale (plus de coups possibles)
    auto moves = getLegal();
    if (moves.empty() || macroFull()) return 0;

    static const int macroWeight[3][3] = {
        { 6, 4, 6 },
        { 4, 8, 4 },
        { 6, 4, 6 }
    };

    int score = 0;

    // Contrôle des cases macro (gagnées)
    for (int mr = 0; mr < 3; mr++) {
        for (int mc = 0; mc < 3; mc++) {
            int v = macro[mr][mc];
            if (v == 1)  score += macroWeight[mr][mc] * 40;
            if (v == -1) score -= macroWeight[mr][mc] * 40;
        }
    }

    // Lignes potentielles sur la macro-grille
    auto macroLineScore = [&](int a, int b, int c) {
        int vals[3] = { a, b, c };
        int cnt1 = 0, cntM1 = 0;
        for (int i = 0; i < 3; i++) {
            if (vals[i] == 1) cnt1++;
            else if (vals[i] == -1) cntM1++;
        }
        if (cnt1 > 0 && cntM1 == 0) {
            if (cnt1 == 2) return 120; // quasi macro-ligne gagnée
            if (cnt1 == 1) return 40;
        }
        if (cntM1 > 0 && cnt1 == 0) {
            if (cntM1 == 2) return -120;
            if (cntM1 == 1) return -40;
        }
        return 0;
    };

    for (int i = 0; i < 3; i++) {
        score += macroLineScore(macro[i][0], macro[i][1], macro[i][2]);
        score += macroLineScore(macro[0][i], macro[1][i], macro[2][i]);
    }
    score += macroLineScore(macro[0][0], macro[1][1], macro[2][2]);
    score += macroLineScore(macro[0][2], macro[1][1], macro[2][0]);

    // Heuristique locale sur les mini-grilles non finies
    for (int mr = 0; mr < 3; mr++) {
        for (int mc = 0; mc < 3; mc++) {
            if (!miniFinished(mr, mc) && macro[mr][mc] == 0) {
                score += evalMini(mr, mc);
            }
        }
    }

    // Bonus pour jouer dans la mini-grille imposée (si elle est intéressante)
    if (targetR != -1) {
        // On valorise si la mini-grille cible n'est pas déjà perdue
        if (macro[targetR][targetC] == 0) {
            score += 10;
        }
    }

    return score;
}

int minimax(int depth, int alpha, int beta, bool isMax) {
    int mw = macroWinner();
    if (mw != 0 || depth == 0) {
        return evaluate();
    }

    auto moves = getLegal();
    if (moves.empty()) {
        return evaluate();
    }

    int best = isMax ? -1000000 : 1000000;

    for (auto &m : moves) {
        int prevTR = targetR, prevTC = targetC;
        int pm[3][3];
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                pm[i][j] = macro[i][j];

        applyMove(m.row, m.col, isMax ? 1 : -1);

        int val = minimax(depth - 1, alpha, beta, !isMax);

        // Undo
        board[m.row][m.col] = 0;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                macro[i][j] = pm[i][j];
        targetR = prevTR;
        targetC = prevTC;

        if (isMax) {
            if (val > best) best = val;
            if (best > alpha) alpha = best;
        } else {
            if (val < best) best = val;
            if (best < beta) beta = best;
        }
        if (beta <= alpha) break;
    }

    return best;
}

GameMove computeBestMove() {
    auto moves = getLegal();
    if (moves.empty()) return { 0, 0 };

    int nb = (int)moves.size();
    int depth;
    if (nb <= 9)      depth = 5;
    else if (nb <= 20) depth = 4;
    else              depth = 3;

    GameMove best = moves[0];
    int bestScore = -1000000;

    for (auto &m : moves) {
        int prevTR = targetR, prevTC = targetC;
        int pm[3][3];
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                pm[i][j] = macro[i][j];

        applyMove(m.row, m.col, 1);
        int score = minimax(depth - 1, -1000000, 1000000, false);

        // Undo
        board[m.row][m.col] = 0;
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                macro[i][j] = pm[i][j];
        targetR = prevTR;
        targetC = prevTC;

        if (score > bestScore) {
            bestScore = score;
            best = m;
        }
    }

    return best;
}

int main()
{
    // Game initialization
    game.initialize(10, Level::MEDIUM_2, Mode::ARENA, false, "Pseudo");

    while (!game.isAllGameFinish())
    {
        for (int r = 0; r < 9; r++)
            for (int c = 0; c < 9; c++)
                board[r][c] = 0;

        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                macro[r][c] = 0;

        targetR = targetC = -1;

        GameMove myMove{ 0, 0 };

        while (!game.isFinish())
        {
            // Get IA move
            GameMove gameMove;
            game.getMove(gameMove);
            std::cerr << "IA move " << gameMove.row << " " << gameMove.col << std::endl;

            if (gameMove.row >= 0 && gameMove.col >= 0)
                applyMove(gameMove.row, gameMove.col, -1);

            if (!game.isFinish())
            {
                myMove = computeBestMove();
                applyMove(myMove.row, myMove.col, 1);

                // Send your move
                std::cerr << "Send move " << myMove.row << " " << myMove.col << std::endl;
                game.setMove(myMove);
            }
        }
    }

    return 0;
}
