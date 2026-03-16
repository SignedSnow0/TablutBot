#pragma once
#include <memory>

#include "connection/Socket.h"
#include "state/Tablut.h"

enum class Turn { White, Black, Draw, WhiteWin, BlackWin };

class TablutSocketReader {
public:
    TablutSocketReader(const std::shared_ptr<Socket> &socket);

    std::pair<Tablut, Turn> ReceiveTable() const;

    bool SameAs(const Tablut &server, const Tablut &client);

private:
    std::shared_ptr<Socket> mSocket;
};
