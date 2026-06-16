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

/*
重新编排项目架构。
该文件旨在使用Thread封装pthread线程的各种调用方法，方便后续的使用和拓展。
它不需要关注自己的具体任务是什么，只需要能够创建之后等待任务，然后执行即可。
值得一提的是，等待任务并不需要固定好任务类型，参数也不需要固定，所以我觉得使用两个模板办法。
第一个模板是执行函数的类型，第二个模板是执行函数所需的参数类型。
调用者可以将需要返回的数据保存在第二个模板中。
不，不对，不需要第一个模板，执行函数的类型模仿pthread_create就行,只接受任意一个类型的参数。
也许需要先思考，Thread线程是否需要持久化。
作为Thread线程的最小单位，不需要考虑持久化，职责越小越好，那么这个最小单位的作用就是，执行上层传递的函数。
如果需要持久化，那么调用者需要先写好一个循环函数，作为构造函数的参数。
*/
using namespace LogModule;
namespace MyThread
{
    static int number = 1;
    enum class TSTATUS
    {
        NEW,
        RUNNING,
        STOP
    };
    template <typename T = void*>
    using func_t = std::function<void(T)>;
    template <typename T = void*>
    class Thread
    {
    public:
        Thread(func_t<T> func)
            : _threadId(0), 
              _status(TSTATUS::NEW), 
              _joinable(false), // 默认不分离，子线程交由main线程管理
              _func(func)
        {
            // 构造名称，number是静态变量，不必担心重名，只是不会回退number，导致线程名只会一直增大
            _name = "Thread-" + std::to_string(number++); 
        }

        void Start()
        {
            if (_status == TSTATUS::RUNNING)
            {
                return;
            }
            int n = ::pthread_create(&_threadId, nullptr, Routine, this);
            _status = TSTATUS::RUNNING;
        }

        bool Stop()
        {
            if (_status == TSTATUS::RUNNING)
            {
                int n = ::pthread_cancel(_threadId);
                if (n != 0)
                {
                    return false;
                }

                _status = TSTATUS::STOP;
                return true;
            }
            return false;
        }

        bool Join()
        {
            if (_joinable)
            {
                int n = ::pthread_join(_threadId, nullptr);
                if (n != 0)
                {
                    return false;
                }

                _status = TSTATUS::STOP; // 设置 STOP 状态，防止又join又cancel
                return true;
            }
            return false;
        }

        void Detach()
        {
            EnableDetach();
            pthread_detach(_threadId);
        }

        bool IsJoinable() { return _joinable; } // 判断是否分离
        std::string Name() { return _name; } // 获取线程名称

        T &Data()
        {
            return _data; // 获取用户数据
        }

        void SetData(T &data)
        {
            _data = data; // 如果是自定义类型需要注意符号重载
        }

    private:
        static void *Routine(void *arg)
        {
            Thread<T> *t = static_cast<Thread<T> *>(arg);

            t->_status = TSTATUS::RUNNING; // 线程状态设置为RUNNING
            t->_func(t->Data()); // 执行真正的函数

            return nullptr;
        }
        void EnableDetach()
        {
            _joinable = false;
        }

    private:
        // 信息变量
        pthread_t _threadId;
        std::string _name;

        TSTATUS _status; // 状态
        bool _joinable;  // 默认不分离（detach）
        std::mutex _mutex;
        std::condition_variable _cond;

        func_t<T> _func;
        T _data;
    };


}