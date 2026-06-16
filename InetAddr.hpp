#pragma once
#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Comm.hpp"

class InetAddr
{
public:
    InetAddr(std::string ip, uint16_t port)
    {
        _addr.sin_family = AF_INET;
        _addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &_addr.sin_addr);
        _ip = ip;
        _port = port;
    }
    InetAddr(struct sockaddr_in &addr)
    {
        _addr = addr;
        inet_ntop(AF_INET, &addr.sin_addr, _ip.data(), 16);
        _port = ntohs(addr.sin_port);
    }

    struct sockaddr_in& GetAddr()
    {
        return _addr;
    }
    std::string GetIp()
    {
        return _ip;
    }
    uint16_t GetPort()
    {
        return _port;
    }
    std::string ToString()
    {
        return _ip + ":" + std::to_string(_port);
    }
    bool operator==(const InetAddr &other) const
    {
        return _ip == other._ip && _port == other._port;
    }
    ~InetAddr() = default;
private:
    struct sockaddr_in _addr;
    std::string _ip = std::string(17, '\0');
    uint16_t _port;
};