#include "Minimax.h"

#include "ai/Evaluations.h"
#include "ai/Heuristic.h"
#include "state/Tablut.h"
#include "state/Utils.h"
#include "utils/Logger.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <limits>
#include <stop_token>
#include <vector>

static constexpr int64_t TT_BEST_MOVE_BONUS = 10000000;

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
        return Heuristic::EvaluateState(initialState, isMax);
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
        auto value = Heuristic::EvaluateState(state, isMax);

        SearchData result;
        result.value = value;
        result.evaluated = 1;

        return result;
    }

    auto moves = state.GenAllMoves(isMax);
    OrderMoves(moves, state, isMax);

    if (isMax) {
        int64_t maxEval = std::numeric_limits<int64_t>::min();
        PieceMove bestMoveInPosition{}; // updated as better moves are found

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

            if (partialResult.value > maxEval) {
                maxEval = partialResult.value;
                bestMoveInPosition = {from, to};
            }

            alpha = std::max(alpha, maxEval);

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

        mTranspositionTable.Insert(state, isMax, depth, maxEval, bound,
                                   bestMoveInPosition);

        result.value = maxEval;
        return result;
    } else {
        int64_t minEval = std::numeric_limits<int64_t>::max();
        PieceMove bestMoveInPosition{}; // updated as better moves are found

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

            if (partialResult.value < minEval) {
                minEval = partialResult.value;
                bestMoveInPosition = {from, to};
            }

            beta = std::min(beta, minEval);

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

        mTranspositionTable.Insert(state, isMax, depth, minEval, bound,
                                   bestMoveInPosition);

        result.value = minEval;
        return result;
    }
}

void Minimax::OrderMoves(std::vector<PieceMove> &moves, const Tablut &state,
                         bool isMax) {
    TTEntry entry;
    bool hasTTMove = mTranspositionTable.TryGet(state, isMax, entry);

    std::sort(moves.begin(), moves.end(),
              [&state, isMax, hasTTMove, &entry](const PieceMove &a,
                                                 const PieceMove &b) {
                  int64_t scoreA = Heuristic::EvaluateMove(a, state, isMax);
                  int64_t scoreB = Heuristic::EvaluateMove(b, state, isMax);

                  if (hasTTMove && a == entry.BestMove) scoreA += TT_BEST_MOVE_BONUS;
                  if (hasTTMove && b == entry.BestMove) scoreB += TT_BEST_MOVE_BONUS;

                  return scoreA > scoreB;
              });
}