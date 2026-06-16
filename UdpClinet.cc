#include <iostream>
#include <cerrno>
#include <cstring>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
/* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Log.hpp"
#include "Comm.hpp"
#include "InetAddr.hpp"

using namespace LogModule;
struct sockaddr_in addr;
int sockfd = -1;
void *Recv(void *)
{
    char buffer[1024];
    while (true)
    {
        struct sockaddr_in peer;
        socklen_t len = sizeof(peer);
        ssize_t recvlen = recvfrom(sockfd, buffer, 1024, 0, (struct sockaddr *)&peer, &len);
        if (recvlen > 0)
        {
            buffer[recvlen] = 0;
            std::cerr << std::endl << buffer << std::endl;
        }
    }
}

int main(int argc, char *argv[])
{
    CONSOLELOG();
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <ip> <port>" << std::endl;
        return Usage_Err;
    }

    std::string ip = argv[1];
    uint16_t port = std::stoi(argv[2]);

    // 1. 创建socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        // LOG(LogLevel::Error) << "socket error: " << strerror(errno);
        exit(Socket_Err);
    }
    // LOG(LogLevel::Debug) << "socket created: " << sockfd;

    // 2. 绑定地址
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &addr.sin_addr);

    pthread_t _recv_id;
    pthread_create(&_recv_id, nullptr, Recv, nullptr);
    while (true)
    {
        std::string inbuffer;
        std::cout << "Please Enter# ";
        getline(std::cin, inbuffer);
        if (inbuffer.empty())
            continue;
        ssize_t sendlen = sendto(sockfd, inbuffer.c_str(), inbuffer.size(), 0, (struct sockaddr *)&addr, sizeof(addr));
        // if (sendlen > 0)
        // {
        //     // char recvbuffer[1024];
        //     // InetAddr inetAddr(addr);
        //     // LOG(LogLevel::Debug) << "Sent to " << inetAddr.ToString() << " - " << inbuffer;

        // }
        // else
        //     exit(-1);
    }
    close(sockfd);
}
