#pragma once
#include <cstdint>
#include <string>

class Socket {
public:
    Socket(const std::string &ip, const uint32_t port);
    ~Socket();

    [[nodiscard]] inline const std::string &Ip() const { return mIp; }
    [[nodiscard]] inline uint32_t Port() const { return mPort; }

private:
    int mSocket;

    const std::string mIp;
    const uint32_t mPort;
};
