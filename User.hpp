#pragma once
#include <iostream>
#include <string>
#include <vector>
#include "InetAddr.hpp"
#include "Log.hpp"
using namespace LogModule;
/*
用户模块，保存用户信息的类，管理用户的模块，信息转发模块。
*/
class UserInterface
{
public:
    virtual ~UserInterface() = default;
    virtual void SendTo(int sockfd, const std::string &message) = 0;
    virtual bool operator==(const InetAddr &u) = 0;
    virtual std::string Id() = 0;
};

class User : public UserInterface
{
public:
    User(InetAddr &addr) : _id(addr)
    {
    }
    virtual ~User()
    {
    }
    virtual void SendTo(int sockfd, const std::string &message) override
    {
        sendto(sockfd, message.c_str(), message.size(), 0, (const sockaddr *)&_id.GetAddr(), sizeof(_id.GetAddr()));
        // LOG(LogLevel::Info) << "成功向" << _id.ToString() << "发送: " << message;
    }
    virtual bool operator==(const InetAddr &u) override
    {
        return u == _id;
    }
    virtual std::string Id() override
    {
        return _id.ToString();
    }
    InetAddr GetAddr()
    {
        return _id;
    }

private:
    InetAddr _id;
};
class UserManager
{
public:
    UserManager()
    {
    }
    ~UserManager()
    {
    }

public:
    bool IsExist(InetAddr &inetAddr)
    {
        for (auto &addr : _route)
        {
            if (addr == inetAddr)
            {
                return true;
            }
        }
        return false;
    }

    bool AddUser(InetAddr &inetAddr)
    {
        if (IsExist(inetAddr))
        {
            LOG(LogLevel::Debug) << "用户已存在...";
            return false;
        }

        _route.push_back(inetAddr);
        LOG(LogLevel::Debug) << "用户添加成功...";
        return true;
    }

    bool DelUser(InetAddr &inetAddr)
    {
        if (!IsExist(inetAddr)) // 判断用户是否存在
        {
            LOG(LogLevel::Error) << "用户不存在...";
            return false;
        }

        for (auto iter = _route.begin(); iter != _route.end(); iter++)
        {
            if (*iter == inetAddr)
            {
                _route.erase(iter);
                LOG(LogLevel::Debug) << "用户已删除...";
                break;
            }
        }
        return true;
    }
    void PrintUser()
    {
        for (auto user : _route)
        {
            LOG(LogLevel::Debug) << "在线用户-> " << user.Id();
        }
    }

    // public:
    void MsgRoute(int fd, std::string &data, InetAddr &src)
    {
        if (data == "quit")
        {
            DelUser(src);
            LOG(LogLevel::Info) << src.ToString() << "退出...";
            return;
        }
        struct sockaddr peer;
        std::string msg = src.ToString() + "# " + data;
        for (auto &addr : _route)
        {
            addr.SendTo(fd, msg);
            LOG(LogLevel::Info) << "成功向" << addr.Id() << "发送: " << data;
        }
    }

private:
    std::vector<User> _route; // 使用 InetAddr 作为用户的最小单位
};