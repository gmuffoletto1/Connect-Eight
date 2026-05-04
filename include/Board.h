#ifndef BOARD_H
#define BOARD_H

#include <utility>
#include <vector>

using namespace std;

struct Move {
    int col;             // column acted on (placement or removal)
    int row;             // resulting row of placement, or row of removal
    char player;         // 'R' or 'Y' - who made the move
    bool isRemoval;      // true if remove, false if drop
    char removedPlayer;  // whose piece was removed (only meaningful when isRemoval)
};

class Board {
public:
    static const int ROWS = 50;
    static const int COLS = 50;
    static const int WIN_LEN = 8;

    static const char EMPTY = '.';
    static const char RED = 'R';
    static const char YELLOW = 'Y';

    Board();

    // Drop a piece into column 'col' for 'player'. Returns false if column is full.
    // On success, fills outMove with the executed move (col, resulting row, etc.).
    bool dropPiece(int col, char player, Move& outMove);

    // Remove piece at (row, col). Pieces above it slide down (gravity collapse).
    // Returns false if cell is empty or out of bounds.
    bool removePiece(int row, int col, char actingPlayer, Move& outMove);

    // Reverse a previously-applied move (used for undo and AI search).
    void undo(const Move& m);

    bool isColumnFull(int col) const;
    bool isFull() const;

    // Returns 'R', 'Y', or EMPTY (no winner yet).
    char winner() const;

    // Cells of the winning run (for GUI highlight). Empty if no winner.
    vector<pair<int, int>> winningCells() const;

    char cell(int r, int c) const { return grid[r][c]; }

    // Returns next free row in column (the row a new piece would land in), or -1 if full.
    int nextFreeRow(int col) const;

    const vector<vector<char>>& raw() const { return grid; }

private:
    vector<vector<char>> grid;  // ADT #1: 2D vector for the 50x50 board

    // Returns true if a run of WIN_LEN of player p starts at (r,c) going (dr,dc).
    bool checkRunFrom(int r, int c, int dr, int dc, char p) const;
};

#endif
