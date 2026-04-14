#pragma once
#include <cstdint>
#include <stop_token>

#include "ai/ThreadPool.hpp"
#include "ai/TranspositionTable.h"
#include "state/Tablut.h"

class Minimax {
public:
    Minimax(uint64_t timeout, uint32_t maxThreads, uint32_t depth);

    int64_t Solve(const Tablut &initialState, bool isMax);

    [[nodiscard]] inline PieceMove BestMove() const { return mBestMove; }

private:
    int64_t Solve(const Tablut &state, uint32_t depth, bool isMax,
                  int64_t alpha, int64_t beta, std::stop_token st);

    int64_t Evaluate(const Tablut &state, bool isMax);

    PieceMove mBestMove;
    std::chrono::milliseconds mTimeout;
    ThreadPool mThreadPool;
    uint32_t mDepth;
    TranspositionTable mTranspositionTable;
};
