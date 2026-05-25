#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "AI.h"

// Classe qui gère la boucle de jeu avec le moteur UTTT
class Game {
private:
    Board board; // Plateau
    AI ai;       // IA

public:
    Game();      // Constructeur
    void play(); // Lance la partie
};

#endif
