#include "Piece.h"

constexpr uint16_t sKingMask = 1 << 15;
constexpr uint16_t sGuardMask = 1 << 14;
constexpr uint16_t sMercenaryMask = 1 << 13;

constexpr uint16_t sBlackMask = sMercenaryMask;
constexpr uint16_t sWhiteMask = sKingMask | sGuardMask;

constexpr uint16_t sCampMask = 1 << 12 | 1 << 11 | 1 << 10 | 1 << 9;

constexpr uint16_t sRowMask = 1 << 7 | 1 << 6 | 1 << 5 | 1 << 4;
constexpr uint16_t sColumnMask = 1 << 3 | 1 << 2 | 1 << 1 | 1 << 0;
constexpr uint16_t sPositionMask = sRowMask | sColumnMask;

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
