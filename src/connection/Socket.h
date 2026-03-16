#pragma once
#include <cstdint>
#include <memory>
#include <string>

#include <nlohmann/json.hpp>

/*
 * Represents a tcp connection to a server.
 */
class Socket {
public:
    /*
     * Connects to the server at the given ip and port.
     * @param ip The ip of the server to connect to, can be a hostname or IP
     * address.
     * @param port The port of the server to connect to.
     * @return A shared pointer to a Socket connected to the server, the caller
     * is responsible for closing the socket when it is no longer needed.
     */
    static std::shared_ptr<Socket> Connect(const std::string &ip,
                                           const uint32_t port) {
        Socket *socket = new Socket(ip, port);
        return std::shared_ptr<Socket>(socket);
    };
    Socket() = delete;
    Socket(Socket &) = delete;
    ~Socket();

    /*
     * Returns the ip of the server this socket is connected to.
     */
    [[nodiscard]] inline const std::string &Ip() const { return mIp; }
    /*
     * Returns the port of the server this socket is connected to.
     */
    [[nodiscard]] inline uint32_t Port() const { return mPort; }

    /*
     * Receives a message from the server, the method blocks until a message is
     * received.
     * @return The message received from the server.
     */
    [[nodiscard]] std::string Receive() const;
    /*
     * Sends a message to the server.
     * @param msg The message to send to the server.
     */
    void Send(const std::string &msg) const;

private:
    Socket(const std::string &ip, const uint32_t port);

    uint32_t mSocket;
    const std::string mIp;
    const uint32_t mPort;
};
