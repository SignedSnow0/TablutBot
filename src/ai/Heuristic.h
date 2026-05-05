#pragma once

#include "state/Tablut.h"
#include <cstdint>

class Heuristic {
public:
    // Evaluates the overall state of the board
    static int64_t EvaluateState(const Tablut &state, bool isMax);

    // Fast evaluation of a single move for move-ordering
    static int64_t EvaluateMove(const PieceMove &move, const Tablut &state,
                                bool isMax);

private:
    // Private discriminators for player-specific logic
    static int64_t WhitePlayerHeuristic(const Tablut &state);
    static int64_t BlackPlayerHeuristic(const Tablut &state);
};