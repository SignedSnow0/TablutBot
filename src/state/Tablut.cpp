#include "Tablut.h"
#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

constexpr uint16_t sKingMask = 1 << 15;
constexpr uint16_t sGuardMask = 1 << 14;
constexpr uint16_t sMercenaryMask = 1 << 13;

constexpr uint16_t sBlackMask = sMercenaryMask;
constexpr uint16_t sWhiteMask = sKingMask | sGuardMask;

constexpr uint16_t sCampMask = 1 << 12 | 1 << 11 | 1 << 10 | 1 << 9;

constexpr uint16_t sRowMask = 1 << 7 | 1 << 6 | 1 << 5 | 1 << 4;
constexpr uint16_t sColumnMask = 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0;
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
    mInternal |= column;
    mInternal |= row << 4;

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

bool Piece::operator<(const Piece &r) const {
    return (mInternal & sPositionMask) < (r.mInternal & sPositionMask);
}

uint8_t Piece::Row() const { return (mInternal & sRowMask) >> 4; }
uint8_t Piece::Column() const { return mInternal & sColumnMask; }
Position Piece::Position() const { return ::Position{Row(), Column()}; }

bool Piece::IsKing() const { return mInternal & sKingMask; }
bool Piece::IsGuard() const { return mInternal & sGuardMask; }
bool Piece::IsMercenary() const { return mInternal & sMercenaryMask; }
bool Piece::IsInCamp() const { return mInternal & sCampMask; }
bool Piece::IsWhite() const { return mInternal & sWhiteMask; }
bool Piece::IsBlack() const { return mInternal & sBlackMask; }

bool Piece::IsAt(uint8_t row, uint8_t column) const {
    uint16_t position = row << 4 | column;
    return (mInternal & sPositionMask) == position;
}

Piece Piece::Move(uint8_t row, uint8_t column) const {
    Piece piece;
    piece.mInternal = mInternal;

    uint16_t newPos = (row << 4) + column;
    piece.mInternal = (~sPositionMask & mInternal) | newPos;

    if (!(mInternal & sCampMask)) {
        return piece;
    }

    if (mInternal & 1 << 12) {
        if (!(((column >= 3 || column < 6) && row == 0) ||
              (row == 1 && column == 4))) {
            piece.mInternal &= ~(1 << 12);
        }
    } else if (mInternal & 1 << 11) {
        if (!(((row >= 3 || row < 6) && column == 9) ||
              (row == 4 && column == 8))) {
            piece.mInternal &= ~(1 << 11);
        }
    } else if (mInternal & 1 << 10) {
        if (!(((column >= 3 || column < 6) && row == 9) ||
              (row == 8 && column == 4))) {
            piece.mInternal &= ~(1 << 10);
        }
    } else if (mInternal & 1 << 9) {
        if (!(((row >= 3 || row < 6) && column == 0) ||
              (column == 1 && row == 4))) {
            piece.mInternal &= ~(1 << 9);
        }
    }

    return piece;
}

std::set<Piece> Tablut::WhitePieces() const {
    std::set<Piece> pieces;
    for (const auto &piece : mPieces) {
        if (piece.IsWhite()) {
            pieces.insert(piece);
        }
    }

    return pieces;
}

std::set<Piece> Tablut::BlackPieces() const {
    std::set<Piece> pieces;
    for (const auto &piece : mPieces) {
        if (piece.IsBlack()) {
            pieces.insert(piece);
        }
    }

    return pieces;
}

bool Tablut::IsEmpty(uint8_t row, uint8_t column) const {
    for (const auto &piece : mPieces) {
        if (piece.IsAt(row, column)) {
            return false;
        }
    }

    return true;
}

// NOTE: to generate camp moves, if the move is from the outside in, it is
// invalid
std::vector<Position> Tablut::GenMoves(uint8_t row, uint8_t column) const {
    const auto &piece = mPieces.find(Piece(row, column, Piece::Type::King));
    if (piece == mPieces.end()) {
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
    auto existing = mPieces.find(Piece(fromRow, fromColumn, Piece::Type::King));
    if (existing == mPieces.end()) {
        std::cerr << "Error: trying to move invalid piece from "
                  << std::to_string(fromRow) << ", "
                  << std::to_string(fromColumn) << std::endl;
        return;
    }
    auto type = Piece::Type::Mercenary;
    if (existing->IsGuard()) {
        type = Piece::Type::Guard;
    } else if (existing->IsKing()) {
        type = Piece::Type::King;
    }
    auto newPiece = existing->Move(toRow, toColumn);

    mPieces.erase(existing);
    mPieces.insert(newPiece);
}

std::string PrintPosition(const Position &position) {
    std::stringstream ss;
    ss << (char)(position.Column + 97) << std::to_string(position.Row + 1);

    return ss.str();
}

std::string PrintPiece(const Piece &cell) {
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
    ss << PrintPosition(cell.Position()) << ")";

    return ss.str();
}

std::string PrintTable(const Tablut &tablut) {
    std::stringstream ss;

    ss << "    a   b   c   d   e   f   g   h   i\n";
    ss << "  -------------------------------------";
    for (uint8_t row = 0; row < 9; row++) {
        ss << "\n" << std::to_string(row + 1) << " ";
        for (uint8_t column = 0; column < 9; column++) {
            const auto piece =
                tablut.Pieces().find(Piece(row, column, Piece::Type::King));
            if (piece != tablut.Pieces().end()) {
                if (piece->IsKing()) {
                    ss << "| K ";
                }
                if (piece->IsGuard()) {
                    ss << "| G ";
                }
                if (piece->IsMercenary()) {
                    ss << "| M ";
                }
            } else {
                ss << "|   ";
            }
        }
        ss << "|\n";
        ss << "  -------------------------------------";
    }

    return ss.str();
}
