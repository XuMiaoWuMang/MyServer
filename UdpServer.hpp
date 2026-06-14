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

// #include "nocopy.hpp"
#include "Log.hpp"
#include "Comm.hpp"
#include "InetAddr.hpp"
#include "Dict.hpp"
#include "Udp_socket.hpp"

class UdpServer : public UdpSocket
{
public:
    UdpServer(uint16_t port = defaultport)
        : UdpSocket(port)
    {
    }
    ~UdpServer()
    {
    }
    void Init()
    {
        // 1. 创建socket
        // _socketfd = socket(AF_INET, SOCK_DGRAM, 0);
        // if (_socketfd < 0)
        // {
        //     LOG(LogLevel::Error) << "socket error: " << strerror(errno);
        //     exit(Socket_Err);
        // }
        // LOG(LogLevel::Debug) << "socket created: " << _socketfd;
        Socket();

        // 2. 绑定网络信息
        // struct sockaddr_in addr;
        // bzero(&addr, sizeof(addr));
        // addr.sin_family = AF_INET;
        // addr.sin_port = htons(_port);
        // addr.sin_addr.s_addr = INADDR_ANY;
        // 3. bind到内核中
        // if (bind(_socketfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        // {
        //     LOG(LogLevel::Error) << "bind error: " << strerror(errno);
        //     exit(Bind_Err);
        // }
        // LOG(LogLevel::Debug) << "bind success: " << _socketfd;
        Bind();
    }
    void Start()
    {
        std::string buffer;
        _Mydict = Dict();
        // 死循环，不退出
        while (true)
        {
            // struct sockaddr_in peer;
            // bzero(&peer, sizeof(peer));
            // socklen_t len = sizeof(peer);
            std::string ipinfo;
            uint16_t portinfo;
            // 接受消息
            ssize_t n = Recver(&buffer, &ipinfo, &portinfo);
            if (n > 0)
            {
                // 读取成功
                InetAddr inetAddr(ipinfo, portinfo);

                buffer[buffer.size()] = '\0';
                LOG(LogLevel::Debug) << "recv data: " << buffer;
                std::string key(buffer);
                std::string value = _Mydict.Translate(key);
                Sendto(value, inetAddr.GetIp(), inetAddr.GetPort());
                LOG(LogLevel::Debug) << "sendto data: " << value;
            }
        }
    }

private:
    // int CreateSocket()
    // {
    // }
    // void Bind();
    // void Recv();
    // void Send(std::string msg, InetAddr &clientAddr);

private:
    char _buffer[defaultsize];
    Dict _Mydict;
};