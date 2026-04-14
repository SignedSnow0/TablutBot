#pragma once

#include <cstdint>

#include "state/Tablut.h"

#define NUM_PIECE_TYPES 3

enum class BoundType { Exact, LowerBound, UpperBound };

struct TTEntry {
    uint64_t Hash = 0;
    uint32_t Depth;
    int64_t Score;
    BoundType Bound;
};

struct TTIndex {
    const Tablut &tablut;
    bool isWhite;
};

class TranspositionTable {
public:
    TranspositionTable(uint64_t size, uint64_t seed = 123456);

    TTEntry &Get(uint64_t hash) { return mTable[hash % mTableSize]; }
    const TTEntry &Get(uint64_t hash) const {
        return mTable[hash % mTableSize];
    }
    TTEntry &Get(const Tablut &state, bool isWhite) {
        return Get(ComputeHash(state, isWhite));
    }
    const TTEntry &Get(const Tablut &state, bool isWhite) const {
        return Get(ComputeHash(state, isWhite));
    }
    bool TryGet(uint64_t hash, TTEntry &entry) {
        auto &item = mTable[hash % mTableSize];
        if (item.Hash == 0) {
            return false;
        }

        entry = item;
        return true;
    }
    bool TryGet(const Tablut &state, bool isWhite, TTEntry &entry) {
        auto hash = ComputeHash(state, isWhite);
        auto &item = mTable[hash % mTableSize];
        if (item.Hash == 0) {
            return false;
        }

        entry = item;
        return true;
    }

    void Insert(uint64_t hash, bool isWhite, uint32_t depth, int64_t score,
                BoundType boundType) {
        auto &item = mTable[hash % mTableSize];
        item.Hash = hash;
        item.Score = score;
        item.Depth = depth;
        item.Bound = boundType;
    }

    void Insert(const Tablut &state, bool isWhite, uint32_t depth,
                int64_t score, BoundType boundType) {
        Insert(ComputeHash(state, isWhite), isWhite, depth, score, boundType);
    }

    uint64_t ComputeHash(const Tablut &state, bool isWhite) const;

    bool Contains(uint64_t hash) const {
        return mTable[hash % mTableSize].Hash != 0;
    }
    bool Contains(const Tablut &tablut, bool isWhite) {
        return Contains(ComputeHash(tablut, isWhite));
    }

private:
    void InitZobrist(uint64_t seed);

    uint64_t mZobrist[BOARD_SIZE][BOARD_SIZE][NUM_PIECE_TYPES];
    uint64_t mZobristTurn;

    uint64_t mTableSize;
    std::vector<TTEntry> mTable;
};
