#pragma once

#include "state/Tablut.h"

bool IsInWinningPosition(const PiecePosition &position);
bool PositionInCamp(uint8_t row, uint8_t column);
bool IsInSameCamp(uint8_t pieceRow, uint8_t pieceColumn, uint8_t newRow,
                  uint8_t newColumn);
bool PositionIsUnreachable(uint8_t row, uint8_t column);
