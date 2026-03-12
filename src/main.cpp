#include <iostream>
#include <string>

#include "connection/Socket.h"
#include "state/Tablut.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " serverIp serverPort timeout"
                  << std::endl;
    }

    auto tablut = Tablut::InitalConfiguration();
    tablut.Move(0, 3, 0, 2);
    std::cout << PrintGrid(tablut) << std::endl;

    std::cout << "Valid moves for position (0, 2): [";
    for (const auto &[row, column] : tablut.GenMoves(0, 2)) {
        std::cout << "(" << std::to_string(row) << ", "
                  << std::to_string(column) << "), ";
    }
    std::cout << "]" << std::endl;
    std::cout << "Valid moves for position (0, 4): [";
    for (const auto &[row, column] : tablut.GenMoves(0, 4)) {
        std::cout << "(" << std::to_string(row) << ", "
                  << std::to_string(column) << "), ";
    }
    std::cout << "]" << std::endl;

    return 0;
}
