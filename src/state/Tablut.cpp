#include "Tablut.h"

#include "state/Utils.h"
#include "utils/Logger.h"

#include <cstdint>
#include <sstream>
#include <string>

bool Tablut::operator==(const Tablut &r) const {
    return mWhiteBoard == r.mWhiteBoard && mBlackBoard == r.mBlackBoard &&
           mKingBoard == r.mKingBoard;
}

std::vector<Piece> Tablut::WhitePieces() const {
    std::vector<Piece> pieces;
    for (uint8_t row{0}; row < BOARD_SIZE; row++) {
        for (uint8_t column{0}; column < BOARD_SIZE; column++) {
            if (mWhiteBoard.test(row * BOARD_SIZE + column)) {
                pieces.push_back(Piece({row, column}, PieceType::Guard));
            } else if (mKingBoard.test(row * BOARD_SIZE + column)) {
                pieces.push_back(Piece({row, column}, PieceType::King));
            }
        }
    }

    return pieces;
}

std::vector<Piece> Tablut::BlackPieces() const {
    std::vector<Piece> pieces;
    for (uint8_t row{0}; row < BOARD_SIZE; row++) {
        for (uint8_t column{0}; column < BOARD_SIZE; column++) {
            if (mBlackBoard.test(row * BOARD_SIZE + column)) {
                pieces.push_back(Piece({row, column}, PieceType::Guard));
            }
        }
    }

    return pieces;
}

bool Tablut::HasKing() const { return mKingBoard.any(); }

Piece Tablut::King() const {
    for (uint8_t row{0}; row < BOARD_SIZE; row++) {
        for (uint8_t column{0}; column < BOARD_SIZE; column++) {
            if (mKingBoard.test(row * BOARD_SIZE + column)) {
                return Piece({row, column}, PieceType::King);
            }
        }
    }

    return Piece({255, 255}, PieceType::King);
}

bool Tablut::IsEmpty(uint8_t row, uint8_t column) const {
    if (row >= BOARD_SIZE || column >= BOARD_SIZE) {
        return false;
    }

    return !mWhiteBoard.test(row * BOARD_SIZE + column) &&
           !mBlackBoard.test(row * BOARD_SIZE + column) &&
           !mKingBoard.test(row * BOARD_SIZE + column);
}

bool Tablut::IsType(uint8_t row, uint8_t column, PieceType type) const {
    if (type == PieceType::King && mKingBoard.test(row * BOARD_SIZE + column)) {
        return true;
    }
    if (type == PieceType::Guard &&
        mWhiteBoard.test(row * BOARD_SIZE + column)) {
        return true;
    }
    if (type == PieceType::Mercenary &&
        mBlackBoard.test(row * BOARD_SIZE + column)) {
        return true;
    }

    return false;
}

// NOTE: to generate camp moves, check only if the move is from the outside in,
// then it is invalid
std::vector<PiecePosition> Tablut::GenMoves(uint8_t row, uint8_t column) const {
    if (IsEmpty(row, column)) {
        LOG_WARNING("Generating moves for empty position");
        return {};
    }

    auto moves = std::vector<PiecePosition>();
    // NOTE: Insert in reverse order to generate sorted array
    for (int8_t r = row - 1; r >= 0; r--) {
        if (IsEmpty(r, column) && (r != 4 || column != 4) &&
            IsInSameCamp(row, column, static_cast<uint8_t>(r), column)) {
            moves.push_back({static_cast<uint8_t>(r), column});
        } else {
            break;
        }
    }

    // NOTE: Insert in reverse order to generate sorted array
    for (int8_t c = column - 1; c >= 0; c--) {
        if (IsEmpty(row, c) && (row != 4 || c != 4) &&
            IsInSameCamp(row, column, row, static_cast<uint8_t>(c))) {
            moves.push_back({row, static_cast<uint8_t>(c)});
        } else {
            break;
        }
    }

    for (uint8_t c = column + 1; c < 9; c++) {
        if (IsEmpty(row, c) && (row != 4 || c != 4) &&
            IsInSameCamp(row, column, row, c)) {
            moves.push_back({row, c});
        } else {
            break;
        }
    }

    for (uint8_t r = row + 1; r < 9; r++) {
        if (IsEmpty(r, column) && (r != 4 || column != 4) &&
            IsInSameCamp(row, column, r, column)) {
            moves.push_back({r, column});
        } else {
            break;
        }
    }

    return moves;
}

std::vector<PieceMove> Tablut::GenAllMoves(bool white) const {
    std::vector<PieceMove> moves;
    for (const auto &piece : white ? WhitePieces() : BlackPieces()) {
        for (const auto &move :
             GenMoves(piece.Position.Row, piece.Position.Column)) {
            moves.push_back({piece.Position, move});
        }
    }

    return moves;
}

Tablut Tablut::Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
                    uint8_t toColumn) const {
    Tablut moved = Tablut{mBlackBoard, mWhiteBoard, mKingBoard};
    if (IsEmpty(fromRow, fromColumn)) {
        LOG_WARNING("Trying to move from empty position");
        return moved;
    }
    if (!IsEmpty(toRow, toColumn)) {
        LOG_WARNING("Trying to move from empty position");
        return moved;
    }

    if (mWhiteBoard.test(fromRow * BOARD_SIZE + fromColumn)) {
        moved.mWhiteBoard.reset(fromRow * BOARD_SIZE + fromColumn);
        moved.mWhiteBoard.set(toRow * BOARD_SIZE + toColumn);

        moved.CheckCapture(toRow, toColumn, true);
    }
    if (mBlackBoard.test(fromRow * BOARD_SIZE + fromColumn)) {
        moved.mBlackBoard.reset(fromRow * BOARD_SIZE + fromColumn);
        moved.mBlackBoard.set(toRow * BOARD_SIZE + toColumn);

        moved.CheckCapture(toRow, toColumn, false);
    }
    if (mKingBoard.test(fromRow * BOARD_SIZE + fromColumn)) {
        moved.mKingBoard.reset(fromRow * BOARD_SIZE + fromColumn);
        moved.mKingBoard.set(toRow * BOARD_SIZE + toColumn);

        moved.CheckCapture(toRow, toColumn, true);
    }

    return moved;
}

