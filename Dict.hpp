#pragma once
#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <cerrno>
#include <cstring>
#include "Log.hpp"

using namespace LogModule;

const std::string sep = ": ";

class Dict
{
public:
    Dict():_dict(std::unordered_map<std::string, std::string>())
    {
        LoadDict();
    }
    std::string Translate(std::string &key)
    {
        auto it = _dict.find(key);
        if (it != _dict.end())
        {
            return it->second;
        }
        return "Unkown";
    }
    ~Dict()
    {
    }

private:
    void LoadDict()
    {
        std::filesystem::path dictPath = "./dict.txt";
        if (!std::filesystem::exists(dictPath))
        {
            LOG(LogLevel::Error) << "dict.txt not found";
            return;
        }
        std::ifstream dictFile(dictPath);
        if (!dictFile.is_open())
        {
            LOG(LogLevel::Error) << "open dict.txt error: " << strerror(errno);
            return;
        }
        std::string line;
        while (std::getline(dictFile, line))
        {
            std::istringstream iss(line);
            std::string k, v;
            auto pos = line.find(sep);
            if (pos == std::string::npos)
                continue;
            k = line.substr(0, pos);
            v = line.substr(pos + sep.size());
            _dict[k] = v;
        }
        dictFile.close();
    }
    std::unordered_map<std::string, std::string> _dict;
};
