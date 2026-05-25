#include "Game.h"
#include "main.h"
#include <iostream>

// On initialise l'IA avec le plateau
Game::Game() : ai(&board) {}

// Boucle principale du jeu
void Game::play() {
    // On initialise le moteur UTTT
    game.initialize(10, Level::MEDIUM_1, Mode::ARENA, false, "Pseudo");

    while (!game.isAllGameFinish()) {
        board.reset(); // On remet tout à zéro

        while (!game.isFinish()) {
            GameMove opp;
            game.getMove(opp); // Coup adverse

            if (opp.row >= 0 && opp.col >= 0)
                board.applyMove(opp.row, opp.col, -1);

            if (!game.isFinish()) {
                GameMove myMove = ai.computeBestMove(); // Coup IA
                board.applyMove(myMove.row, myMove.col, 1);
                game.setMove(myMove); // On envoie le coup
            }
        }
    }
}
