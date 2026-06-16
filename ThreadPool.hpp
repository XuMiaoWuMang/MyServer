#pragma once
#include <iostream>
#include <mutex>
#include <condition_variable>
#include <unistd.h>
#include <string>
#include <vector>
#include <queue>
#include "Log.hpp"
#include "Thread.hpp"

/*
该文件旨在实现 线程池 。
通过管理任务队列和线程指针数组，实现队列入任务，数组取任务。
是一个简单的消费者-生产者模型。
*/

namespace ThreadPoolModule
{
    using namespace LogModule;
    using namespace MyThread;

    const static int gdefaultthreadnum = 10; // 默认只构造10个线程

    // 使用两个模板参数，第一个是任务的模板，第二个是数据类的模板

    template <typename T, typename D = void *>
    class ThreadPool
    {
    private:
        bool IsEmpty()
        {
            return _task_queue.empty();
        }
        // 线程池线程处理函数
        void HandlerTask(D data)
        {
            while (_running)
            {
                T task;
                // D *d = static_cast<D *>(data);
                {
                    // 安全地获取任务
                    std::unique_lock<std::mutex> lock(_mutex);
                    _waitnum++;
                    _cond.wait(lock, [&]
                               { return IsRunning() && !IsEmpty(); });

                    _waitnum--;
                    task = _task_queue.front();
                    _task_queue.pop();
                }
                // 处理任务
                task(data);
            }
        }

    public:
        // 注意是管理 线程指针 ，不是一个个线程单位。
        // 构造函数私有化，防止外部直接创建对象
        ThreadPool()
            : _threadnum(gdefaultthreadnum)
        {
            for (int i = 0; i < _threadnum; i++)
            {
                // make_share构造线程指针，类型填入线程类型，参数填入执行函数，执行函数是线程池需要提前写好的循环函数，用于持久化线程
                // 执行函数需要一个 模板 参数。
                _threads.push_back(
                    std::make_shared<Thread<D>>([this](D data)
                                                { this->HandlerTask(data); }));
                LOG(LogLevel::Info) << "构建线程" << _threads.back()->Name() << "对象 ... 成功";
            }
        }
        ThreadPool(int count)
            : _threadnum(count)
        {
            for (int i = 0; i < _threadnum; i++)
            {
                _threads.push_back(
                    std::make_shared<Thread<D>>([this](D data)
                                                { this->HandlerTask(data); }));
                LOG(LogLevel::Info) << "构建线程" << _threads.back()->Name() << "对象 ... 成功";
            }
        }

        // 任务队列入任务
        void Equeue(T &&task)
        {
            std::lock_guard lock(_mutex);
            if (!_running)
                return;
            _task_queue.push(task);
            if (_waitnum > 0)
            {
                _cond.notify_all();
            }
        }

        bool IsRunning()
        {
            return _running;
        }

        // 启动线程池，创建线程
        void Start()
        {
            if (IsRunning())
                return;
            _running = true;
            for (auto &thread : _threads)
            {
                thread->Start();
            }
        }
        void Wait()
        {
            for (auto &thread_ptr : _threads)
            {
                thread_ptr->Join();
                LOG(LogLevel::Info) << "回收线程" << thread_ptr->Name() << " ... 成功";
            }
        }

        void Stop()
        {
            std::lock_guard lock(_mutex);
            if (_running)
            {
                // 3. 不能在入任务了
                _running = false; // 不工作
                // 1. 让线程自己退出(要唤醒) && // 2. 历史的任务被处理完了
                if (_waitnum > 0)
                    _cond.notify_all();
            }
        }

    public:
        // 只有第⼀次会创建对象，后续都是获取
        // 双判断的⽅式，可以有效减少获取单例的加锁成本，⽽且保证线程安全
        static ThreadPool<T, D> *GetInstance()
        {
            if (nullptr == _instance)
            // 保证第⼆次之后，所有线程，不⽤在加锁，直接返回_instance单例对象
            {
                std::lock_guard<std::mutex> lockguard(_mutex);
                if (nullptr == _instance)
                {
                    _instance = new ThreadPool<T, D>();
                    _instance->Start();
                    LOG(LogLevel::Debug) << "创建线程池单例";
                    return _instance;
                }
            }
            LOG(LogLevel::Debug) << "获取线程池单例";
            return _instance;
        }

        ~ThreadPool()
        {
        }

        // 禁止拷贝构造和赋值操作
        ThreadPool<T, D> &operator=(const ThreadPool<T, D> &) = delete;
        ThreadPool(const ThreadPool<T, D> &) = delete;

    private:
        std::vector<std::shared_ptr<Thread<D>>> _threads; // 线程池线程容器
        std::queue<T> _task_queue;                        // 任务队列
        unsigned int _threadnum;                          // 线程池线程数量
        bool _running = false;                            // 线程池运行状态
        int _waitnum = 0;                                 // 等待线程数量

        static std::mutex _mutex; // 线程池单例对象的互斥锁
        std::condition_variable _cond;
        static ThreadPool<T, D> *_instance; // 线程池单例对象
    };

    template <typename T, typename D>
    ThreadPool<T, D> *ThreadPool<T, D>::_instance = nullptr;

    template <typename T, typename D>
    std::mutex ThreadPool<T, D>::_mutex;
}