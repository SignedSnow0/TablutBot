#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <vector>

struct Position {
    uint8_t Row;
    uint8_t Column;
};

class Piece {
public:
    enum class Type { King, Guard, Mercenary };
    enum class Camp { None, Top, Left, Bottom, Right };

    Piece(uint8_t row, uint8_t column, Type type, Camp camp = Camp::None);

    bool operator<(const Piece &r) const;

    [[nodiscard]] uint8_t Row() const;
    [[nodiscard]] uint8_t Column() const;
    [[nodiscard]] ::Position Position() const;
    [[nodiscard]] bool IsKing() const;
    [[nodiscard]] bool IsGuard() const;
    [[nodiscard]] bool IsMercenary() const;
    [[nodiscard]] bool IsInCamp() const;
    [[nodiscard]] bool IsWhite() const;
    [[nodiscard]] bool IsBlack() const;
    [[nodiscard]] bool IsAt(uint8_t row, uint8_t column) const;

    Piece Move(uint8_t row, uint8_t column) const;

private:
    Piece() = default;

    /* Bit representation of a cell:
     *   K  G  M  | TC RC BC LC |    |     Row     |    Column
     *   15 14 13 | 12 11 10 09 | 08 | 07 06 05 04 | 03 02 01 00
     */
    uint16_t mInternal{0};
};

class Tablut {
public:
    static Tablut InitalConfiguration() {
        Tablut result;

        result.mPieces.insert(
            Piece(0, 3, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.insert(
            Piece(0, 4, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.insert(
            Piece(0, 5, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.insert(
            Piece(1, 4, Piece::Type::Mercenary, Piece::Camp::Top));

        result.mPieces.insert(
            Piece(8, 3, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.insert(
            Piece(8, 4, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.insert(
            Piece(8, 5, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.insert(
            Piece(7, 4, Piece::Type::Mercenary, Piece::Camp::Bottom));

        result.mPieces.insert(
            Piece(3, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.insert(
            Piece(4, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.insert(
            Piece(5, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.insert(
            Piece(4, 1, Piece::Type::Mercenary, Piece::Camp::Left));

        result.mPieces.insert(
            Piece(3, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.insert(
            Piece(4, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.insert(
            Piece(5, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.insert(
            Piece(4, 7, Piece::Type::Mercenary, Piece::Camp::Right));

        result.mPieces.insert(Piece(4, 4, Piece::Type::King));

        result.mPieces.insert(Piece(3, 4, Piece::Type::Guard));
        result.mPieces.insert(Piece(2, 4, Piece::Type::Guard));

        result.mPieces.insert(Piece(5, 4, Piece::Type::Guard));
        result.mPieces.insert(Piece(6, 4, Piece::Type::Guard));

        result.mPieces.insert(Piece(4, 3, Piece::Type::Guard));
        result.mPieces.insert(Piece(4, 2, Piece::Type::Guard));

        result.mPieces.insert(Piece(4, 5, Piece::Type::Guard));
        result.mPieces.insert(Piece(4, 6, Piece::Type::Guard));

        return result;
    }

    [[nodiscard]] inline const std::set<Piece> &Pieces() const {
        return mPieces;
    }

    std::set<Piece> WhitePieces() const;
    std::set<Piece> BlackPieces() const;
    [[nodiscard]] bool IsEmpty(uint8_t row, uint8_t column) const;

    [[nodiscard]] std::vector<Position> GenMoves(uint8_t row,
                                                 uint8_t column) const;
    [[nodiscard]] inline std::vector<Position>
    GenMoves(const Piece &piece) const {
        return GenMoves(piece.Row(), piece.Column());
    }

    void Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
              uint8_t toColumn);
    inline void Move(Position fromPosition, Position toPosition) {
        Move(fromPosition.Row, fromPosition.Column, toPosition.Row,
             toPosition.Column);
    }

private:
    std::set<Piece> mPieces;

    friend class TablutSocketReader;
};

std::string PrintPosition(const Position &position);
std::string PrintPiece(const Piece &cell);
std::string PrintTable(const Tablut &tablut);
