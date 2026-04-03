#include <algorithm>
#include <cctype>
#include <cstdint>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "ai/Minimax.h"
#include "connection/Socket.h"
#include "connection/TablutReader.h"
#include "connection/TablutWriter.h"
#include "state/Tablut.h"
#include "utils/Logger.h"

#define WHITE_PORT 5800
#define BLACK_PORT 5801

#define BOT_NAME_WHITE "Joshua_White"
#define BOT_NAME_BLACK "Joshua_Black"

#define MAX_DEPTH 5

void toLower(std::string &string) {
    for (char &c : string) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <role> <server_ip> <timeout (seconds)>" << std::endl;

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

    auto solver = Minimax();

    std::shared_ptr<Socket> socket;
    if (isWhite) {
        socket = Socket::Connect(argv[2], WHITE_PORT);
        socket->Send(BOT_NAME_WHITE);
    } else {
        socket = Socket::Connect(argv[2], BLACK_PORT);
        socket->Send(BOT_NAME_BLACK);
    }
    LOG_INFO("Connected to {}:{} with name {}", argv[2],
             isWhite ? WHITE_PORT : BLACK_PORT,
             isWhite ? BOT_NAME_WHITE : BOT_NAME_BLACK);

    TablutSocketReader reader(socket);
    TablutSocketWriter writer(socket);
    const auto _discard = reader.ReceiveTable();
    uint32_t moveIndex{0};

    if (isWhite) {
        while (true) {
            // Make move
            solver.Solve(MAX_DEPTH, true);
            const auto &[fromPosition, toPosition] = solver.MaxMove();

            LOG_INFO("Chosen move: {} to {}", PrintPosition(fromPosition),
                     PrintPosition(toPosition));
            writer.WriteMove(fromPosition, toPosition, isWhite);
            moveIndex++;

            const auto &[mirrorTable, blackTurn] = reader.ReceiveTable();
            solver.ChangeRoot(mirrorTable);
            if (solver.CurrentState() != mirrorTable) {
                LOG_ERROR("Mismatch between internal state and server state\n "
                          "Server is:\n{}\nClient is:\n{}",
                          PrintTable(mirrorTable),
                          PrintTable(solver.CurrentState()));
            }
            // LOG_INFO("Table at move {} (black to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            if (blackTurn != Turn::Black) {
                if (blackTurn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (blackTurn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (blackTurn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }

                Logger::Shutdown();
                return 0;
            }

            // Receive Move
            const auto &[table, whiteTurn] = reader.ReceiveTable();
            solver.ChangeRoot(table);
            if (whiteTurn != Turn::White) {
                if (blackTurn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (blackTurn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (blackTurn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }

                Logger::Shutdown();
                return 0;
            }
            moveIndex++;

            // LOG_INFO("Table at move {} (white to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));
        }
    } else {
        while (true) {
            // Receive move
            const auto &[table, blackTurn] = reader.ReceiveTable();
            solver.ChangeRoot(table);
            if (blackTurn != Turn::Black) {
                if (blackTurn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (blackTurn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (blackTurn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }

                Logger::Shutdown();
                return 0;
            }
            moveIndex++;

            // LOG_INFO("Table at move {} (black to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            // Make move
            solver.Solve(MAX_DEPTH, false);
            const auto &[fromPosition, toPosition] = solver.MaxMove();

            LOG_INFO("Chosen move: {} to {}", PrintPosition(fromPosition),
                     PrintPosition(toPosition));
            writer.WriteMove(fromPosition, toPosition, isWhite);
            moveIndex++;

            const auto &[mirrorTable, whiteTurn] = reader.ReceiveTable();
            solver.ChangeRoot(mirrorTable);
            if (solver.CurrentState() != mirrorTable) {
                LOG_ERROR("Mismatch between internal state and server state\n "
                          "Server is:\n{}\nClient is:\n{}",
                          PrintTable(mirrorTable),
                          PrintTable(solver.CurrentState()));
            }
            // LOG_INFO("Table at move {} (white to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            if (whiteTurn != Turn::White) {
                if (blackTurn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (blackTurn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (blackTurn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }
                Logger::Shutdown();
                return 0;
            }
        }
    }

    Logger::Shutdown();
    return 0;
}
