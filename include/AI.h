#ifndef AI_H
#define AI_H

#include "Board.h"
#include "Game.h"

#include <utility>
#include <vector>

class AI {
public:
    AI(char aiPlayer = Board::YELLOW, int depth = 3);

    // Pick the best move for the AI in the current game state.
    // The returned Move can be either a drop (isRemoval=false) or a removal.
    // If no moves exist (board full), returns a sentinel with col == -1.
    Move chooseMove(const Game& game);

    void setDepth(int d) { maxDepth = d; }
    int  getDepth() const { return maxDepth; }

private:
    char me;          // 'R' or 'Y' - which side the AI plays
    char opp;         // the other side
    int  maxDepth;

    // Only consider columns within radius 4 of any placed piece.
    // On an empty board, returns the center column.
    std::vector<int> candidateCols(const Board& b) const;

    // True if removing one of opponent's pieces would break a long opponent threat
    // (e.g., a run of 7). When true, the AI considers removals.
    bool opponentHasUrgentThreat(const Board& b) const;

    // Returns the set of (row, col) opponent pieces that are PART OF the longest
    // opponent run. These are the candidates for emergency removal.
    std::vector<std::pair<int,int>> threatRemovalCandidates(const Board& b) const;

    // Negamax-style minimax with alpha-beta. Returns score from 'player' perspective.
    // 'player' is the side to move at this node.
    int  minimax(Board& b,
                 int depth,
                 int alpha,
                 int beta,
                 char player,
                 int removalsMe,
                 int removalsOpp);

    // Heuristic evaluation from 'me' perspective.
    int  evaluate(const Board& b) const;

    static const int WIN_SCORE = 10000000;
};

#endif
