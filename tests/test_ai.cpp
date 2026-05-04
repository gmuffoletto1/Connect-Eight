#include "AI.h"

#include <cassert>
#include <iostream>

int main()
{
    {
        Game game;
        AI ai(Board::YELLOW, 1);
        const Move move = ai.chooseMove(game);

        assert(!move.isRemoval);
        assert(move.col == Board::COLS / 2);
        assert(move.row == Board::ROWS - 1);
        assert(move.player == Board::YELLOW);
    }

    {
        Game game;
        for (int i = 0; i < Board::WIN_LEN - 1; ++i) {
            assert(game.playDrop(i));
            assert(game.playDrop(Board::COLS - 1 - i));
        }

        AI ai(Board::YELLOW, 1);
        const Move move = ai.chooseMove(game);
        assert(move.col >= 0);
        assert(move.player == Board::YELLOW);

        if (move.isRemoval) {
            assert(move.row >= 0);
            assert(move.row < Board::ROWS);
            assert(move.col < Board::COLS);
            assert(game.getBoard().cell(move.row, move.col) == Board::RED);
            assert(move.removedPlayer == Board::RED);
        } else {
            assert(move.col < Board::COLS);
            assert(!game.getBoard().isColumnFull(move.col));
            assert(move.row == game.getBoard().nextFreeRow(move.col));
        }
    }

    std::cout << "AI tests passed\n";
    return 0;
}
