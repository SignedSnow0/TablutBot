#pragma once
#include <cstdint>
#include <stop_token>
#include <vector>

#include "ai/ThreadPool.hpp"
#include "ai/TranspositionTable.h"
#include "state/Tablut.h"

class Minimax {
public:
    Minimax(uint64_t timeout, uint32_t maxThreads, uint32_t depth);

    int64_t Solve(const Tablut &initialState, bool isMax);

    [[nodiscard]] inline PieceMove BestMove() const { return mBestMove; }

private:
    struct SearchData {
        int64_t value;
        uint64_t cached = 0;
        uint64_t evaluated = 0;
        uint64_t interrupted = 0;
        uint64_t pruned = 0;
    };

    SearchData Solve(const Tablut &state, uint32_t depth, bool isMax,
                     int64_t alpha, int64_t beta, std::stop_token st);

    void OrderMoves(std::vector<PieceMove> &moves, const Tablut &state,
                    bool isMax);


    PieceMove mBestMove{{4, 4}, {4, 4}};
    std::chrono::milliseconds mTimeout;
    ThreadPool mThreadPool;
    uint32_t mDepth;
    TranspositionTable mTranspositionTable;

    Tablut mLastState;
};
