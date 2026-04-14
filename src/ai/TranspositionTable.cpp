#include "TranspositionTable.h"

#include <cstdint>
#include <random>

TranspositionTable::TranspositionTable(uint64_t size, uint64_t seed)
    : mTableSize(size) {
    InitZobrist(seed);

    mTable.resize(size);
}

void TranspositionTable::InitZobrist(uint64_t seed) {
    std::mt19937_64 rng(seed);

    for (uint32_t row = 0; row < BOARD_SIZE; row++) {
        for (uint32_t column = 0; column < BOARD_SIZE; column++) {
            for (uint32_t piece = 0; piece < NUM_PIECE_TYPES; piece++) {
                mZobrist[row][column][piece] = rng();
            }
        }
    }

    mZobristTurn = rng();
}

uint64_t TranspositionTable::ComputeHash(const Tablut &state,
                                         bool isWhite) const {
    uint64_t hash = 0;

    for (uint8_t row = 0; row < BOARD_SIZE; row++) {
        for (uint8_t column = 0; column < BOARD_SIZE; column++) {
            if (state.IsType(row, column, PieceType::Guard)) {
                hash ^= mZobrist[row][column]
                                [static_cast<uint64_t>(PieceType::Guard)];
            } else if (state.IsType(row, column, PieceType::Mercenary)) {
                hash ^= mZobrist[row][column]
                                [static_cast<uint64_t>(PieceType::Mercenary)];
            } else if (state.IsType(row, column, PieceType::King)) {
                hash ^= mZobrist[row][column]
                                [static_cast<uint64_t>(PieceType::King)];
            }
        }
    }

    if (!isWhite) {
        hash ^= mZobristTurn;
    }

    return hash;
}
