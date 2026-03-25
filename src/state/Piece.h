#pragma once
#include <cstdint>

/*
 * Represents a position on the board.
 */
struct Position {
    uint8_t Row;
    uint8_t Column;
};

/*
 * Represents a piece on the board.
 */
class Piece {
public:
    /*
     * Used to represent the type of a piece.
     */
    enum class Type { King, Guard, Mercenary };
    /*
     * Used to represent the initial camp of a mercenary.
     * On any other type the value is disregarded
     */
    enum class Camp { None, Top, Left, Bottom, Right };

    /*
     * Creates a new piece with the specified values.
     * @param row The row of the piece, between 0 and 8.
     * @param column The column of the piece, between 0 and 8.
     * @param type The type of the piece.
     * @param camp The initial camp of the piece, only relevant if the type is
     * Mercenary.
     */
    Piece(uint8_t row, uint8_t column, Type type, Camp camp = Camp::None);

    /*
     * Compares two pieces only by their positions, with the row having higher
     * priority than the column.
     */
    bool operator<(const Piece &r) const;

    /*
     * Returns the row of the piece.
     */
    [[nodiscard]] uint8_t Row() const;
    /*
     * Returns the column of the piece.
     */
    [[nodiscard]] uint8_t Column() const;
    /*
     * Returns the position of the piece.
     */
    [[nodiscard]] ::Position Position() const;
    /*
     * Returns true if the piece is a king.
     */
    [[nodiscard]] bool IsKing() const;
    /*
     * Returns true if the piece is a guard.
     */
    [[nodiscard]] bool IsGuard() const;
    /*
     * Returns true if the piece is a mercenary.
     */
    [[nodiscard]] bool IsMercenary() const;
    /*
     * Returns true if the piece is in its initial camp.
     */
    [[nodiscard]] bool IsInCamp() const;
    /*
     * Returns true if the piece is white.
     */
    [[nodiscard]] bool IsWhite() const;
    /*
     * Returns true if the piece is black.
     */
    [[nodiscard]] bool IsBlack() const;
    /*
     * Returns true if the piece is at the specified position.
     */
    [[nodiscard]] bool IsAt(uint8_t row, uint8_t column) const;

    /*
     * Creates a new piece identical to the current one but at the specified
     * position. It is important to use this method instead of the constructor
     * to move a piece, as the constructor does not guarantee that the type and
     * camp are preserved.
     *
     * @param row The row of the new piece, between 0 and 8.
     * @param column The column of the new piece, between 0 and 8.
     */
    Piece Move(uint8_t row, uint8_t column) const;

private:
    Piece() = default;

    /*
     * Bit representation of a cell:
     * K  G  M  | TC RC BC LC |    |     Row     |    Column
     * 15 14 13 | 12 11 10 09 | 08 | 07 06 05 04 | 03 02 01 00
     */
    uint16_t mInternal{0};

    friend class Tablut;
};
