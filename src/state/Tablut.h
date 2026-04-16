#pragma once
#include <bitset>
#include <cstdint>
#include <string>
#include <vector>

#define BOARD_SIZE 9

struct PiecePosition {
    uint8_t Row;
    uint8_t Column;
};

struct PieceMove {
    PiecePosition From;
    PiecePosition To;
};

enum class PieceType { King, Guard, Mercenary };

struct Piece {
    PiecePosition Position;
    PieceType Type;
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
        result.InsertBlack(0, 3);
        result.InsertBlack(0, 4);
        result.InsertBlack(0, 5);
        result.InsertBlack(1, 4);

        result.InsertBlack(8, 3);
        result.InsertBlack(8, 4);
        result.InsertBlack(8, 5);
        result.InsertBlack(7, 4);

        result.InsertBlack(3, 0);
        result.InsertBlack(4, 0);
        result.InsertBlack(5, 0);
        result.InsertBlack(4, 1);

        result.InsertBlack(3, 8);
        result.InsertBlack(4, 8);
        result.InsertBlack(5, 8);
        result.InsertBlack(4, 7);

        result.InsertWhite(3, 4);
        result.InsertWhite(2, 4);

        result.InsertWhite(5, 4);
        result.InsertWhite(6, 4);

        result.InsertWhite(4, 3);
        result.InsertWhite(4, 2);

        result.InsertWhite(4, 5);
        result.InsertWhite(4, 6);

        result.InsertKing(4, 4);

        return result;
    }

    [[nodiscard]] bool operator==(const Tablut &r) const;

    /*
     * Returns the white pieces currently on the board.
     * The pieces are ordered by their position, with the row having higher
     * priority than the column.
     */
    [[nodiscard]] std::vector<Piece> WhitePieces() const;
    /*
     * Returns the black pieces currently on the board.
     * The pieces are ordered by their position, with the row having higher
     * priority than the column.
     */
    [[nodiscard]] std::vector<Piece> BlackPieces() const;

    /*
     * Returns true if the table has the king.
     */
    [[nodiscard]] bool HasKing() const;
    [[nodiscard]] Piece King() const;
    /**
     * Returns whether the square at the given position is empty.
     * @param row The row of the square, between 0 and 8.
     * @param column The column of the square, between 0 and 8.
     */
    [[nodiscard]] bool IsEmpty(uint8_t row, uint8_t column) const;
    [[nodiscard]] bool IsType(uint8_t row, uint8_t column,
                              PieceType type) const;

    /*
     * Generates the possible moves for the piece at the given position.
     * If there is no piece at the given position, an empty vector is returned.
     *
     * @param row The row of the piece, between 0 and 8.
     * @param column The column of the piece, between 0 and 8.
     */
    [[nodiscard]] std::vector<PiecePosition> GenMoves(uint8_t row,
                                                      uint8_t column) const;
    /*
     * Generates the possible moves for the piece at the given position.
     * If there is no piece at the given position, an empty vector is returned.
     *
     * @param row The row of the piece, between 0 and 8.
     * @param column The column of the piece, between 0 and 8.
     */
    [[nodiscard]] inline std::vector<PiecePosition>
    GenMoves(PiecePosition position) const {
        return GenMoves(position.Row, position.Column);
    }

    [[nodiscard]] std::vector<PieceMove> GenAllMoves(bool white) const;

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
    Tablut Move(uint8_t fromRow, uint8_t fromColumn, uint8_t toRow,
                uint8_t toColumn) const;
    /*
     * Moves the piece at the given position to the given position.
     * The method does not check if the move is valid, it is the caller's
     * responsibility to ensure that the move is valid before calling this
     * method.
     *
     * @param fromPosition The position of the piece to move.
     * @param toPosition The position of the destination.
     */
    inline Tablut Move(PiecePosition fromPosition,
                       PiecePosition toPosition) const {
        return Move(fromPosition.Row, fromPosition.Column, toPosition.Row,
                    toPosition.Column);
    }

private:
    Tablut() = default;
    Tablut(std::bitset<BOARD_SIZE * BOARD_SIZE> blackBoard,
           std::bitset<BOARD_SIZE * BOARD_SIZE> whiteBoard,
           std::bitset<BOARD_SIZE * BOARD_SIZE> kingBoard)
        : mBlackBoard(blackBoard), mWhiteBoard(whiteBoard),
          mKingBoard(kingBoard) {}

    void InsertWhite(uint8_t row, uint8_t column);
    void InsertBlack(uint8_t row, uint8_t column);
    void InsertKing(uint8_t row, uint8_t column);

    bool PositionIsColor(uint8_t row, uint8_t column, bool white);
    void CheckCapture(uint8_t row, uint8_t column, bool isWhite);
    void CheckKingCapture();

    std::bitset<BOARD_SIZE * BOARD_SIZE> mBlackBoard;
    std::bitset<BOARD_SIZE * BOARD_SIZE> mWhiteBoard;
    std::bitset<BOARD_SIZE * BOARD_SIZE> mKingBoard;

    friend class TablutSocketReader;
};

/*
 * Prints the given position in a two-character format with the column in [a, i]
 * and the column in [1, 9].
 * @param position The position to print.
 * @return The string representation of the position.
 */
std::string PrintPosition(const PiecePosition &position);

/*
 * Prints the given Tablut board in a human-readable format.
 * @param tablut The Tablut board to print.
 * @return The string representation of the Tablut board.
 */
std::string PrintTable(const Tablut &tablut);
