#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <vector>

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
};

/*
 * Represents the state of a Tablut game.
 */
class Tablut {
public:
    /*
     * Returns the initial configuration of the game.
     */
    static Tablut InitialConfiguration() {
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

    /*
     * Returns the pieces currently on the board.
     * The pieces are ordered by their position, with the row having higher
     * priority than the column.
     */
    [[nodiscard]] inline const std::set<Piece> &Pieces() const {
        return mPieces;
    }

    /*
     * Returns the white pieces currently on the board.
     * The pieces are ordered by their position, with the row having higher
     * priority than the column.
     */
    std::set<Piece> WhitePieces() const;
    /*
     * Returns the black pieces currently on the board.
     * The pieces are ordered by their position, with the row having higher
     * priority than the column.
     */
    std::set<Piece> BlackPieces() const;
    /**
     * Returns whether the square at the given position is empty.
     * @param row The row of the square, between 0 and 8.
     * @param column The column of the square, between 0 and 8.
     */
    [[nodiscard]] bool IsEmpty(uint8_t row, uint8_t column) const;

    /*
     * Generates the possible moves for the piece at the given position.
     * If there is no piece at the given position, an empty vector is returned.
     *
     * @param row The row of the piece, between 0 and 8.
     * @param column The column of the piece, between 0 and 8.
     */
    [[nodiscard]] std::vector<Position> GenMoves(uint8_t row,
                                                 uint8_t column) const;
    /**
     * Generates the possible moves for the given piece.
     * If there is no piece at the given position, an empty vector is returned.
     *
     * @param piece The piece for which to generate moves, the method does not
     * check if the piece given is the same as the one on the board.
     */
    [[nodiscard]] inline std::vector<Position>
    GenMoves(const Piece &piece) const {
        return GenMoves(piece.Row(), piece.Column());
    }

    /*
     * Moves the piece at the given position to the given position.
     * The method does not check if the move is valid, it is the caller's
     * responsibility to ensure that the move is valid before calling this
     * method.
     *
     * @param fromRow The row of the piece to move, between 0 and 8.
     * @param fromColumn The column of the piece to move, between 0 and 8.
     * @param toRow The row of the destination, between 0 and 8.
     * @param toColumn The column of the destination, between 0 and
     */
    void Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
              uint8_t toColumn);
    /*
     * Moves the piece at the given position to the given position.
     * The method does not check if the move is valid, it is the caller's
     * responsibility to ensure that the move is valid before calling this
     * method.
     *
     * @param fromPosition The position of the piece to move.
     * @param toPosition The position of the destination.
     */
    inline void Move(Position fromPosition, Position toPosition) {
        Move(fromPosition.Row, fromPosition.Column, toPosition.Row,
             toPosition.Column);
    }

private:
    std::set<Piece> mPieces;

    friend class TablutSocketReader;
};

/*
 * Prints the given position in a two-character format with the column in [a, i]
 * and the column in [1, 9].
 * @param position The position to print.
 * @return The string representation of the position.
 */
std::string PrintPosition(const Position &position);
/*
 * Prints the given piece in a two-character format.
 * @param cell The piece to print.
 * @return The string representation of the piece.
 */
std::string PrintPiece(const Piece &cell);
/*
 * Prints the given Tablut board in a human-readable format.
 * @param tablut The Tablut board to print.
 * @return The string representation of the Tablut board.
 */
std::string PrintTable(const Tablut &tablut);
