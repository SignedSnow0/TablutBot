#pragma once
#include <cstdint>
#include <string>

#include <nlohmann/json.hpp>

class Socket {
public:
    Socket(const std::string &ip, const uint32_t port);
    ~Socket();

    [[nodiscard]] inline const std::string &Ip() const { return mIp; }
    [[nodiscard]] inline uint32_t Port() const { return mPort; }

    [[nodiscard]] std::string Receive() const;
    void Send(const std::string &msg) const;

private:
    int mSocket;

    const std::string mIp;
    const uint32_t mPort;
};
