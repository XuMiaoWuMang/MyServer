#pragma once
#include <iostream>
#include <string>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <unordered_map>
#include "Log.hpp"
#include "Comm.hpp"


class UdpSocket
{
public:
    UdpSocket(uint16_t port = defaultport)
        : _port(port),
          _socketfd(defaultfd)
    {
    }
    int Socket()
    {
        _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_socketfd < 0)
        {
            LOG(LogLevel::Error) << "socket error: " << strerror(errno);
            return (Socket_Err);
        }
        LOG(LogLevel::Debug) << "socket created: " << _socketfd;
        return _socketfd;
    }
    bool Bind()
    {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(_port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if (::bind(_socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        {
            LOG(LogLevel::Error) << "bind error: " << strerror(errno);
            return (Bind_Err);
        }
        LOG(LogLevel::Debug) << "bind success: " << _socketfd;
        return true;
    }
    bool Sendto(std::string &data, std::string ip, uint16_t port)
    {
        struct sockaddr_in addr;
        bzero(&addr, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);
        ssize_t n = sendto(_socketfd, data.c_str(), data.size(), 0, (struct sockaddr *)&addr, sizeof(addr));
        if (n < 0)
        {
            LOG(LogLevel::Error) << "sendto error: " << strerror(errno);
            return false;
        }
        return true;
    }
    bool Recver(std::string *data, std::string* ip = NULL, uint16_t* port = NULL)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        char inbuffer[defaultsize];
        int n = recvfrom(_socketfd, inbuffer, defaultsize, 0, (struct sockaddr *)&peer, &len);
        if(n > 0)
        {
            data->assign(inbuffer, n);
        }else{
            LOG(LogLevel::Error) << "Recv error";
            return false;
        }
        if(ip != nullptr)
            *ip = inet_ntoa(peer.sin_addr);
        if(port != nullptr)
            *port = ntohs(peer.sin_port);
        return true;
    }
    bool Close()
    {
        close(_socketfd);
        return true;
    }

    int GetSocketFd()
    {
        return _socketfd;
    }
    uint16_t GetPort()
    {
        return _port;
    }

    ~UdpSocket()
    {
    }

private:
    uint16_t _port;
    int _socketfd;
};