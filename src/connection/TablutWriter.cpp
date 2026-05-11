#include "TablutWriter.h"

#include <nlohmann/json.hpp>

#include "state/Tablut.h"

using json = nlohmann::json;

TablutSocketWriter::TablutSocketWriter(const std::shared_ptr<Socket> &socket)
    : mSocket(socket) {}

void TablutSocketWriter::WriteMove(const PiecePosition &fromPosition,
                                   const PiecePosition &toPosition,
                                   bool whiteTurn) const {
    auto outMsg = json::object();
    outMsg["from"] = PrintPosition(fromPosition);
    outMsg["to"] = PrintPosition(toPosition);
    outMsg["turn"] = whiteTurn ? "WHITE" : "BLACK";

    mSocket->Send(outMsg.dump());
}
