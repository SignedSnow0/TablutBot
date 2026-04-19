#include "Minimax.h"

#include "ai/Evaluations.h"
#include "state/Tablut.h"
#include "state/Utils.h"
#include "utils/Logger.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stop_token>
#include <vector>

struct SearchResult {
    std::atomic<int64_t> value;
    std::atomic<uint32_t> index;
    std::atomic<uint32_t> finished;

    std::atomic<uint64_t> cached;
    std::atomic<uint64_t> evaluated;
    std::atomic<uint64_t> interrupted;
    std::atomic<uint64_t> pruned;
};

Minimax::Minimax(uint64_t timeout, uint32_t maxThreads, uint32_t depth)
    : mTimeout(std::chrono::milliseconds(timeout)), mThreadPool(maxThreads),
      mDepth(depth), mTranspositionTable(1 << 22),
      mLastState(Tablut::InitialConfiguration()) {}

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

            SearchData result = Solve(state, mDepth - 1, !isMax,
                                      std::numeric_limits<int64_t>::min(),
                                      std::numeric_limits<int64_t>::max(), st);
            best.pruned.fetch_add(result.pruned, std::memory_order_relaxed);
            best.interrupted.fetch_add(result.interrupted,
                                       std::memory_order_relaxed);
            best.evaluated.fetch_add(result.evaluated,
                                     std::memory_order_relaxed);
            best.cached.fetch_add(result.cached, std::memory_order_relaxed);

            if (st.stop_requested()) {
                return;
            }

            best.finished++;

            if (isMax) {
                int64_t current = best.value.load();

                while (
                    result.value > current &&
                    !best.value.compare_exchange_weak(current, result.value)) {
                }

                if (best.value.load() == result.value) {
                    best.index.store(i);
                }
            } else {
                int64_t current = best.value.load();

                while (
                    result.value < current &&
                    !best.value.compare_exchange_weak(current, result.value)) {
                }

                if (best.value.load() == result.value) {
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
        LOG_WARNING(
            "The search terminated before the timeout, increasing depth");
        mDepth++;
    }
    LOG_INFO("Search terminated, completed {}/{} branches, {} evaluated, {} "
             "cached, {} pruned, {} interrupted",
             best.finished.load(), moves.size(), best.evaluated.load(),
             best.cached.load(), best.pruned.load(), best.interrupted.load());

    mLastState = mLastState.Move(mBestMove.From, mBestMove.To);
    mBestMove = moves[best.index.load()];

    return best.value.load();
}

