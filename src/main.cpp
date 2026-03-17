#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "connection/Socket.h"
#include "connection/TablutReader.h"
#include "connection/TablutWriter.h"
#include "state/Tablut.h"
#include "utils/Logger.h"

#define WHITE_PORT 5800
#define BLACK_PORT 5801

#define BOT_NAME_WHITE "TablutBotWhite"
#define BOT_NAME_BLACK "TablutBotBlack"

std::pair<Position, Position> RandomMove(const Tablut &tablut, bool white) {
    const auto &pieces = white ? tablut.WhitePieces() : tablut.BlackPieces();
    for (const auto &piece : pieces) {
        const auto &moves = tablut.GenMoves(piece);
        if (!moves.empty()) {
            const auto &move = moves.begin();
            return {piece.Position(), *move};
        }
    }

    return {{0, 0}, {0, 0}};
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " role serverIp timeout"
                  << std::endl;

        Logger::Shutdown();
        return 1;
    }
    std::string player(argv[1]);
    bool isWhite = player == "WHITE" || player == "white";

    if (isWhite) {
        Logger::Init("logs/white.log");
    } else {
        Logger::Init("logs/black.log");
    }

    LOG_DEBUG("Debug test");
    LOG_INFO("Info test");
    LOG_WARNING("Warning test");
    LOG_ERROR("Error test");

    auto tablut = Tablut::InitialConfiguration();
    std::shared_ptr<Socket> socket;
    if (isWhite) {
        socket = Socket::Connect(argv[2], WHITE_PORT);
        socket->Send(BOT_NAME_WHITE);
    } else {
        socket = Socket::Connect(argv[2], BLACK_PORT);
        socket->Send(BOT_NAME_BLACK);
    }

    TablutSocketReader reader(socket);
    TablutSocketWriter writer(socket);
    const auto _discard = reader.ReceiveTable();
    uint32_t moveIndex{0};

    std::cout << "Table at move " << std::to_string(moveIndex)
              << " (white to move)" << std::endl;
    std::cout << PrintTable(tablut) << std::endl;

    if (isWhite) {
        while (true) {
            // Make move
            const auto &[fromPosition, toPosition] =
                RandomMove(tablut, isWhite);

            std::cout << "Chosen move: " << PrintPosition(fromPosition)
                      << " to " << PrintPosition(toPosition) << std::endl;
            writer.WriteMove(fromPosition, toPosition, isWhite);
            tablut.Move(fromPosition, toPosition);
            moveIndex++;

            std::cout << "Table at move " << std::to_string(moveIndex)
                      << " (black to move)" << std::endl;
            std::cout << PrintTable(tablut) << std::endl;

            // Discard own table
            const auto &[_discard, turn1] = reader.ReceiveTable();
            if (turn1 != Turn::Black) {
                if (turn1 == Turn::Draw) {
                    std::cout << "End of game: draw" << std::endl;
                } else if (turn1 == Turn::WhiteWin) {
                    std::cout << "End of game: lose" << std::endl;
                } else if (turn1 == Turn::BlackWin) {
                    std::cout << "End of game: win" << std::endl;
                }

                Logger::Shutdown();
                return 0;
            }

            // Receive Move
            const auto &[table, turn] = reader.ReceiveTable();
            if (turn != Turn::White) {
                if (turn == Turn::Draw) {
                    std::cout << "End of game: draw" << std::endl;
                } else if (turn == Turn::WhiteWin) {
                    std::cout << "End of game: you win" << std::endl;
                } else if (turn == Turn::BlackWin) {
                    std::cout << "End of game: you lose" << std::endl;
                }

                Logger::Shutdown();
                return 0;
            }
            tablut = table;
            moveIndex++;

            std::cout << "Table at move " << std::to_string(moveIndex)
                      << " (white to move)" << std::endl;
            std::cout << PrintTable(tablut) << std::endl;
        }
    } else {
        while (true) {
            // Receive move
            const auto &[table, turn] = reader.ReceiveTable();
            if (turn != Turn::Black) {
                if (turn == Turn::Draw) {
                    std::cout << "End of game: draw" << std::endl;
                } else if (turn == Turn::WhiteWin) {
                    std::cout << "End of game: lose" << std::endl;
                } else if (turn == Turn::BlackWin) {
                    std::cout << "End of game: win" << std::endl;
                }

                Logger::Shutdown();
                return 0;
            }
            tablut = table;
            moveIndex++;

            std::cout << "Table at move " << std::to_string(moveIndex)
                      << " (black to move)" << std::endl;
            std::cout << PrintTable(tablut) << std::endl;

            // Make move
            const auto &[fromPosition, toPosition] =
                RandomMove(tablut, isWhite);

            std::cout << "Chosen move: " << PrintPosition(fromPosition)
                      << " to " << PrintPosition(toPosition) << std::endl;
            writer.WriteMove(fromPosition, toPosition, isWhite);
            tablut.Move(fromPosition, toPosition);
            moveIndex++;

            std::cout << "Table at move " << std::to_string(moveIndex)
                      << " (white to move)" << std::endl;
            std::cout << PrintTable(tablut) << std::endl;

            // Discard own table
            const auto &[_discard, turn1] = reader.ReceiveTable();
            if (turn1 != Turn::White) {
                if (turn1 == Turn::Draw) {
                    std::cout << "End of game: draw" << std::endl;
                } else if (turn1 == Turn::WhiteWin) {
                    std::cout << "End of game: lose" << std::endl;
                } else if (turn1 == Turn::BlackWin) {
                    std::cout << "End of game: win" << std::endl;
                }

                Logger::Shutdown();
                return 0;
            }
        }
    }

    Logger::Shutdown();
    return 0;
}
