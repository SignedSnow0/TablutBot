#pragma once
#include <cstdint>

#include "state/Tablut.h"

/**
 * @brief Calculates the total number of unobstructed, straight-line paths the
 * King currently has to the edges of the board.
 */
uint8_t EscapeRoutes(const Tablut &t, const PiecePosition &king);

/**
 * @brief  how many enemy Mercenaries (Black pieces) are in the immediate
 * vicinity (3 x 3 grid centered in the king) of the King.
 */
uint8_t MercenariesAdjacentKing(const Tablut &t, const PiecePosition &king);

/**
 * @brief Counts how many Guards (White pieces) are in the immediate vicinity (3
 * x 3 grid centered in the king) of the King.
 */
uint8_t GuardsAdjacentKing(const Tablut &t, const PiecePosition &king);

/**
 * @brief Calculates the minimum number of moves required for a piece (usually
 * the King) to escape to any edge of the board.
 * @param p The position of the piece for which to calculate the distance to the
 * edge.
 * @returns The number of sequential moves to reach the edge, or 255 (the
 * maximum value for a uint8_t) if the piece is completely trapped.
 */
uint8_t BfsDistanceToEdge(const Tablut &t, const PiecePosition &p);

/**
 * @brief Evaluates the overall positional strength of the White pieces (Guards)
 * It uses a weighted sum of the positions of the pieces
 */
int64_t PositionWeigthedWhite(const Tablut &t);

/**
 * @brief Evaluates the overall positional strength of the King
 * It uses a weighted sum of the positions of the pieces
 */
int64_t PositionWeigthedKing(const Tablut &t);

/**
 * @brief Evaluates the overall positional strength of the Black pieces
 * (Mercenaries) It uses a weighted sum of the positions of the pieces
 */
int64_t PositionWeigthedBlack(const Tablut &t);
