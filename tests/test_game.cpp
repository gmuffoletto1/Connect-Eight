#include "Game.h"

#include <cassert>

void testStartsWithRed()
{
    Game game;

    assert(game.currentPlayer() == Board::RED);
    assert(game.removalsLeft(Board::RED) == Game::REMOVALS_PER_PLAYER);
    assert(game.removalsLeft(Board::YELLOW) == Game::REMOVALS_PER_PLAYER);
    assert(game.historySize() == 0);
    assert(!game.isOver());
}

void testDropSwitchesTurnsAndUndoRestores()
{
    Game game;

    assert(game.playDrop(3));
    assert(game.currentPlayer() == Board::YELLOW);
    assert(game.getBoard().cell(Board::ROWS - 1, 3) == Board::RED);
    assert(game.historySize() == 1);

    assert(game.undoLast());
    assert(game.currentPlayer() == Board::RED);
    assert(game.getBoard().cell(Board::ROWS - 1, 3) == Board::EMPTY);
    assert(game.historySize() == 0);
}

void testRemoveRulesAndReset()
{
    Game game;

    assert(game.playDrop(0));  // Red
    assert(game.playDrop(1));  // Yellow

    assert(!game.playRemove(Board::ROWS - 1, 0));  // Red cannot remove own piece.
    assert(game.playRemove(Board::ROWS - 1, 1));   // Red removes Yellow.
    assert(game.currentPlayer() == Board::YELLOW);
    assert(game.removalsLeft(Board::RED) == Game::REMOVALS_PER_PLAYER - 1);

    assert(game.undoLast());
    assert(game.currentPlayer() == Board::RED);
    assert(game.removalsLeft(Board::RED) == Game::REMOVALS_PER_PLAYER);
    assert(game.getBoard().cell(Board::ROWS - 1, 1) == Board::YELLOW);

    game.reset();
    assert(game.currentPlayer() == Board::RED);
    assert(game.historySize() == 0);
    assert(game.getBoard().cell(Board::ROWS - 1, 0) == Board::EMPTY);
}

int main()
{
    testStartsWithRed();
    testDropSwitchesTurnsAndUndoRestores();
    testRemoveRulesAndReset();
    return 0;
}