void Tablut::InsertWhite(uint8_t row, uint8_t column) {
    mWhiteBoard.set(row * BOARD_SIZE + column);
}

void Tablut::InsertBlack(uint8_t row, uint8_t column) {
    mBlackBoard.set(row * BOARD_SIZE + column);
}

void Tablut::InsertKing(uint8_t row, uint8_t column) {
    mKingBoard.set(row * BOARD_SIZE + column);
}

bool Tablut::PositionIsColor(uint8_t row, uint8_t column, bool white) {
    if (row >= BOARD_SIZE || column >= BOARD_SIZE) {
        return false;
    }

    if (IsEmpty(row, column)) {
        return false;
    }

    return !white == mBlackBoard.test(row * BOARD_SIZE + column);
}

void Tablut::CheckCapture(uint8_t row, uint8_t column, bool isWhite) {
    if (isWhite) {
        if (PositionIsColor(row - 1, column, false) &&
            PositionIsColor(row - 2, column, true)) {
            mBlackBoard.reset((row - 1) * BOARD_SIZE + column);
        }
        if (PositionIsColor(row + 1, column, false) &&
            PositionIsColor(row + 2, column, true)) {
            mBlackBoard.reset((row + 1) * BOARD_SIZE + column);
        }
        if (PositionIsColor(row, column - 1, false) &&
            PositionIsColor(row, column - 2, true)) {
            mBlackBoard.reset(row * BOARD_SIZE + (column - 1));
        }
        if (PositionIsColor(row, column + 1, false) &&
            PositionIsColor(row, column + 2, true)) {
            mBlackBoard.reset(row * BOARD_SIZE + (column + 1));
        }

    } else {
        CheckKingCapture();

        if (PositionIsColor(row - 1, column, true) &&
            (PositionIsColor(row - 2, column, false) ||
             PositionIsUnreachable(row - 2, column))) {
            mWhiteBoard.reset((row - 1) * BOARD_SIZE + column);
        }
        if (PositionIsColor(row + 1, column, true) &&
            (PositionIsColor(row + 2, column, false) ||
             PositionIsUnreachable(row + 2, column))) {
            mWhiteBoard.reset((row + 1) * BOARD_SIZE + column);
        }
        if (PositionIsColor(row, column - 1, true) &&
            (PositionIsColor(row, column - 2, false) ||
             PositionIsUnreachable(row, column - 2))) {
            mWhiteBoard.reset(row * BOARD_SIZE + (column - 1));
        }
        if (PositionIsColor(row, column + 1, true) &&
            (PositionIsColor(row, column + 2, false) ||
             PositionIsUnreachable(row, column + 2))) {
            mWhiteBoard.reset(row * BOARD_SIZE + (column + 1));
        }
    }
}

void Tablut::CheckKingCapture() {
    uint8_t kingRow{0};
    uint8_t kingColumn{0};

    for (; kingRow < BOARD_SIZE; kingRow++) {
        for (; kingColumn < BOARD_SIZE; kingColumn++) {
            if (mKingBoard.test(kingRow * BOARD_SIZE + kingColumn)) {
                break;
            }
        }
    }

    if (kingRow == 4 && kingColumn == 4) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mKingBoard.reset(kingRow * BOARD_SIZE + kingColumn);
        }
    }
    if (kingRow == 3 && kingColumn == 4) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mKingBoard.reset(kingRow * BOARD_SIZE + kingColumn);
        }
    }
    if (kingRow == 5 && kingColumn == 4) {
        if (PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mKingBoard.reset(kingRow * BOARD_SIZE + kingColumn);
        }
    }
    if (kingRow == 4 && kingColumn == 3) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false)) {
            mKingBoard.reset(kingRow * BOARD_SIZE + kingColumn);
        }
    }
    if (kingRow == 4 && kingColumn == 5) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mKingBoard.reset(kingRow * BOARD_SIZE + kingColumn);
        }
    }
}

std::string PrintPosition(const PiecePosition &position) {
    std::stringstream ss;
    ss << (char)(position.Column + 97) << std::to_string(position.Row + 1);

    return ss.str();
}

std::string PrintTable(const Tablut &tablut) {
    std::stringstream ss;

    ss << "    a   b   c   d   e   f   g   h   i\n";
    ss << "  -------------------------------------";
    for (uint8_t row = 0; row < 9; row++) {
        ss << "\n" << std::to_string(row + 1) << " ";
        for (uint8_t column = 0; column < 9; column++) {
            if (tablut.IsType(row, column, PieceType::King)) {
                ss << "| K ";
            } else if (tablut.IsType(row, column, PieceType::Guard)) {
                ss << "| G ";
            } else if (tablut.IsType(row, column, PieceType::Mercenary)) {
                ss << "| M ";
            } else {
                ss << "|   ";
            }
        }
        ss << "|\n";
        ss << "  -------------------------------------";
    }

    return ss.str();
}
