#include "Board.h"

#include <array>

Board::Board()
    : grid(ROWS, std::vector<char>(COLS, EMPTY))
{
}

bool Board::dropPiece(int col, char player, Move& outMove)
{
    const int row = nextFreeRow(col);
    if (row < 0) {
        return false;
    }

    grid[row][col] = player;
    outMove = {col, row, player, false, EMPTY};
    return true;
}

bool Board::removePiece(int row, int col, char actingPlayer, Move& outMove)
{
    if (row < 0 || row >= ROWS || col < 0 || col >= COLS || grid[row][col] == EMPTY) {
        return false;
    }

    const char removed = grid[row][col];
    for (int r = row; r > 0; --r) {
        grid[r][col] = grid[r - 1][col];
    }
    grid[0][col] = EMPTY;

    outMove = {col, row, actingPlayer, true, removed};
    return true;
}

void Board::undo(const Move& m)
{
    if (m.col < 0 || m.col >= COLS || m.row < 0 || m.row >= ROWS) {
        return;
    }

    if (!m.isRemoval) {
        grid[m.row][m.col] = EMPTY;
        return;
    }

    for (int r = 0; r < m.row; ++r) {
        grid[r][m.col] = grid[r + 1][m.col];
    }
    grid[m.row][m.col] = m.removedPlayer;
}

bool Board::isColumnFull(int col) const
{
    return col < 0 || col >= COLS || grid[0][col] != EMPTY;
}

bool Board::isFull() const
{
    for (int c = 0; c < COLS; ++c) {
        if (!isColumnFull(c)) {
            return false;
        }
    }
    return true;
}

char Board::winner() const
{
    const auto cells = winningCells();
    return cells.empty() ? EMPTY : grid[cells.front().first][cells.front().second];
}

std::vector<std::pair<int, int>> Board::winningCells() const
{
    static constexpr std::array<std::pair<int, int>, 4> dirs{{
        {0, 1},
        {1, 0},
        {1, 1},
        {1, -1},
    }};

    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            const char p = grid[r][c];
            if (p == EMPTY) {
                continue;
            }
            for (const auto& [dr, dc] : dirs) {
                if (!checkRunFrom(r, c, dr, dc, p)) {
                    continue;
                }

                std::vector<std::pair<int, int>> result;
                result.reserve(WIN_LEN);
                for (int i = 0; i < WIN_LEN; ++i) {
                    result.emplace_back(r + dr * i, c + dc * i);
                }
                return result;
            }
        }
    }
    return {};
}

int Board::nextFreeRow(int col) const
{
    if (col < 0 || col >= COLS) {
        return -1;
    }
    for (int r = ROWS - 1; r >= 0; --r) {
        if (grid[r][col] == EMPTY) {
            return r;
        }
    }
    return -1;
}

bool Board::checkRunFrom(int r, int c, int dr, int dc, char p) const
{
    for (int i = 0; i < WIN_LEN; ++i) {
        const int rr = r + dr * i;
        const int cc = c + dc * i;
        if (rr < 0 || rr >= ROWS || cc < 0 || cc >= COLS || grid[rr][cc] != p) {
            return false;
        }
    }
    return true;
}
