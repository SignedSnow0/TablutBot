#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

class Socket {
public:
    static std::shared_ptr<Socket> Connect(const std::string &ip,
                                           const uint32_t port) {
        Socket *socket = new Socket(ip, port);
        return std::shared_ptr<Socket>(socket);
    };
    Socket() = delete;
    Socket(Socket &) = delete;
    ~Socket();

    [[nodiscard]] inline const std::string &Ip() const { return mIp; }
    [[nodiscard]] inline uint32_t Port() const { return mPort; }

    [[nodiscard]] std::string Receive() const;
    void Send(const std::string &msg) const;

private:
    Socket(const std::string &ip, const uint32_t port);

    uint32_t mSocket;
    const std::string mIp;
    const uint32_t mPort;
};
