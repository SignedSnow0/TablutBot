#pragma once
#include <memory>

#include "connection/Socket.h"
#include "state/Tablut.h"

/**
 * Represents the turn in the game.
 */
enum class Turn {
    /*
     * It is the white player's turn.
     */
    White,
    /*
     * It is the black player's turn.
     */
    Black,
    /*
     * The game ended in a draw.
     */
    Draw,
    /*
     * The white player has won.
     */
    WhiteWin,
    /*
     * The black player has won.
     */
    BlackWin
};

/*
 * Wrapper to receive tablut messages through a socket.
 */
class TablutSocketReader {
public:
    /*
     * Creates a new TablutSocketReader with the given socket.
     * @param socket The socket to use for receiving messages, the
     * TablutSocketReader takes ownership of the socket and will close it when
     * it is destroyed.
     */
    TablutSocketReader(const std::shared_ptr<Socket> &socket);

    /*
     * Receives a table from the socket.
     * @return A pair containing the received table and the next turn.
     */
    std::pair<Tablut, Turn> ReceiveTable() const;

private:
    std::shared_ptr<Socket> mSocket;
};
