#pragma once
#include <memory>

#include "connection/Socket.h"
#include "state/Tablut.h"

/*
 * Wrapper to send tablut messages through a socket.
 */
class TablutSocketWriter {
public:
    /*
     * Creates a new TablutSocketWriter with the given socket.
     * @param socket The socket to use for sending messages, the
     * TablutSocketWriter takes ownership of the socket and will close it when
     * it is destroyed.
     */
    TablutSocketWriter(const std::shared_ptr<Socket> &socket);

    /*
     * Writes a move to the socket.
     * @param fromPosition The position of the piece to move.
     * @param toPosition The position to move the piece to.
     * @param whiteTurn Whether it is the white player's turn.
     */
    void WriteMove(const Position &fromPosition, const Position &toPosition,
                   bool whiteTurn) const;

private:
    std::shared_ptr<Socket> mSocket;
};
