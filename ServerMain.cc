#include "UdpServer.hpp"
#include "ThreadPool.hpp"
// #include "Log.hpp"
void Usage(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(Usage_Err);
    }
}

// using namespace LogModule;
using namespace ThreadPoolModule;
using ser_task_t = std::function<void(int, std::string &, InetAddr &)>;

int main(int argc, char *argv[])
{
    CONSOLELOG();
    Usage(argc, argv);

    // // 创建路由服务器
    // auto tp = ThreadPool<ser_task_t>::GetInstance();

    // // std::unique_ptr<ThreadPool<ser_task_t>> tp = std::make_unique<ThreadPool<ser_task_t>>();
    // tp->Start();

    UdpServer ser(std::stoi(argv[1]));
    ser.Init();
    ser.Start();


    return 0;
}