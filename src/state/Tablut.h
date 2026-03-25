#pragma once
#include <cstdint>
#include <set>
#include <string>
#include <vector>

#include "state/Piece.h"

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

    /*
     * Returns true if the table has the king.
     */
    bool HasKing() const;
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
    bool PositionIsColor(uint8_t row, uint8_t column, bool white);
    void CheckCapture(const Piece &movedPiece);
    void CheckKingCapture();

    // TODO: black and white sets?
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
