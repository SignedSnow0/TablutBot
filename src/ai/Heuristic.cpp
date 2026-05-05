#include "Heuristic.h"
#include "ai/Evaluations.h"
#include "state/Utils.h"
#include <limits>

int64_t Heuristic::EvaluateState(const Tablut &state, bool isMax) {
    if (!state.HasKing()) {
        return std::numeric_limits<int64_t>::min();
    }

    auto kingPosition = state.King().Position;
    if (IsInWinningPosition(kingPosition)) {
        return std::numeric_limits<int64_t>::max();
    }

    // Delegate to the specific player heuristic
    if (isMax) {
        return WhitePlayerHeuristic(state);
    } else {
        return BlackPlayerHeuristic(state);
    }
}

int64_t Heuristic::WhitePlayerHeuristic(const Tablut &state) {
    auto kingPosition = state.King().Position;
    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();

    int64_t value{0};
    value += 1500 * EscapeRoutes(state, kingPosition);
    value += 200 * (BOARD_SIZE - 1 - BfsDistanceToEdge(state, kingPosition));
    value += 120 * GuardsAdjacentKing(state, kingPosition);
    value += 80 * (whitePieces - blackPieces);
    value += 3 * PositionWeigthedKing(state);
    value += 2 * PositionWeigthedWhite(state);
    value -= 250 * MercenariesAdjacentKing(state, kingPosition);

    return value;
}

int64_t Heuristic::BlackPlayerHeuristic(const Tablut &state) {
    auto kingPosition = state.King().Position;
    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();

    int64_t value{0};
    value -= 1500 * (4 - EscapeRoutes(state, kingPosition));
    value -= 250 * MercenariesAdjacentKing(state, kingPosition);
    value -= 80 * (blackPieces - whitePieces);
    value -= 2 * PositionWeigthedBlack(state);

    return value;
}

int64_t Heuristic::EvaluateMove(const PieceMove &move, const Tablut &state,
                                bool isMax) {
    auto newState = state.Move(move.From, move.To);

    if (!newState.HasKing() && !isMax) {
        return std::numeric_limits<int64_t>::max();
    }

    auto newKingPos = newState.King().Position;
    if (IsInWinningPosition(newKingPos) && isMax) {
        return std::numeric_limits<int64_t>::max();
    }

    int64_t result{0};

    auto kingPosition = state.King().Position;
    if (move.From == kingPosition) {
        int before = BfsDistanceToEdge(state, kingPosition);
        int after = BfsDistanceToEdge(newState, newKingPos);

        result += (before - after) * 1000;

        uint8_t r1 = EscapeRoutes(state, kingPosition);
        uint8_t r2 = EscapeRoutes(newState, newKingPos);

        result += (int64_t)(r2 - r1) * 1000;
    }

    if (!isMax) {
        result = -result;
    }
    return result;
}