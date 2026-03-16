#pragma once
#include <memory>

#include "connection/Socket.h"
#include "state/Tablut.h"

class TablutSocketReader {
public:
    TablutSocketReader(const std::shared_ptr<Socket> &socket);

    Tablut ReceiveTable() const;

    bool SameAs(const Tablut &server, const Tablut &client);

private:
    std::shared_ptr<Socket> mSocket;
};
