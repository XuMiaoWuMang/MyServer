
#include "ThreadPool.hpp"
#include "InetAddr.hpp"
#include <iostream>

#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <pthread.h>

using namespace ThreadPoolModule;

struct UsrData
{
    int num;
};
void Push(UsrData *data)
{
    (void *)data;
    int count = 3;
    while (count--)
    {
        LOG(LogLevel::Info) << "我是一个push函数";
        sleep(1);
    }
}
void Get(UsrData *data)
{
    (void *)data;
    int count = 3;
    while (count--)
    {
        LOG(LogLevel::Info) << "我是一个get函数";
        sleep(1);
    }
}
void Remove(UsrData *data)
{
    (void *)data;
    int count = 3;
    while (count--)
    {
        LOG(LogLevel::Info) << "我是一个remove函数";
        sleep(1);
    }
}
using func1_t = std::function<void(UsrData*)>;

int main()
{
    auto tp = ThreadPool<func1_t, UsrData *>::GetInstance();
    tp->Start();

    tp->Equeue(Push);
    tp->Equeue(Get);
    tp->Equeue(Remove);
    sleep(3);
    LOG(LogLevel::Info) << "等待线程结束...";
    tp->Stop();
    sleep(3);

    tp->Wait();
    return 0;
}