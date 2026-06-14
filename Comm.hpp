#pragma once
const static uint16_t defaultport = 8888;
const static int defaultfd = -1;
const static int defaultsize = 1024;
using namespace LogModule;
enum{
Usage_Err = 1,
Socket_Err,
Bind_Err,
Recv_Err
};