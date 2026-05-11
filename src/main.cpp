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

#define MAX_DEPTH 6
#define NUM_THREADS 4

void toLower(std::string &string) {
    for (char &c : string) {
        c = std::tolower(static_cast<unsigned char>(c));
    }
}

int main(int argc, char **argv) {

    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << " <role> <timeout (seconds)> <server_ip>" << std::endl;

        Logger::Shutdown();
        return 1;
    }
    std::string player(argv[1]);
    toLower(player);
    bool isWhite = player == "white";

    if (isWhite) {
        Logger::Init("logs/white.log");
    } else {
        Logger::Init("logs/black.log");
    }

    auto solver = Minimax(std::max(atoi(argv[2]) * 1000 - 500, 1000),
                          NUM_THREADS, MAX_DEPTH);

    std::shared_ptr<Socket> socket;
    if (isWhite) {
        socket = Socket::Connect(argv[3], WHITE_PORT);
        socket->Send(BOT_NAME_WHITE);
    } else {
        socket = Socket::Connect(argv[3], BLACK_PORT);
        socket->Send(BOT_NAME_BLACK);
    }
    LOG_INFO("Connected to {}:{} with name {}", argv[3],
             isWhite ? WHITE_PORT : BLACK_PORT,
             isWhite ? BOT_NAME_WHITE : BOT_NAME_BLACK);

    TablutSocketReader reader(socket);
    TablutSocketWriter writer(socket);
    auto received = reader.ReceiveTable();
    solver.RegisterState(received.first, received.second == Turn::White);
    uint32_t moveIndex{0};

    if (isWhite) {
        while (true) {
            // Make move
            int64_t value = solver.Solve(received.first, true);
            const auto &[fromPosition, toPosition] = solver.BestMove();

            LOG_INFO("Chosen move: {} -> {} with score of {}",
                     PrintPosition(fromPosition), PrintPosition(toPosition),
                     value);
            writer.WriteMove(fromPosition, toPosition, isWhite);
            moveIndex++;

            received = reader.ReceiveTable();
            solver.RegisterState(received.first,
                                 received.second == Turn::White);
            // LOG_INFO("Table at move {} (black to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            auto turn = received.second;
            if (turn != Turn::Black) {
                if (turn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (turn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (turn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }

                Logger::Shutdown();
                return 0;
            }

            // Receive Move
            received = reader.ReceiveTable();
            solver.RegisterState(received.first,
                                 received.second == Turn::White);
            turn = received.second;
            if (turn != Turn::White) {
                if (turn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (turn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (turn == Turn::BlackWin) {
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
            received = reader.ReceiveTable();
            solver.RegisterState(received.first,
                                 received.second == Turn::White);
            auto turn = received.second;
            if (turn != Turn::Black) {
                if (turn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (turn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (turn == Turn::BlackWin) {
                    LOG_INFO("End of game: black won");
                }

                Logger::Shutdown();
                return 0;
            }
            moveIndex++;

            // LOG_INFO("Table at move {} (black to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            // Make move
            int64_t value = solver.Solve(received.first, false);
            const auto &[fromPosition, toPosition] = solver.BestMove();

            LOG_INFO("Chosen move: {} to {} with score of {}",
                     PrintPosition(fromPosition), PrintPosition(toPosition),
                     value);
            writer.WriteMove(fromPosition, toPosition, isWhite);
            moveIndex++;

            received = reader.ReceiveTable();
            solver.RegisterState(received.first,
                                 received.second == Turn::White);
            turn = received.second;
            // LOG_INFO("Table at move {} (white to move)\n{}", moveIndex,
            //          PrintTable(solver.CurrentState()));

            if (turn != Turn::White) {
                if (turn == Turn::Draw) {
                    LOG_INFO("End of game: draw");
                } else if (turn == Turn::WhiteWin) {
                    LOG_INFO("End of game: white won");
                } else if (turn == Turn::BlackWin) {
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
