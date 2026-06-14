#include "Log.hpp"
// #include "Thread.hpp"
#include "InetAddr.hpp"
#include "Comm.hpp"
#include "UdpServer.hpp"

using namespace LogModule;
// using namespace MyThread;


int main(int argc, char *argv[])
{
    if(argv[1] == nullptr){
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return Usage_Err;
    }
    CONSOLELOG();
    // struct sockaddr_in addr;
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(std::stoi(argv[1]));
    // addr.sin_addr.s_addr = INADDR_ANY;

    UdpServer server(std::stoi(argv[1]));
    server.Init();
    server.Start();
    return 0;
}
