#include "AI.h"

#include <algorithm>
#include <climits>
#include <cmath>
#include <set>

namespace {
constexpr int kWinScore = 10000000;

char otherPlayer(char p)
{
    return p == Board::RED ? Board::YELLOW : Board::RED;
}

bool inBounds(int r, int c)
{
    return r >= 0 && r < Board::ROWS && c >= 0 && c < Board::COLS;
}

int runLengthFrom(const Board& b, int r, int c, int dr, int dc, char p)
{
    int len = 0;
    while (inBounds(r, c) && b.cell(r, c) == p) {
        ++len;
        r += dr;
        c += dc;
    }
    return len;
}

int longestRun(const Board& b, char p)
{
    static const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    int best = 0;

    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            if (b.cell(r, c) != p) {
                continue;
            }
            for (const auto& d : dirs) {
                const int prevR = r - d[0];
                const int prevC = c - d[1];
                if (inBounds(prevR, prevC) && b.cell(prevR, prevC) == p) {
                    continue;
                }
                best = std::max(best, runLengthFrom(b, r, c, d[0], d[1], p));
            }
        }
    }

    return best;
}

int windowScore(int mine, int theirs, int empty)
{
    if (mine > 0 && theirs > 0) {
        return 0;
    }
    if (mine == Board::WIN_LEN) {
        return kWinScore;
    }
    if (theirs == Board::WIN_LEN) {
        return -kWinScore;
    }

    auto value = [](int count, int spaces) {
        if (count == 0) {
            return 0;
        }
        int score = 1;
        for (int i = 0; i < count; ++i) {
            score *= 5;
        }
        return score + spaces;
    };

    if (mine > 0) {
        return value(mine, empty);
    }
    if (theirs > 0) {
        return -value(theirs, empty);
    }
    return 0;
}
}  // namespace

AI::AI(char aiPlayer, int depth)
    : me(aiPlayer), opp(otherPlayer(aiPlayer)), maxDepth(depth)
{
}

Move AI::chooseMove(const Game& game)
{
    const Board& start = game.getBoard();
    if (start.isFull()) {
        return Move{-1, -1, me, false, Board::EMPTY};
    }

    Move best{-1, -1, me, false, Board::EMPTY};
    int bestScore = INT_MIN;
    const int myRemovals = game.removalsLeft(me);
    const int theirRemovals = game.removalsLeft(opp);
    const std::vector<int> cols = candidateCols(start);

    for (int col : cols) {
        Board b = start;
        Move m{};
        if (!b.dropPiece(col, me, m)) {
            continue;
        }
        const int score = minimax(b, maxDepth - 1, INT_MIN + 1, INT_MAX - 1, opp, myRemovals, theirRemovals);
        if (score > bestScore) {
            bestScore = score;
            best = m;
        }
    }

    if (myRemovals > 0 && opponentHasUrgentThreat(start)) {
        for (const auto& rc : threatRemovalCandidates(start)) {
            Board b = start;
            Move m{};
            if (!b.removePiece(rc.first, rc.second, me, m)) {
                continue;
            }
            const int score = minimax(b, maxDepth - 1, INT_MIN + 1, INT_MAX - 1, opp, myRemovals - 1, theirRemovals);
            if (score > bestScore) {
                bestScore = score;
                best = m;
            }
        }
    }

    return best;
}

std::vector<int> AI::candidateCols(const Board& b) const
{
    std::set<int> cols;
    bool hasPiece = false;

    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            if (b.cell(r, c) == Board::EMPTY) {
                continue;
            }
            hasPiece = true;
            for (int dc = -4; dc <= 4; ++dc) {
                const int col = c + dc;
                if (col >= 0 && col < Board::COLS && !b.isColumnFull(col)) {
                    cols.insert(col);
                }
            }
        }
    }

    if (!hasPiece) {
        cols.insert(Board::COLS / 2);
    }

    return std::vector<int>(cols.begin(), cols.end());
}

bool AI::opponentHasUrgentThreat(const Board& b) const
{
    return longestRun(b, opp) >= Board::WIN_LEN - 1;
}

