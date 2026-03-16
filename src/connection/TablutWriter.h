#pragma once
#include <memory>

#include "connection/Socket.h"
#include "state/Tablut.h"

class TablutSocketWriter {
public:
    TablutSocketWriter(const std::shared_ptr<Socket> &socket);

    void WriteMove(const Position &fromPosition, const Position &toPositon,
                   bool whiteTurn) const;

private:
    std::shared_ptr<Socket> mSocket;
};
