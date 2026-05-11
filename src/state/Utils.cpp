#include "Utils.h"

bool positionInTopCamp(uint8_t row, uint8_t column) {
    if (row == 0 && (column >= 3 && column < 6)) {
        return true;
    }
    if (row == 1 && column == 4) {
        return true;
    }

    return false;
}

bool positionInRightCamp(uint8_t row, uint8_t column) {
    if (column == 0 && (row >= 3 && column < 6)) {
        return true;
    }
    if (row == 4 && column == 7) {
        return true;
    }

    return false;
}

bool positionInBottomCamp(uint8_t row, uint8_t column) {
    if (row == 8 && (column >= 3 && column < 6)) {
        return true;
    }
    if (row == 7 & column == 4) {
        return true;
    }

    return false;
}

bool positionInLeftCamp(uint8_t row, uint8_t column) {
    if (column == 8 && (row >= 3 && column < 6)) {
        return true;
    }
    if (row == 4 && column == 1) {
        return true;
    }

    return false;
}

bool IsInSameCamp(uint8_t pieceRow, uint8_t pieceColumn, uint8_t newRow,
                  uint8_t newColumn) {
    if (!PositionInCamp(newRow, newColumn)) {
        return true;
    }

    if (positionInTopCamp(pieceRow, pieceColumn) &&
        positionInTopCamp(newRow, newColumn)) {
        return true;
    }
    if (positionInRightCamp(pieceRow, pieceColumn) &&
        positionInRightCamp(newRow, newColumn)) {
        return true;
    }
    if (positionInBottomCamp(pieceRow, pieceColumn) &&
        positionInBottomCamp(newRow, newColumn)) {
        return true;
    }
    if (positionInLeftCamp(pieceRow, pieceColumn) &&
        positionInLeftCamp(newRow, newColumn)) {
        return true;
    }

    return false;
}

bool IsInWinningPosition(const PiecePosition &position) {
    if ((position.Row == 0 || position.Row == 8) &&
        (position.Column == 1 || position.Column == 2 || position.Column == 6 ||
         position.Column == 7)) {
        return true;
    }

    if ((position.Column == 0 || position.Column == 8) &&
        (position.Row == 1 || position.Row == 2 || position.Row == 6 ||
         position.Row == 7)) {
        return true;
    }

    return false;
}

bool PositionInCamp(uint8_t row, uint8_t column) {
    return positionInTopCamp(row, column) || positionInRightCamp(row, column) ||
           positionInBottomCamp(row, column) || positionInLeftCamp(row, column);
}

bool PositionIsUnreachable(uint8_t row, uint8_t column) {
    return PositionInCamp(row, column) || (row == 4 && column == 4);
}
