#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <string>
#include <unordered_map>
#include <functional>
#include <pthread.h>
#include "Log.hpp"

using namespace LogModule;

namespace MyThread
{
    const unsigned int DefaultCount = 5;            // 默认线程池只有5个线程
    static unsigned int ThreadCount = DefaultCount; // 线程池线函数未就绪程数量
    std::mutex _gMutex;                             // 互斥锁
    std::condition_variable _gcond;                 // 条件变量
    using func_t = std::function<void(void *)>;     // 任务函数

    class Thread
    {
    private:
        // 线程处理函数, 解决函数类型不同的问题
        static void *headlerFunc(void *arg)
        {
            Thread *self = static_cast<Thread *>(arg);
            while (true)
            {
                LOG(LogLevel::Debug) << self->_threadName << "等待中...";
                sleep(1);
                {
                    std::unique_lock<std::mutex> lock(_gMutex); // 先上锁
                    // 如果不符合条件，就释放并等待 ReadyCount
                    _gcond.wait(lock, [&]
                                { return self->_func != nullptr; });

                    LOG(LogLevel::Debug) << self->_threadName << "已就绪";
                }
                self->_func(nullptr);
                self->SetFunc(nullptr);
                {
                    std::lock_guard<std::mutex> lock(_gMutex);
                    ThreadCount++;
                }

                // std::cout << self->_threadName << " is finish" << std::endl;
            }
            return nullptr;
        }

    public:
        // 初始化线程, 并设置运行状态为false, 函数就绪状态为false
        Thread(std::string threadName, func_t func = nullptr)
            : _threadName(threadName),
              _func(func),
              _running(false)
        //   ,_ready(false)
        {
            LOG(LogLevel::Info) << _threadName << "已创建";
        }
        // 启动线程，设置运行状态为true
        void Start()
        {
            // 如果线程已运行, 则直接返回
            if (isRunning())
                return;
            pthread_create(&_threadId, nullptr, headlerFunc, this);
            _running = true;
            LOG(LogLevel::Info) << _threadName << "已运行";
        }

        // 设置线程函数, 并设置函数就绪状态为true
        void SetFunc(func_t func)
        {
            // 如果函数就绪, 则直接返回
            // if (isReady())
            //     return;
            std::lock_guard<std::mutex> lock(_mutex);

            _func = func;
        }
        void Join()
        {
            // 如果线程未运行, 则直接返回
            if (!isRunning())
                return;
            pthread_join(_threadId, nullptr);
            _running = false;
        }
        ~Thread()
        {
        }
        bool isRunning()
        {
            std::lock_guard<std::mutex> lock(_mutex);
            return _running;
        }
        // bool isReady()
        // {
        //     std::lock_guard<std::mutex> lock(_mutex);
        //     return _ready;
        // }
        std::string getThreadName()
        {
            return _threadName;
        }

    private:
        pthread_t _threadId;
        std::string _threadName;
        // bool _ready = false;
        func_t _func = nullptr;
        bool _running = false;
        std::mutex _mutex;
    };

    class ThreadsManager
    {
    public:
        ThreadsManager()
        {
        }

        ThreadsManager(unsigned int Count)
        {
            std::lock_guard<std::mutex> lock(_gMutex);

            for (unsigned int i = 0; i < Count; i++)
            {
                std::string threadName = "Thread-" + std::to_string(i);
                _threads.emplace(threadName, threadName);
                _threads.at(threadName).Start();
            }
        }
        void DebugPrint()
        {
            for (auto &thread : _threads)
            {
                LOG(LogLevel::Debug) << thread.second.getThreadName();
            }
        }
        bool Dispatcher(func_t func)
        {
            {
                std::unique_lock<std::mutex> lock(_gMutex);
                _gcond.wait(lock, [&]
                            { return ThreadCount > 0; });
                ThreadCount--;
                LOG(LogLevel::Debug) << "Dispatcher: " << ThreadCount << " threads are ready";
            }
            for (auto &thread : _threads)
            {
                // 如果线程未运行, 则直接返回
                if (!thread.second.isRunning())
                    continue;
                LOG(LogLevel::Debug) << "Dispatcher: " << thread.second.getThreadName() << " is ready";
                thread.second.SetFunc(func);
                _gcond.notify_all();

                return true;
            }

            return false;
        }
        ~ThreadsManager()
        {
        }

    private:
        std::unordered_map<std::string, Thread> _threads;
        std::mutex _mutex;
    };
    static ThreadsManager HandleManager = ThreadsManager(DefaultCount);

}