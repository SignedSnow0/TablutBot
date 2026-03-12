#include "Tablut.h"
#include <cstdint>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <vector>

constexpr uint16_t sKingMask = 1 << 15;
constexpr uint16_t sGuardMask = 1 << 14;
constexpr uint16_t sMercenaryMask = 1 << 13;

constexpr uint16_t sCampMask = 1 << 12 | 1 << 11 | 1 << 10 | 1 << 9;

constexpr uint16_t sRowMask = 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0;
constexpr uint16_t sColumnMask = 1 << 7 | 1 << 6 | 1 << 5 | 1 << 4;
constexpr uint16_t sPositionMask = sRowMask | sColumnMask;

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

bool isInSameCamp(const Piece &piece, uint8_t newRow, uint8_t newColumn) {
    if (!positionInTopCamp(newRow, newColumn) &&
        !positionInRightCamp(newRow, newColumn) &&
        !positionInBottomCamp(newRow, newColumn) &&
        !positionInLeftCamp(newRow, newColumn)) {
        return true;
    }
    if (!piece.IsInCamp()) {
        return false;
    }

    uint8_t pieceRow = piece.Row();
    uint8_t pieceColumn = piece.Column();
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

Piece::Piece(uint8_t row, uint8_t column, Type type, Camp camp) {
    mInternal = 0;
    mInternal |= row;
    mInternal |= column << 4;

    if (type == Type::King) {
        mInternal |= sKingMask;
    } else if (type == Type::Guard) {
        mInternal |= sGuardMask;
    } else if (type == Type::Mercenary) {
        mInternal |= sMercenaryMask;

        if (camp == Camp::Top) {
            mInternal |= 1 << 12;
        } else if (camp == Camp::Right) {
            mInternal |= 1 << 11;
        } else if (camp == Camp::Bottom) {
            mInternal |= 1 << 10;
        } else if (camp == Camp::Left) {
            mInternal |= 1 << 9;
        }
    }
}

uint8_t Piece::Row() const { return mInternal & sRowMask; }
uint8_t Piece::Column() const { return (mInternal & sColumnMask) >> 4; }

bool Piece::IsKing() const { return mInternal & sKingMask; }
bool Piece::IsGuard() const { return mInternal & sGuardMask; }
bool Piece::IsMercenary() const { return mInternal & sMercenaryMask; }
bool Piece::IsInCamp() const { return mInternal & sCampMask; }

bool Piece::IsAt(uint8_t row, uint8_t column) const {
    uint16_t position = row | column << 4;
    return (mInternal & sPositionMask) == position;
}

void Piece::Move(uint8_t row, uint8_t column) {
    uint16_t newPos = row + (column << 4);
    mInternal &= ~sPositionMask | newPos;

    if (mInternal & 1 << 12) {
        if (!(((column >= 3 || column < 6) && row == 0) ||
              (row == 1 && column == 4))) {
            mInternal &= ~(1 << 12);
        }
    } else if (mInternal & 1 << 11) {
        if (!(((row >= 3 || row < 6) && column == 9) ||
              (row == 4 && column == 8))) {
            mInternal &= ~(1 << 11);
        }
    } else if (mInternal & 1 << 10) {
        if (!(((column >= 3 || column < 6) && row == 9) ||
              (row == 8 && column == 4))) {
            mInternal &= ~(1 << 10);
        }
    } else if (mInternal & 1 << 9) {
        if (!(((row >= 3 || row < 6) && column == 0) ||
              (column == 1 && row == 4))) {
            mInternal &= ~(1 << 9);
        }
    }
}

bool Tablut::IsEmpty(uint8_t row, uint8_t column) const {
    for (const auto &piece : mPieces) {
        if (piece.IsAt(row, column)) {
            return false;
        }
    }

    return true;
}

std::optional<Piece> Tablut::PieceAt(uint8_t row, uint8_t column) const {
    for (const auto &piece : mPieces) {
        if (piece.IsAt(row, column)) {
            return piece;
        }
    }

    return {};
}

std::vector<Position> Tablut::GenMoves(uint8_t row, uint8_t column) const {
    const auto &piece = PieceAt(row, column);
    if (!piece) {
        return {};
    }

    auto moves = std::vector<Position>();
    // NOTE: Insert in reverse order to generate sorted array
    for (int8_t r = row - 1; r >= 0; r--) {
        if (IsEmpty(r, column) && (r != 4 || column != 4) &&
            isInSameCamp(*piece, static_cast<uint8_t>(r), column)) {
            moves.push_back({static_cast<uint8_t>(r), column});
        } else {
            break;
        }
    }

    // NOTE: Insert in reverse order to generate sorted array
    for (int8_t c = column - 1; c >= 0; c--) {
        if (IsEmpty(row, c) && (row != 4 || c != 4) &&
            isInSameCamp(*piece, row, static_cast<uint8_t>(c))) {
            moves.push_back({row, static_cast<uint8_t>(c)});
        } else {
            break;
        }
    }

    for (uint8_t c = column + 1; c < 9; c++) {
        if (IsEmpty(row, c) && (row != 4 || c != 4) &&
            isInSameCamp(*piece, row, c)) {
            moves.push_back({row, c});
        } else {
            break;
        }
    }

    for (uint8_t r = row + 1; r < 9; r++) {
        if (IsEmpty(r, column) && (r != 4 || column != 4) &&
            isInSameCamp(*piece, r, column)) {
            moves.push_back({r, column});
        } else {
            break;
        }
    }

    return moves;
}

void Tablut::Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
                  uint8_t toColumn) {
    for (auto &piece : mPieces) {
        if (piece.IsAt(fromRow, fromColumn)) {
            auto type = Piece::Type::Mercenary;
            if (piece.IsGuard()) {
                type = Piece::Type::Guard;
            } else if (piece.IsKing()) {
                type = Piece::Type::King;
            }

            piece.Move(toRow, toColumn);
            return;
        }
    }
}

std::string PrintCell(const Piece &cell) {
    std::stringstream ss;
    ss << "Cell(";
    if (cell.IsKing()) {
        ss << "King, ";
    }
    if (cell.IsGuard()) {
        ss << "Guard, ";
    }
    if (cell.IsMercenary()) {
        ss << "Mercenary ";
        if (cell.IsInCamp()) {
            ss << "(in camp), ";
        }
    }

    ss << std::to_string(cell.Row()) << ", " << std::to_string(cell.Column())
       << ")";

    return ss.str();
}

std::string PrintGrid(const Tablut &tablut) {
    std::stringstream ss;

    ss << "-------------------------------------";
    for (uint8_t row = 0; row < 9; row++) {
        ss << "\n";
        for (uint8_t column = 0; column < 9; column++) {
            auto cell = tablut.PieceAt(row, column);
            if (cell) {
                if (cell->IsKing()) {
                    ss << "| K ";
                }
                if (cell->IsGuard()) {
                    ss << "| G ";
                }
                if (cell->IsMercenary()) {
                    ss << "| M ";
                }
            } else {
                ss << "|   ";
            }
        }
        ss << "|\n";
        ss << "-------------------------------------";
    }

    return ss.str();
}
