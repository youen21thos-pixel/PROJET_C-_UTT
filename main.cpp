#include <iostream>
#include <vector>
#include "main.h"

static int board[9][9] = {};
static int macro[3][3] = {};
static int targetR = -1, targetC = -1;

int miniWinner(int mr, int mc) {
    int b[3][3];
    for (int r=0;r<3;r++) for (int c=0;c<3;c++) b[r][c]=board[mr*3+r][mc*3+c];
    for (int i=0;i<3;i++) {
        if (b[i][0] && b[i][0]==b[i][1] && b[i][1]==b[i][2]) return b[i][0];
        if (b[0][i] && b[0][i]==b[1][i] && b[1][i]==b[2][i]) return b[0][i];
    }
    if (b[0][0] && b[0][0]==b[1][1] && b[1][1]==b[2][2]) return b[0][0];
    if (b[0][2] && b[0][2]==b[1][1] && b[1][1]==b[2][0]) return b[0][2];
    return 0;
}

int macroWinner() {
    for (int i=0;i<3;i++) {
        if (macro[i][0] && macro[i][0]==macro[i][1] && macro[i][1]==macro[i][2]) return macro[i][0];
        if (macro[0][i] && macro[0][i]==macro[1][i] && macro[1][i]==macro[2][i]) return macro[0][i];
    }
    if (macro[0][0] && macro[0][0]==macro[1][1] && macro[1][1]==macro[2][2]) return macro[0][0];
    if (macro[0][2] && macro[0][2]==macro[1][1] && macro[1][1]==macro[2][0]) return macro[0][2];
    return 0;
}

bool miniFinished(int mr, int mc) {
    if (macro[mr][mc]) return true;
    for (int r=0;r<3;r++) for (int c=0;c<3;c++) if (!board[mr*3+r][mc*3+c]) return false;
    return true;
}

void applyMove(int r, int c, int player) {
    board[r][c] = player;
    int mr=r/3, mc=c/3;
    macro[mr][mc] = miniWinner(mr, mc);
    int nr=r%3, nc=c%3;
    targetR = miniFinished(nr,nc) ? -1 : nr;
    targetC = miniFinished(nr,nc) ? -1 : nc;
}

std::vector<GameMove> getLegal() {
    std::vector<GameMove> moves;
    auto addMini = [&](int mr, int mc) {
        if (miniFinished(mr,mc)) return;
        for (int lr=0;lr<3;lr++) for (int lc=0;lc<3;lc++)
            if (!board[mr*3+lr][mc*3+lc]) moves.push_back({mr*3+lr, mc*3+lc});
    };
    if (targetR!=-1) addMini(targetR, targetC);
    else for (int mr=0;mr<3;mr++) for (int mc=0;mc<3;mc++) addMini(mr,mc);
    return moves;
}

int evaluate() {
    int mw=macroWinner();
    if (mw== 1) return  10000;
    if (mw==-1) return -10000;
    static const int w[3][3]={{3,2,3},{2,4,2},{3,2,3}};
    int score=0;
    for (int mr=0;mr<3;mr++) for (int mc=0;mc<3;mc++) score+=macro[mr][mc]*w[mr][mc]*50;
    return score;
}

int minimax(int depth, int alpha, int beta, bool isMax) {
    if (macroWinner() || depth==0) return evaluate();
    auto moves=getLegal();
    if (moves.empty()) return evaluate();
    int best=isMax?-99999:99999;
    for (auto& m : moves) {
        int prevTR=targetR, prevTC=targetC, pm[3][3];
        for(int i=0;i<3;i++) for(int j=0;j<3;j++) pm[i][j]=macro[i][j];
        applyMove(m.row, m.col, isMax?1:-1);
        int val=minimax(depth-1, alpha, beta, !isMax);
        board[m.row][m.col]=0;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++) macro[i][j]=pm[i][j];
        targetR=prevTR; targetC=prevTC;
        if (isMax){best=std::max(best,val);alpha=std::max(alpha,best);}
        else      {best=std::min(best,val);beta =std::min(beta, best);}
        if (beta<=alpha) break;
    }
    return best;
}

GameMove computeBestMove() {
    auto moves=getLegal();
    if (moves.empty()) return {0,0};
    int nb=(int)moves.size(), depth=(nb<=9)?6:(nb<=20)?5:4;
    GameMove best=moves[0]; int bestScore=-99999;
    for (auto& m : moves) {
        int prevTR=targetR, prevTC=targetC, pm[3][3];
        for(int i=0;i<3;i++) for(int j=0;j<3;j++) pm[i][j]=macro[i][j];
        applyMove(m.row, m.col, 1);
        int score=minimax(depth-1,-99999,99999,false);
        board[m.row][m.col]=0;
        for(int i=0;i<3;i++) for(int j=0;j<3;j++) macro[i][j]=pm[i][j];
        targetR=prevTR; targetC=prevTC;
        if (score>bestScore){bestScore=score; best=m;}
    }
    return best;
}

int main()
{
    // Game initialization
    game.initialize(10, Level::MEDIUM_1, Mode::DEBUG, false, "Pseudo");

    while (!game.isAllGameFinish())
    {
        for(int r=0;r<9;r++) for(int c=0;c<9;c++) board[r][c]=0;
        for(int r=0;r<3;r++) for(int c=0;c<3;c++) macro[r][c]=0;
        targetR=targetC=-1;

        GameMove myMove{0, 0};

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
