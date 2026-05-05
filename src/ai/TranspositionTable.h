#pragma once

#include "state/Tablut.h"
#include <cstdint>

#define NUM_PIECE_TYPES 3

/**
 * @brief Represents the type of bound stored in the Transposition Table for
 * Alpha-Beta pruning.
 *
 * - Exact: The score is an exact evaluation of the node.
 * - LowerBound: A beta cutoff occurred; the true score is at least this high
 * (used for Max nodes).
 * - UpperBound: An alpha cutoff occurred; the true score is at most this high
 * (used for Min nodes).
 */
enum class BoundType { Exact, LowerBound, UpperBound };

/**
 * @brief A single entry in the transposition table.
 * Contains the cached evaluation data for a previously searched board state.
 *
 */
struct TTEntry {
    uint64_t Hash = 0; ///< The Zobrist hash of the board state. 0 denotes an
                       ///< empty entry.
    uint32_t Depth;    ///< The search depth at which this evaluation was
                       ///< computed.
    int64_t Score;     ///< The evaluated score of the board state.
    BoundType Bound; ///< The type of bound this score represents (Exact, Lower,
                     ///< or Upper).
};

/**
 * @brief Helper struct to index a state and turn.
 */
struct TTIndex {
    const Tablut &tablut; ///< Reference to the board state.
    bool isWhite;         ///< True if it's White's turn, false for Black.
};

/**
 * @brief A hash table that caches Minimax evaluations to prevent redundant
 * searches.
 *
 * It uses Zobrist Hashing to generate near-unique 64-bit keys for any given
 * board state and player turn. The table size is fixed upon initialization and
 * uses a direct-mapping replacement scheme (hash % table_size).
 */
class TranspositionTable {
public:
    /**
     * @brief Constructs the Transposition Table and initializes the Zobrist
     * keys.
     * @param size The total number of entries the table can hold.
     * @param seed The random seed used to generate Zobrist hashing keys.
     */
    TranspositionTable(uint64_t size, uint64_t seed = 123456);

    /**
     * @brief Retrieves an entry from the table based on a pre-computed hash.
     * @param hash The 64-bit Zobrist hash.
     * @return Reference to the TTEntry at the mapped index.
     */
    TTEntry &Get(uint64_t hash) { return mTable[hash % mTableSize]; }
    const TTEntry &Get(uint64_t hash) const {
        return mTable[hash % mTableSize];
    }

    /**
     * @brief Computes the hash for the state and retrieves its corresponding
     * entry.
     * @param state The current Tablut board state.
     * @param isWhite True if it is White's turn.
     * @return Reference to the TTEntry at the mapped index.
     */
    TTEntry &Get(const Tablut &state, bool isWhite) {
        return Get(ComputeHash(state, isWhite));
    }
    const TTEntry &Get(const Tablut &state, bool isWhite) const {
        return Get(ComputeHash(state, isWhite));
    }

    /**
     * @brief Attempts to retrieve a valid entry from the table using a
     * pre-computed hash.
     * @param hash The 64-bit Zobrist hash.
     * @param entry [out] Reference to populate if the entry is found.
     * @return True if a valid entry exists at this index, false if it is empty.
     */
    bool TryGet(uint64_t hash, TTEntry &entry) {
        auto &item = mTable[hash % mTableSize];
        if (item.Hash == 0) {
            return false;
        }

        entry = item;
        return true;
    }

    /**
     * @brief Attempts to retrieve a valid entry from the table using the board
     * state.
     * @param state The current Tablut board state.
     * @param isWhite True if it is White's turn.
     * @param entry [out] Reference to populate if the entry is found.
     * @return True if a valid entry exists at this index, false if it is empty.
     */
    bool TryGet(const Tablut &state, bool isWhite, TTEntry &entry) {
        auto hash = ComputeHash(state, isWhite);
        auto &item = mTable[hash % mTableSize];
        if (item.Hash == 0) {
            return false;
        }

        entry = item;
        return true;
    }

    /**
     * @brief Inserts or overwrites an entry in the transposition table.
     * @param hash The pre-computed 64-bit Zobrist hash.
     * @param isWhite True if it is White's turn.
     * @param depth The search depth used to achieve this score.
     * @param score The evaluated score.
     * @param boundType The type of bound (Exact, LowerBound, UpperBound).
     */
    void Insert(uint64_t hash, bool isWhite, uint32_t depth, int64_t score,
                BoundType boundType) {
        auto &item = mTable[hash % mTableSize];
        item.Hash = hash;
        item.Score = score;
        item.Depth = depth;
        item.Bound = boundType;
    }

    /**
     * @brief Computes the hash and inserts an entry into the transposition
     * table.
     * @param state The current Tablut board state.
     * @param isWhite True if it is White's turn.
     * @param depth The search depth used to achieve this score.
     * @param score The evaluated score.
     * @param boundType The type of bound (Exact, LowerBound, UpperBound).
     */
    void Insert(const Tablut &state, bool isWhite, uint32_t depth,
                int64_t score, BoundType boundType) {
        Insert(ComputeHash(state, isWhite), isWhite, depth, score, boundType);
    }

    /**
     * @brief Computes the Zobrist hash of a given board state and turn.
     * @param state The Tablut board state to hash.
     * @param isWhite True if it is White's turn.
     * @return A 64-bit hash representing the exact configuration and whose turn
     * it is.
     */
    uint64_t ComputeHash(const Tablut &state, bool isWhite) const;

    /**
     * @brief Checks if a given hash currently exists (is valid) in the table.
     * @param hash The Zobrist hash to check.
     * @return True if the entry has a non-zero hash, false otherwise.
     */
    bool Contains(uint64_t hash) const {
        return mTable[hash % mTableSize].Hash != 0;
    }

    /**
     * @brief Checks if the given state and turn currently exist in the table.
     * @param tablut The board state.
     * @param isWhite True if it's White's turn.
     * @return True if the corresponding entry has a non-zero hash, false
     * otherwise.
     */
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