Minimax::SearchData Minimax::Solve(const Tablut &state, uint32_t depth,
                                   bool isMax, int64_t alpha, int64_t beta,
                                   std::stop_token st) {
    const auto originalAlpha = alpha;

    TTEntry entry;
    if (mTranspositionTable.TryGet(state, isMax, entry)) {
        if (entry.Depth >= depth) {
            if (entry.Bound == BoundType::Exact) {
                auto value = entry.Score;
                SearchData result;
                result.value = value;
                result.cached = 1;

                return result;
            }

            if (entry.Bound == BoundType::LowerBound) {
                alpha = std::max(alpha, entry.Score);
            }

            if (entry.Bound == BoundType::UpperBound) {
                beta = std::min(beta, entry.Score);
            }

            if (alpha >= beta) {
                auto value = entry.Score;
                SearchData result;
                result.value = value;
                result.cached = 1;

                return result;
            }
        }
    }

    if (st.stop_requested()) {
        auto value = isMax ? std::numeric_limits<int64_t>::min()
                           : std::numeric_limits<int64_t>::max();
        SearchData result;
        result.value = value;
        result.interrupted = 1;

        return result;
    }

    if (depth == 0) {
        auto value = Evaluate(state, isMax);
        SearchData result;
        result.value = value;
        result.evaluated = 1;

        return result;
    }

    auto moves = state.GenAllMoves(isMax);
    OrderMoves(moves, state, isMax);

    if (isMax) {
        int64_t maxEval = std::numeric_limits<int64_t>::min();

        SearchData result;
        for (const auto &[from, to] : moves) {
            if (st.stop_requested()) {
                break;
            }

            auto newState = state.Move(from, to);

            if (newState == mLastState) {
                SearchData partialResult;
                partialResult.evaluated = 1;
                partialResult.value = 0;

                return partialResult;
            }

            SearchData partialResult =
                Solve(newState, depth - 1, false, alpha, beta, st);
            result.pruned += partialResult.pruned;
            result.cached += partialResult.cached;
            result.evaluated += partialResult.evaluated;
            result.interrupted += partialResult.interrupted;

            maxEval = std::max(maxEval, partialResult.value);
            alpha = std::max(alpha, partialResult.value);

            if (beta <= alpha) {
                result.pruned++;
                break;
            }
        }

        BoundType bound;
        if (maxEval <= originalAlpha) {
            bound = BoundType::UpperBound;
        } else if (maxEval >= beta) {
            bound = BoundType::LowerBound;
        } else {
            bound = BoundType::Exact;
        }

        mTranspositionTable.Insert(state, isMax, depth, maxEval, bound);

        result.value = maxEval;
        return result;
    } else {
        int64_t minEval = std::numeric_limits<int64_t>::max();

        SearchData result;
        for (const auto &[from, to] : moves) {
            if (st.stop_requested()) {
                break;
            }

            auto newState = state.Move(from, to);

            if (newState == mLastState) {
                SearchData partialResult;
                partialResult.evaluated = 1;
                partialResult.value = 0;

                return partialResult;
            }

            SearchData partialResult =
                Solve(newState, depth - 1, true, alpha, beta, st);
            result.pruned += partialResult.pruned;
            result.cached += partialResult.cached;
            result.evaluated += partialResult.evaluated;
            result.interrupted += partialResult.interrupted;

            minEval = std::min(minEval, partialResult.value);
            beta = std::min(beta, partialResult.value);

            if (beta <= alpha) {
                result.pruned++;
                break;
            }
        }

        BoundType bound;
        if (minEval <= originalAlpha) {
            bound = BoundType::UpperBound;
        } else if (minEval >= beta) {
            bound = BoundType::LowerBound;
        } else {
            bound = BoundType::Exact;
        }

        mTranspositionTable.Insert(state, isMax, depth, minEval, bound);

        result.value = minEval;
        return result;
    }
}

int64_t Minimax::Evaluate(const Tablut &state, bool isMax) {
    if (!state.HasKing()) {
        return std::numeric_limits<int64_t>::min();
    }

    auto kingPosition = state.King().Position;
    if (IsInWinningPosition(kingPosition)) {
        return std::numeric_limits<int64_t>::max();
    }

    int64_t whitePieces = state.WhitePieces().size();
    int64_t blackPieces = state.BlackPieces().size();
    if (isMax) {
        int64_t value{0};
        value += 1500 * EscapeRoutes(state, kingPosition);
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
        value -= 250 * MercenariesAdjacentKing(state, kingPosition);
        value -= 80 * (blackPieces - whitePieces);
        value -= 2 * PositionWeigthedBlack(state);

        return value;
    }
}

void Minimax::OrderMoves(std::vector<PieceMove> &moves, const Tablut &state,
                         bool isMax) {
    for (int i = 0; i < moves.size(); i++) {
        int best = i;
        int64_t bestScore = EvaluateMove(moves[i], state, isMax);

        for (int j = i + 1; j < moves.size(); j++) {
            int64_t s = EvaluateMove(moves[j], state, isMax);

            if (s > bestScore) {
                bestScore = s;
                best = j;
            }
        }

        std::swap(moves[i], moves[best]);
    }
}

int64_t Minimax::EvaluateMove(const PieceMove &move, const Tablut &state,
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
