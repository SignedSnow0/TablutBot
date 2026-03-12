#pragma once
#include <cstdint>
#include <optional>
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
    Piece &operator=(const Piece &) = delete;

    [[nodiscard]] inline uint8_t Row() const;
    [[nodiscard]] inline uint8_t Column() const;
    [[nodiscard]] inline bool IsKing() const;
    [[nodiscard]] inline bool IsGuard() const;
    [[nodiscard]] inline bool IsMercenary() const;
    [[nodiscard]] inline bool IsInCamp() const;
    [[nodiscard]] inline bool IsAt(uint8_t row, uint8_t column) const;

    void Move(uint8_t row, uint8_t column);

private:
    /* Bit representation of a cell:
     *   K  G  M  | TC RC BC LC |    |    Column   |     Row
     *   15 14 13 | 12 11 10 09 | 08 | 07 06 05 04 | 03 02 01 00
     */
    uint16_t mInternal{0};
};

class Tablut {
public:
    static Tablut InitalConfiguration() {
        Tablut result;
        result.mPieces.reserve(25);

        result.mPieces.push_back(
            Piece(0, 3, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.push_back(
            Piece(0, 4, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.push_back(
            Piece(0, 5, Piece::Type::Mercenary, Piece::Camp::Top));
        result.mPieces.push_back(
            Piece(1, 4, Piece::Type::Mercenary, Piece::Camp::Top));

        result.mPieces.push_back(
            Piece(8, 3, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.push_back(
            Piece(8, 4, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.push_back(
            Piece(8, 5, Piece::Type::Mercenary, Piece::Camp::Bottom));
        result.mPieces.push_back(
            Piece(7, 4, Piece::Type::Mercenary, Piece::Camp::Bottom));

        result.mPieces.push_back(
            Piece(3, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.push_back(
            Piece(4, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.push_back(
            Piece(5, 0, Piece::Type::Mercenary, Piece::Camp::Left));
        result.mPieces.push_back(
            Piece(4, 1, Piece::Type::Mercenary, Piece::Camp::Left));

        result.mPieces.push_back(
            Piece(3, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.push_back(
            Piece(4, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.push_back(
            Piece(5, 8, Piece::Type::Mercenary, Piece::Camp::Right));
        result.mPieces.push_back(
            Piece(4, 7, Piece::Type::Mercenary, Piece::Camp::Right));

        result.mPieces.push_back(Piece(4, 4, Piece::Type::King));

        result.mPieces.push_back(Piece(3, 4, Piece::Type::Guard));
        result.mPieces.push_back(Piece(2, 4, Piece::Type::Guard));

        result.mPieces.push_back(Piece(5, 4, Piece::Type::Guard));
        result.mPieces.push_back(Piece(6, 4, Piece::Type::Guard));

        result.mPieces.push_back(Piece(4, 3, Piece::Type::Guard));
        result.mPieces.push_back(Piece(4, 2, Piece::Type::Guard));

        result.mPieces.push_back(Piece(4, 5, Piece::Type::Guard));
        result.mPieces.push_back(Piece(4, 6, Piece::Type::Guard));

        return result;
    }

    [[nodiscard]] bool IsEmpty(uint8_t row, uint8_t column) const;
    [[nodiscard]] std::optional<Piece> PieceAt(uint8_t row,
                                               uint8_t column) const;

    [[nodiscard]] std::vector<Position> GenMoves(uint8_t row,
                                                 uint8_t column) const;

    void Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
              uint8_t toColumn);

private:
    std::vector<Piece> mPieces;
};

std::string PrintCell(const Piece &cell);
std::string PrintGrid(const Tablut &tablut);
