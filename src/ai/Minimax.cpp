#include "Minimax.h"

#include "ai/Evaluations.h"
#include "state/Tablut.h"
#include "utils/Logger.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stop_token>

struct SearchResult {
    std::atomic<int64_t> value;
    std::atomic<uint32_t> index;
    std::atomic<uint32_t> finished;
};

bool isInWinningPosition(const PiecePosition &position) {
    if ((position.Row == 0 || position.Row == 8) &&
        (position.Column == 1 || position.Column == 2 || position.Column == 6 ||
         position.Column == 7)) {
        return true;
    }

    if ((position.Column == 0 || position.Column == 8) &&
        (position.Row == 1 || position.Row == 2 || position.Row == 6 ||
         position.Row == 7)) {
        return true;
    }

    return false;
}

Minimax::Minimax(uint64_t timeout, uint32_t maxThreads, uint32_t depth)
    : mTimeout(std::chrono::milliseconds(timeout)), mThreadPool(maxThreads),
      mDepth(depth) {}

int64_t Minimax::Solve(const Tablut &initialState, bool isMax) {
    const auto moves = initialState.GenAllMoves(isMax);
    if (moves.empty()) {
        return Evaluate(initialState, isMax);
    }

    SearchResult best;
    best.value = isMax ? std::numeric_limits<int64_t>::min()
                       : std::numeric_limits<int64_t>::max();
    best.index = 0;
    best.finished = 0;

    for (uint32_t i = 0; i < moves.size(); i++) {
        const auto [from, to] = moves[i];

        mThreadPool.Submit([&, i, from, to, this](std::stop_token st) {
            auto state = initialState.Move(from, to);

            int64_t value = Solve(state, mDepth - 1, !isMax,
                                  std::numeric_limits<int64_t>::min(),
                                  std::numeric_limits<int64_t>::max(), st);

            if (st.stop_requested()) {
                return;
            }

            best.finished++;

            if (isMax) {
                int64_t current = best.value.load();

                while (value > current &&
                       !best.value.compare_exchange_weak(current, value)) {
                }

                if (best.value.load() == value) {
                    best.index.store(i);
                }
            } else {
                int64_t current = best.value.load();

                while (value < current &&
                       !best.value.compare_exchange_weak(current, value)) {
                }

                if (best.value.load() == value) {
                    best.index.store(i);
                }
            }
        });
    }

    mThreadPool.RunFor(mTimeout);

    // std::this_thread:sleep_for(std::chrono::milliseconds(10));

    if (best.finished == 0) {
        LOG_WARNING("The search could not complete any branch in time, "
                    "decreasing depth");
        mDepth--;
    } else if (best.finished == moves.size()) {
        LOG_WARNING("The search terminated before the timeout");
    }
    LOG_INFO("Search terminated, completed {}/{} branches",
             best.finished.load(), moves.size());

    mBestMove = moves[best.index.load()];
    return best.value.load();
}

int64_t Minimax::Solve(const Tablut &state, uint32_t depth, bool isMax,
                       int64_t alpha, int64_t beta, std::stop_token st) {
    if (st.stop_requested()) {
        return isMax ? std::numeric_limits<int64_t>::min()
                     : std::numeric_limits<int64_t>::max();
    }

    if (depth == 0) {
        return Evaluate(state, isMax);
    }

    const auto moves = state.GenAllMoves(isMax);

    if (isMax) {
        int64_t maxEval = std::numeric_limits<int64_t>::min();

        for (const auto &[from, to] : moves) {
            if (st.stop_requested()) {
                break;
            }

            auto newState = state.Move(from, to);
            int64_t eval = Solve(newState, depth - 1, false, alpha, beta, st);

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);

            if (beta <= alpha) {
                break;
            }
        }

        return maxEval;
    } else {
        int64_t minEval = std::numeric_limits<int64_t>::max();

        for (const auto &[from, to] : moves) {
            if (st.stop_requested()) {
                break;
            }

            auto newState = state.Move(from, to);
            int64_t eval = Solve(newState, depth - 1, true, alpha, beta, st);

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);

            if (beta <= alpha) {
                break;
            }
        }

        return minEval;
    }
}

int64_t Minimax::Evaluate(const Tablut &state, bool isMax) {
    if (!state.HasKing()) {
        return std::numeric_limits<int64_t>::min();
    }

    auto kingPosition = state.King().Position;
    if (isInWinningPosition(kingPosition)) {
        return std::numeric_limits<int64_t>::max();
    }

    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();
    if (isMax) {
        int64_t value{0};
        value += 1500 * EscapeRoutes(state, kingPosition);
        value += 400 * CapturesNextMove(state, isMax);
        value +=
            200 * (BOARD_SIZE - 1 - BfsDistanceToEdge(state, kingPosition));
        value += 120 * GuardsAdjacentKing(state, kingPosition);
        value += 80 * (whitePieces - blackPieces);
        value += 3 * PositionWeigthedKing(state);
        value += 2 * PositionWeigthedWhite(state);

        value -= 250 * MercenariesAdjacentKing(state, kingPosition);

        return value;
    } else {
        int64_t value{0};
        value -= 1500 * (4 - EscapeRoutes(state, kingPosition));
        value -= 400 * CapturesNextMove(state, isMax);
        value -= 250 * MercenariesAdjacentKing(state, kingPosition);
        value -= 80 * (blackPieces - whitePieces);
        value -= 2 * PositionWeigthedBlack(state);

        return value;
    }
}
