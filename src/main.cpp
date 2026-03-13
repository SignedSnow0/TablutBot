#include <iostream>
#include <string>

#include "connection/Socket.h"
#include "state/Tablut.h"

#define WHITE_PORT 5800
#define BLACK_PORT 5801

#define BOT_NAME_WHITE "TablutBotWhite"
#define BOT_NAME_BLACK "TablutBotBlack"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " role serverIp timeout"
                  << std::endl;
    }

    auto tablut = Tablut::InitalConfiguration();
    std::string player(argv[1]);
    if (player == "WHITE" || player == "white") {
        auto socket = Socket(argv[2], WHITE_PORT);
        socket.Send(BOT_NAME_WHITE);

        std::string msg = socket.Receive();
        while (msg != "") {
            std::cout << "Received msg: " << msg << std::endl;

            msg = socket.Receive();
        }
    } else {
        auto socket = Socket(argv[2], BLACK_PORT);
        socket.Send(BOT_NAME_BLACK);

        std::string msg = socket.Receive();
        while (msg != "") {
            std::cout << "Received msg: " << msg << std::endl;

            msg = socket.Receive();
        }
    }

    return 0;
}