std::vector<std::pair<int, int>> AI::threatRemovalCandidates(const Board& b) const
{
    static const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    std::set<std::pair<int, int>> cells;
    int best = 0;

    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            if (b.cell(r, c) != opp) {
                continue;
            }
            for (const auto& d : dirs) {
                const int prevR = r - d[0];
                const int prevC = c - d[1];
                if (inBounds(prevR, prevC) && b.cell(prevR, prevC) == opp) {
                    continue;
                }

                const int len = runLengthFrom(b, r, c, d[0], d[1], opp);
                if (len < best) {
                    continue;
                }
                if (len > best) {
                    best = len;
                    cells.clear();
                }
                for (int i = 0; i < len; ++i) {
                    cells.insert({r + i * d[0], c + i * d[1]});
                }
            }
        }
    }

    if (best < Board::WIN_LEN - 1) {
        return {};
    }

    return std::vector<std::pair<int, int>>(cells.begin(), cells.end());
}

int AI::minimax(Board& b, int depth, int alpha, int beta, char player, int removalsMe, int removalsOpp)
{
    const char w = b.winner();
    if (w == me) {
        return WIN_SCORE + depth;
    }
    if (w == opp) {
        return -WIN_SCORE - depth;
    }
    if (depth == 0 || b.isFull()) {
        return evaluate(b);
    }

    const bool maximizingForMe = player == me;
    int best = maximizingForMe ? INT_MIN + 1 : INT_MAX - 1;

    for (int col : candidateCols(b)) {
        Move m{};
        if (!b.dropPiece(col, player, m)) {
            continue;
        }
        const int score = minimax(b, depth - 1, alpha, beta, otherPlayer(player), removalsMe, removalsOpp);
        b.undo(m);

        if (maximizingForMe) {
            best = std::max(best, score);
            alpha = std::max(alpha, score);
            if (alpha >= beta) {
                return best;
            }
        } else {
            best = std::min(best, score);
            beta = std::min(beta, score);
            if (alpha >= beta) {
                return best;
            }
        }
    }

    int& currentRemovals = maximizingForMe ? removalsMe : removalsOpp;
    if (currentRemovals > 0 && longestRun(b, otherPlayer(player)) >= Board::WIN_LEN - 1) {
        for (int r = 0; r < Board::ROWS; ++r) {
            for (int c = 0; c < Board::COLS; ++c) {
                if (b.cell(r, c) != otherPlayer(player)) {
                    continue;
                }
                Move m{};
                if (!b.removePiece(r, c, player, m)) {
                    continue;
                }
                --currentRemovals;
                const int score = minimax(b, depth - 1, alpha, beta, otherPlayer(player), removalsMe, removalsOpp);
                ++currentRemovals;
                b.undo(m);

                if (maximizingForMe) {
                    best = std::max(best, score);
                    alpha = std::max(alpha, score);
                    if (alpha >= beta) {
                        return best;
                    }
                } else {
                    best = std::min(best, score);
                    beta = std::min(beta, score);
                    if (alpha >= beta) {
                        return best;
                    }
                }
            }
        }
    }

    if (best == INT_MIN + 1 || best == INT_MAX - 1) {
        return evaluate(b);
    }
    return best;
}

int AI::evaluate(const Board& b) const
{
    static const int dirs[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    int score = 0;

    for (int r = 0; r < Board::ROWS; ++r) {
        for (int c = 0; c < Board::COLS; ++c) {
            for (const auto& d : dirs) {
                const int endR = r + (Board::WIN_LEN - 1) * d[0];
                const int endC = c + (Board::WIN_LEN - 1) * d[1];
                if (!inBounds(endR, endC)) {
                    continue;
                }

                int mine = 0;
                int theirs = 0;
                int empty = 0;
                for (int i = 0; i < Board::WIN_LEN; ++i) {
                    const char value = b.cell(r + i * d[0], c + i * d[1]);
                    if (value == me) {
                        ++mine;
                    } else if (value == opp) {
                        ++theirs;
                    } else {
                        ++empty;
                    }
                }
                score += windowScore(mine, theirs, empty);
            }
        }
    }

    for (int c = 0; c < Board::COLS; ++c) {
        for (int r = 0; r < Board::ROWS; ++r) {
            if (b.cell(r, c) == me) {
                score += 2 * (Board::ROWS - std::abs(c - Board::COLS / 2));
            } else if (b.cell(r, c) == opp) {
                score -= 2 * (Board::ROWS - std::abs(c - Board::COLS / 2));
            }
        }
    }

    return score;
}
