#include "Board.h"

// Constructeur : on initialise la grille
Board::Board() {
    reset();
}

// Remet tout à zéro
void Board::reset() {
    for (int r = 0; r < 9; r++)
        for (int c = 0; c < 9; c++)
            board[r][c] = 0;

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            macro[r][c] = 0;

    targetR = targetC = -1;
}

// Lecture d'une case
int Board::getCell(int r, int c) const {
    return board[r][c];
}

// Lecture d'une macro-case
int Board::getMacro(int mr, int mc) const {
    return macro[mr][mc];
}

// Lecture de la mini-grille imposée
void Board::getTarget(int& r, int& c) const {
    r = targetR;
    c = targetC;
}

// Détermine le gagnant d'une mini-grille
int Board::miniWinner(int mr, int mc) const {
    int b[3][3];

    // On extrait la mini-grille
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

// Détermine le gagnant de la macro-grille
int Board::macroWinner() const {
    for (int i = 0; i < 3; i++) {
        if (macro[i][0] && macro[i][0] == macro[i][1] && macro[i][1] == macro[i][2]) return macro[i][0];
        if (macro[0][i] && macro[0][i] == macro[1][i] && macro[1][i] == macro[2][i]) return macro[0][i];
    }

    if (macro[0][0] && macro[0][0] == macro[1][1] && macro[1][1] == macro[2][2]) return macro[0][0];
    if (macro[0][2] && macro[0][2] == macro[1][1] && macro[1][1] == macro[2][0]) return macro[0][2];

    return 0;
}

// Vérifie si une mini-grille est finie
bool Board::miniFinished(int mr, int mc) const {
    if (macro[mr][mc]) return true;

    for (int r = 0; r < 3; r++)
        for (int c = 0; c < 3; c++)
            if (!board[mr * 3 + r][mc * 3 + c]) return false;

    return true;
}

// Vérifie si toutes les mini-grilles sont finies
bool Board::macroFull() const {
    for (int mr = 0; mr < 3; mr++)
        for (int mc = 0; mc < 3; mc++)
            if (!miniFinished(mr, mc)) return false;
    return true;
}

// Applique un coup et met à jour la mini-grille imposée
void Board::applyMove(int r, int c, int player) {
    board[r][c] = player;

    int mr = r / 3, mc = c / 3;
    int w = miniWinner(mr, mc);
    if (w != 0) macro[mr][mc] = w;

    int nr = r % 3, nc = c % 3;
    if (miniFinished(nr, nc)) {
        targetR = targetC = -1;
    } else {
        targetR = nr;
        targetC = nc;
    }
}

// Retourne tous les coups légaux
std::vector<GameMove> Board::getLegalMoves() const {
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
