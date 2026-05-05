#include "Heuristic.h"
#include "ai/Evaluations.h"
#include "state/Utils.h"
#include <cstdlib>
#include <limits>

// Value assigned to winning positions, should be higher than
// any possible heuristic evaluation. It is not set to INT64_MAX
// to avoid overflow issues when negating the value for the
// opponent's winning positions.
constexpr int64_t WINNING_POSITION_VALUE = 1000000000;

// The size of the board minus 1
constexpr int BOARD_SIZE_L1 = BOARD_SIZE - 1;

int64_t Heuristic::EvaluateState(const Tablut &state, bool isMax) {
    if (!state.HasKing()) {
        return -WINNING_POSITION_VALUE;
    }

    auto kingPosition = state.King().Position;
    if (IsInWinningPosition(kingPosition)) {
        return WINNING_POSITION_VALUE;
    }

    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();
    uint8_t nEscapeRoutes = EscapeRoutes(state, kingPosition);
    uint8_t distanceToEdge = BfsDistanceToEdge(state, kingPosition);
    uint8_t nGuardsAdj = GuardsAdjacentKing(state, kingPosition);
    uint8_t nMercsAdj = MercenariesAdjacentKing(state, kingPosition);

    // score (max is white)
    int64_t score = 0;

    // Contribute to the score based on the number of pieces
    // A white piece is more valuable than a black piece, as it can help the
    // king escape or defend it, while black pieces are mostly useful for
    // surrounding the king. More, the starting position has 16 black pieces and
    // 9 white pieces
    score += (whitePieces * 200);
    score -= (blackPieces * 150);

    // Number of escape routes for the king is a critical factor, as it directly
    // relates to the king's chances of winning. Each escape route significantly
    score += nEscapeRoutes * 600;

    if (distanceToEdge != std::numeric_limits<uint8_t>::max()) {
        // The closer the king is to the edge, the better for White, as it
        // increases the chances of escaping
        score += (BOARD_SIZE - distanceToEdge) * 80;
    } else {
        // If the king is completely blocked, it's a very bad position for White
        score -= 800; // Heavily penalize White if the King is boxed in
    }

    // Guards adjacent to the king are beneficial for White, while mercenaries
    // adjacent to the king are detrimental, as they can contribute to its
    // capture.
    score += nGuardsAdj * 75;
    score -= nMercsAdj * 250;

    // Positional evaluation using piece-square tables, giving more weight to
    // the king's position as it is the most critical piece on the board.
    score += PositionWeigthedKing(state) * 2;
    score += PositionWeigthedWhite(state);
    score -= PositionWeigthedBlack(state);

    return score;
}

int64_t Heuristic::EvaluateMove(const PieceMove &move, const Tablut &state,
                                bool isMax) {

    bool isKingMove =
        state.IsType(move.From.Row, move.From.Column, PieceType::King);
    // If the move is a winning move for White (moving the King to an escape
    // position), return a very high score immediately.
    if (isKingMove && IsInWinningPosition(move.To)) {
        return WINNING_POSITION_VALUE;
    }

    // Generate the resulting state (Bitboard operations are relatively fast)
    auto newState = state.Move(move.From, move.To);
    // If the move results in a winning position for Black (capturing the King),
    // if you are white it is a very bad move, if you are black it is a very
    // good move
    if (!newState.HasKing()) {
        return isMax ? -WINNING_POSITION_VALUE : WINNING_POSITION_VALUE;
    }

    int64_t moveScore = 0;

    // Counting set bits on a bitboard is extremely fast.
    // We check if the number of pieces changed to detect a capture.
    int whitePiecesBefore = state.WhitePieces().size();
    int whitePiecesAfter = newState.WhitePieces().size();
    int blackPiecesBefore = state.BlackPieces().size();
    int blackPiecesAfter = newState.BlackPieces().size();

    int whiteCaptured = whitePiecesBefore - whitePiecesAfter;
    int blackCaptured = blackPiecesBefore - blackPiecesAfter;

    // If the move captures pieces, it is generally a good move, either for
    // Black (capturing the King or guards) or for White (capturing mercenaries
    // that threaten the King).
    // It is more important to capture white pieces
    if (isMax) {
        moveScore += blackCaptured * 1000;
    } else {
        moveScore += whiteCaptured * 1000;
    }

    // Instead of using BFS, it is better use a more direct heuristic for move
    // ordering
    if (isKingMove) {
        // If the King moved, prioritize moves that take it closer to the edges
        // (0 or 8)
        int distRow = std::min(move.To.Row, BOARD_SIZE_L1 - move.To.Row);
        int distCol = std::min(move.To.Column, BOARD_SIZE_L1 - move.To.Column);
        int distanceToEdge = std::min(distRow, distCol);

        // Lower distance to edge is better for White
        moveScore += (4 - distanceToEdge) * 100;
    } else {
        // If a standard piece moved, prioritize moving closer to the King
        // (Good for Black to attack, good for White to defend)
        auto kingPos = state.King().Position;
        int oldDistToKing = std::abs(move.From.Row - kingPos.Row) +
                            std::abs(move.From.Column - kingPos.Column);
        int newDistToKing = std::abs(move.To.Row - kingPos.Row) +
                            std::abs(move.To.Column - kingPos.Column);

        if (newDistToKing < oldDistToKing) {
            moveScore += 100;
        }
    }

    return moveScore;
}