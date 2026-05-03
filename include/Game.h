#ifndef GAME_H
#define GAME_H

#include "Board.h"

#include <stack>

class Game {
public:
    static const int REMOVALS_PER_PLAYER = 3;

    Game();

    // Drop a piece for the current player into 'col'. Returns false if illegal.
    bool playDrop(int col);

    // Current player removes opponent's piece at (row, col). Returns false if illegal
    // (own piece, empty cell, no removals left, etc.).
    bool playRemove(int row, int col);

    // Pop the last move off the history stack and reverse it. Returns false if empty.
    bool undoLast();

    // Reset the game to a fresh board with first turn = RED.
    void reset();

    char currentPlayer() const { return turn; }
    int removalsLeft(char player) const;
    char winner() const { return board.winner(); }
    bool isOver() const { return winner() != Board::EMPTY || board.isFull(); }

    const Board& getBoard() const { return board; }
    Board& mutableBoard() { return board; }  // for AI search to mutate/undo

    int historySize() const { return static_cast<int>(history.size()); }

private:
    Board board;
    std::stack<Move> history;  // ADT #2: LIFO stack of moves (enables undo)
    char turn;                 // 'R' or 'Y'
    int removalsR;
    int removalsY;

    void switchTurn();
};

#endif
