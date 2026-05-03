#include "Game.h"

Game::Game()
    : board(),
      history(),
      turn(Board::RED),
      removalsR(REMOVALS_PER_PLAYER),
      removalsY(REMOVALS_PER_PLAYER) {}

bool Game::playDrop(int col) {
    if (isOver()) {
        return false;
    }

    Move move{};
    if (!board.dropPiece(col, turn, move)) {
        return false;
    }

    history.push(move);
    switchTurn();
    return true;
}

bool Game::playRemove(int row, int col) {
    if (isOver() || row < 0 || row >= Board::ROWS || col < 0 || col >= Board::COLS) {
        return false;
    }

    const char target = board.cell(row, col);
    if (target == Board::EMPTY || target == turn) {
        return false;
    }

    int& remaining = (turn == Board::RED) ? removalsR : removalsY;
    if (remaining <= 0) {
        return false;
    }

    Move move{};
    if (!board.removePiece(row, col, turn, move)) {
        return false;
    }

    --remaining;
    history.push(move);
    switchTurn();
    return true;
}

bool Game::undoLast() {
    if (history.empty()) {
        return false;
    }

    const Move move = history.top();
    history.pop();
    board.undo(move);
    turn = move.player;

    if (move.isRemoval) {
        int& remaining = (move.player == Board::RED) ? removalsR : removalsY;
        ++remaining;
    }

    return true;
}

void Game::reset() {
    board = Board();
    history = std::stack<Move>();
    turn = Board::RED;
    removalsR = REMOVALS_PER_PLAYER;
    removalsY = REMOVALS_PER_PLAYER;
}

int Game::removalsLeft(char player) const {
    if (player == Board::RED) {
        return removalsR;
    }
    if (player == Board::YELLOW) {
        return removalsY;
    }
    return 0;
}

void Game::switchTurn() {
    turn = (turn == Board::RED) ? Board::YELLOW : Board::RED;
}
