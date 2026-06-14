#pragma once
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <ctime>

namespace LogModule
{
    // #define GetConsoleLogger() (LOG = new ConsoleLogger())
    // #define GetFileLogger() (LOG = new FileLogger())
    // #define LOG(level) (LogMessage(level, ""))
    const std::string LogFilePath = "./logs/";
    const std::string LogFileName = "log";
    const std::string LogFileSuffix = ".txt";

    // 日志级别
    // Debug: 调试日志
    // Info: 信息日志
    // Warn: 警告日志
    // Error: 错误日志
    // Fatal: 致命错误日志
    // 等级越高，记录越重要
    enum LogLevel
    {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Fatal = 4,
    };

    // 日志策略 基类
    class strategy
    {
    public:
        virtual void Log(const std::string &msg) = 0;
    };
    // 命令行打印的日志策略
    class ConsoleStrategy : public strategy
    {
    public:
        // 将日志输出到命令行中
        void Log(const std::string &msg) override
        {
            {
                std::lock_guard<std::mutex> glock(Cmutex);
                std::cout << msg << std::endl;
            }
        }

    private:
        std::mutex Cmutex;
    };
    // 保存到文件的日志策略
    class FileStrategy : public strategy
    {
    public:
        // 初始化判断目录是否存在，不存在就创建目录
        FileStrategy()
        {
            {
                std::lock_guard<std::mutex> glock(Fmutex);
                if (std::filesystem::exists(LogFilePath))
                    return;
                try
                {
                    std::filesystem::create_directories(LogFilePath);
                }
                catch (const std::filesystem::filesystem_error &e)
                {
                    std::cerr << e.what() << '\n';
                }
            }
        }
        void Log(const std::string &msg) override
        {
            // 将日志保存到 ./logs/log.txt 文件中
            {
                std::string filename;
                // todo: 将日志文件分割，避免日志文件过大
                // for (int i = 1;; i++)
                // {
                //     std::stringstream ss;
                //     ss << LogFilePath << LogFileName << i << LogFileSuffix;
                //     if (std::filesystem::exists(ss.str()))
                //     {
                //         filename = ss.str();
                //     }
                // }
                filename += LogFilePath + LogFileName + LogFileSuffix;
                std::ofstream ofs(filename.c_str(), std::ios::app);
                if (!ofs.is_open())
                    return;
                ofs << msg << std::endl;
                ofs.close();
            }
        }

    private:
        std::mutex Fmutex;
    };

    std::string GetCurrTime()
    {
        time_t tm = time(nullptr);
        struct tm curr;
        localtime_r(&tm, &curr);
        std::stringstream ss;
        ss << curr.tm_year + 1900 << "/"
           << curr.tm_mon << "/"
           << curr.tm_mday << " "
           << curr.tm_hour << ":"
           << curr.tm_min << ":"
           << curr.tm_sec;
        return ss.str();
    }
    std::string Level2Str(LogLevel Level)
    {
        switch (Level)
        {
        case Debug:
            return "Debug";
        case Info:
            return "Info";
        case Warn:
            return "Warn";
        case Error:
            return "Error";
        case Fatal:
            return "Fatal";
        default:
            return "Unknown";
        }
    }

    // 日志控制类
    class Logger
    {
    public:
        Logger() { UseConsoleStrategy(); }

        void UseConsoleStrategy() { _strategy = std::make_unique<ConsoleStrategy>(); }
        void UseFileStrategy() { _strategy = std::make_unique<FileStrategy>(); }

        // 日志输出类
        class LogMessage
        {
        public:
            LogMessage(LogLevel level, std::string FileName, int Line, Logger &logger)
                : _level(level), _curr_time(GetCurrTime()), _logger(logger)
            {
                std::stringstream ss;
                ss << "[" << _curr_time << "] ";
                ss << "[" << Level2Str(_level) << "] ";
                ss << "[" << FileName << "] ";
                ss << "[" << Line << "]: ";

                _info = ss.str();
            }

            // 析构函数修改成 调用对应日志策略的打印方式
            ~LogMessage()
            {
                _logger._strategy->Log(_info);
            }

            // 像 cout 一样可以多次使用 << 添加输出内容
            template <typename T>
            LogMessage &operator<<(const T &msg)
            {
                std::stringstream ss;
                ss << msg;
                _info += ss.str();
                return *this;
            }

        private:
            LogLevel _level;
            std::string _curr_time; // ⽇志时间
            std::string _info;
            Logger &_logger;
        };

        // 方便调用,重载 () ,像函数一样调用日志
        LogMessage operator()(LogLevel level, std::string FileName, int Line)
        {
            return LogMessage(level, FileName, Line, *this);
        }

    private:
        std::unique_ptr<strategy> _strategy;
    };
    Logger logger;
} // namespace LogModule
#define LOG(level) (logger(level, __FILE__, __LINE__)) 
#define CONSOLELOG() logger.UseConsoleStrategy()
#define FILELOG() logger.UseFileStrategy()
