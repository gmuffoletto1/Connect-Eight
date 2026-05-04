#include "Board.h"

#include <cassert>
#include <iostream>

void testInitialBoardIsEmpty()
{
    Board board;

    assert(!board.isFull());
    assert(board.winner() == Board::EMPTY);
    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            assert(board.cell(r, c) == Board::EMPTY);
        }
    }
}

void testDropAndUndo()
{
    Board board;
    Move move{};

    assert(board.dropPiece(3, Board::RED, move));
    assert(move.col == 3);
    assert(move.row == Board::ROWS - 1);
    assert(move.player == Board::RED);
    assert(!move.isRemoval);
    assert(board.cell(Board::ROWS - 1, 3) == Board::RED);
    assert(board.nextFreeRow(3) == Board::ROWS - 2);

    board.undo(move);
    assert(board.cell(Board::ROWS - 1, 3) == Board::EMPTY);
    assert(board.nextFreeRow(3) == Board::ROWS - 1);
}

void testColumnFull()
{
    Board board;
    Move move{};

    for (int i = 0; i < Board::ROWS; ++i) {
        assert(board.dropPiece(0, Board::YELLOW, move));
    }

    assert(board.isColumnFull(0));
    assert(board.nextFreeRow(0) == -1);
    assert(!board.dropPiece(0, Board::RED, move));
}

void testRemovalCollapseAndUndo()
{
    Board board;
    Move first{};
    Move second{};
    Move removal{};

    assert(board.dropPiece(4, Board::RED, first));
    assert(board.dropPiece(4, Board::YELLOW, second));

    assert(board.removePiece(Board::ROWS - 1, 4, Board::RED, removal));
    assert(removal.isRemoval);
    assert(removal.removedPlayer == Board::RED);
    assert(board.cell(Board::ROWS - 1, 4) == Board::YELLOW);
    assert(board.cell(Board::ROWS - 2, 4) == Board::EMPTY);

    board.undo(removal);
    assert(board.cell(Board::ROWS - 1, 4) == Board::RED);
    assert(board.cell(Board::ROWS - 2, 4) == Board::YELLOW);
}

void testWinnerDetection()
{
    Board board;
    Move move{};

    for (int c = 0; c < Board::WIN_LEN; ++c) {
        assert(board.dropPiece(c, Board::RED, move));
    }

    assert(board.winner() == Board::RED);
    const vector<pair<int, int>> cells = board.winningCells();
    assert(cells.size() == Board::WIN_LEN);
}

int main()
{
    testInitialBoardIsEmpty();
    testDropAndUndo();
    testColumnFull();
    testRemovalCollapseAndUndo();
    testWinnerDetection();

    std::cout << "Board tests passed\n";
    return 0;
}
