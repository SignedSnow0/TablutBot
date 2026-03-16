#include "TablutReader.h"
#include "nlohmann/json_fwd.hpp"
#include "state/Tablut.h"

#include <cstdint>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

TablutSocketReader::TablutSocketReader(const std::shared_ptr<Socket> &socket)
    : mSocket(socket) {}

Tablut TablutSocketReader::ReceiveTable() const {
    const auto data = mSocket->Receive();
    const auto parsed = json::parse(data);

    const auto &board = parsed["board"];

    Tablut out;
    for (uint32_t row = 0; row < board.size(); row++) {
        for (uint32_t column = 0; column < board[row].size(); column++) {
            if (board[row][column] == "BLACK") {
                out.mPieces.insert(Piece(row, column, Piece::Type::Mercenary));
            } else if (board[row][column] == "WHITE") {
                out.mPieces.insert(Piece(row, column, Piece::Type::Guard));
            } else if (board[row][column] == "KING") {
                out.mPieces.insert(Piece(row, column, Piece::Type::King));
            }
        }
    }

    return std::move(out);
}

bool TablutSocketReader::SameAs(const Tablut &server, const Tablut &client) {
    auto &serverPieces = server.Pieces();
    auto &clientPieces = client.Pieces();

    if (serverPieces.size() != clientPieces.size()) {
        return false;
    }

    auto serverPiece = serverPieces.begin();
    auto clientPiece = clientPieces.begin();
    while (serverPiece != serverPieces.end()) {
        if (serverPiece->Column() != clientPiece->Column() ||
            serverPiece->Row() != clientPiece->Row()) {
            return false;
        }

        serverPiece++;
        clientPiece++;
    }

    return true;
}
