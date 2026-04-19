#pragma once
#include <cstdint>

#include "state/Tablut.h"

uint8_t EscapeRoutes(const Tablut &t, const PiecePosition &king);
uint8_t MercenariesAdjacentKing(const Tablut &t, const PiecePosition &king);
uint8_t GuardsAdjacentKing(const Tablut &t, const PiecePosition &king);

uint8_t BfsDistanceToEdge(const Tablut &t, const PiecePosition &p);

int64_t PositionWeigthedWhite(const Tablut &t);
int64_t PositionWeigthedKing(const Tablut &t);
int64_t PositionWeigthedBlack(const Tablut &t);
