#include "Socket.h"
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef __unix__
#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Socket(const std::string &ip, const uint32_t port)
    : mIp(ip), mPort(port) {
    struct addrinfo hints, *res, *p;
    int sock;
    char buffer[1024];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status =
        getaddrinfo(ip.c_str(), std::to_string(port).c_str(), &hints, &res);
    if (status != 0) {
        throw std::runtime_error("Failed to resolve ip " + ip + ":" +
                                 std::to_string(port) + " (" +
                                 gai_strerror(status) + ")");
    }

    for (p = res; p != nullptr; p = p->ai_next) {
        sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (sock == -1)
            continue;

        if (connect(sock, p->ai_addr, p->ai_addrlen) == 0) {
            mSocket = sock;
            return;
        }

        close(sock);
    }

    throw std::runtime_error("Failed to connect to " + ip + ":" +
                             std::to_string(port));
}

Socket::~Socket() { close(mSocket); }

std::string Socket::Receive() const {
    uint32_t string_size{0};
    read(mSocket, &string_size, sizeof(uint32_t));
    string_size = htonl(string_size);

    std::string msg;
    msg.resize(string_size);

    if (read(mSocket, msg.data(), string_size) <= 0) {
        return "";
    }

    return msg;
}

void Socket::Send(const std::string &msg) const {
    uint32_t msg_length = ntohl(static_cast<uint32_t>(msg.size()));
    write(mSocket, &msg_length, sizeof(msg_length));

    size_t sent = write(mSocket, msg.data(), msg.size());
    if (sent != msg.size()) {
        std::cerr << "Failed to send to socket: expected " << msg.size()
                  << ", sent " << sent << std::endl;
    }
}
#endif
