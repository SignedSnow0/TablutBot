#include "Tablut.h"

#include <cstdint>
#include <sstream>
#include <string>

#include "state/Piece.h"
#include "utils/Logger.h"

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

bool positionInCamp(uint8_t row, uint8_t column) {
    return positionInTopCamp(row, column) || positionInRightCamp(row, column) ||
           positionInBottomCamp(row, column) || positionInLeftCamp(row, column);
}

bool positionIsUnreachable(uint8_t row, uint8_t column) {
    return positionInCamp(row, column) || (row == 4 && column == 4);
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

bool specialSquare(uint8_t row, uint8_t column) {
    if ((row == 4 && column == 4) || positionInTopCamp(row, column) ||
        positionInRightCamp(row, column) || positionInBottomCamp(row, column) ||
        positionInLeftCamp(row, column)) {
        return true;
    }

    return false;
}

bool Tablut::operator==(const Tablut &r) const {
    if (mPieces.size() != r.mPieces.size()) {
        return false;
    }

    auto lPiece = mPieces.begin();
    auto rPiece = r.mPieces.begin();
    while (lPiece != mPieces.end()) {
        if (*lPiece != *rPiece) {
            return false;
        }

        lPiece++;
        rPiece++;
    }

    return true;
}

bool Tablut::IsSameAsServer(const Tablut &r) const {
    if (mPieces.size() != r.mPieces.size()) {
        return false;
    }

    auto lPiece = mPieces.begin();
    auto rPiece = r.mPieces.begin();
    while (lPiece != mPieces.end()) {
        bool sameType = lPiece->IsBlack() == rPiece->IsBlack();
        sameType &= lPiece->IsWhite() == rPiece->IsWhite();
        sameType &= lPiece->IsKing() == rPiece->IsKing();

        if (!sameType || lPiece->Position() != rPiece->Position()) {
            return false;
        }

        lPiece++;
        rPiece++;
    }

    return true;
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

bool Tablut::HasKing() const {
    for (const auto &piece : mPieces) {
        if (piece.IsKing()) {
            return true;
        }
    }

    return false;
}

bool Tablut::IsEmpty(uint8_t row, uint8_t column) const {
    for (const auto &piece : mPieces) {
        if (piece.IsAt(row, column)) {
            return false;
        }
    }

    return true;
}

// NOTE: to generate camp moves, check only if the move is from the outside in,
// then it is invalid
std::vector<Position> Tablut::GenMoves(uint8_t row, uint8_t column) const {
    const auto &piece = mPieces.find(Piece(row, column, Piece::Type::King));
    if (piece == mPieces.end()) {
        LOG_WARNING("Generating moves for empty position");
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
        LOG_WARNING("Trying to move from empty position");
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

    CheckCapture(newPiece);
}

bool Tablut::PositionIsColor(uint8_t row, uint8_t column, bool white) {
    const auto &piece = mPieces.find(Piece{row, column, Piece::Type::King});
    if (piece == mPieces.end()) {
        return false;
    }

    return white == piece->IsWhite();
}

void Tablut::CheckCapture(const Piece &movedPiece) {
    uint8_t row = movedPiece.Row();
    uint8_t column = movedPiece.Column();

    if (movedPiece.IsWhite()) {
        if (PositionIsColor(row - 1, column, false) &&
            PositionIsColor(row - 2, column, true)) {
            mPieces.erase(Piece(row - 1, column, Piece::Type::King));
        }
        if (PositionIsColor(row + 1, column, false) &&
            PositionIsColor(row + 2, column, true)) {
            mPieces.erase(Piece(row + 1, column, Piece::Type::King));
        }
        if (PositionIsColor(row, column - 1, false) &&
            PositionIsColor(row, column - 2, true)) {
            mPieces.erase(Piece(row, column - 1, Piece::Type::King));
        }
        if (PositionIsColor(row, column + 1, false) &&
            PositionIsColor(row, column + 2, true)) {
            mPieces.erase(Piece(row, column + 1, Piece::Type::King));
        }

    } else {
        CheckKingCapture();

        if (PositionIsColor(row - 1, column, true) &&
            (PositionIsColor(row - 2, column, false) ||
             positionIsUnreachable(row - 2, column))) {
            mPieces.erase(Piece(row - 1, column, Piece::Type::King));
        }
        if (PositionIsColor(row + 1, column, true) &&
            (PositionIsColor(row + 2, column, false) ||
             positionIsUnreachable(row + 2, column))) {
            mPieces.erase(Piece(row + 1, column, Piece::Type::King));
        }
        if (PositionIsColor(row, column - 1, true) &&
            (PositionIsColor(row, column - 2, false) ||
             positionIsUnreachable(row, column - 2))) {
            mPieces.erase(Piece(row, column - 1, Piece::Type::King));
        }
        if (PositionIsColor(row, column + 1, true) &&
            (PositionIsColor(row, column + 2, false) ||
             positionIsUnreachable(row, column + 2))) {
            mPieces.erase(Piece(row, column + 1, Piece::Type::King));
        }
    }
}

void Tablut::CheckKingCapture() {
    auto king = mPieces.begin();
    for (auto it = mPieces.begin(); it != mPieces.end(); it++) {
        if (it->IsKing()) {
            king = it;
            break;
        }
    }

    uint8_t kingRow = king->Row();
    uint8_t kingColumn = king->Column();

    if (kingRow == 4 && kingColumn == 4) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mPieces.erase(king);
        }
    }
    if (kingRow == 3 && kingColumn == 4) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mPieces.erase(king);
        }
    }
    if (kingRow == 5 && kingColumn == 4) {
        if (PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mPieces.erase(king);
        }
    }
    if (kingRow == 4 && kingColumn == 3) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn - 1, false)) {
            mPieces.erase(king);
        }
    }
    if (kingRow == 4 && kingColumn == 5) {
        if (PositionIsColor(kingRow - 1, kingColumn, false) &&
            PositionIsColor(kingRow + 1, kingColumn, false) &&
            PositionIsColor(kingRow, kingColumn + 1, false)) {
            mPieces.erase(king);
        }
    }
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
