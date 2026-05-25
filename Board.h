#ifndef BOARD_H
#define BOARD_H

#include <vector>
#include "main.h"

// Classe qui gère tout l'état du jeu :
// - la grille 9x9
// - la macro-grille 3x3
// - la mini-grille cible
class Board {
private:
    int board[9][9];   // Grille complète
    int macro[3][3];   // Résultats des mini-grilles
    int targetR, targetC; // Mini-grille imposée (-1 = libre)

public:
    Board();       // Constructeur
    void reset();  // Réinitialise tout

    // Accès lecture
    int getCell(int r, int c) const;
    int getMacro(int mr, int mc) const;
    void getTarget(int& r, int& c) const;

    // Applique un coup sur la grille
    void applyMove(int r, int c, int player);

    // Fonctions utilitaires
    int miniWinner(int mr, int mc) const;
    int macroWinner() const;
    bool miniFinished(int mr, int mc) const;
    bool macroFull() const;

    // Retourne tous les coups légaux
    std::vector<GameMove> getLegalMoves() const;
};

#endif
