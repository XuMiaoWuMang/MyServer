# ChatServer And ChatClient
基于Udp通信搭建的聊天室

## 原理
通过Udp通信，将用户的信息互相发送。

## 使用方式

clone之后，依次执行，其中port更换成你想换的端口。

服务端：
```bash
make 
./UdpServer <port>
```

客户端：
```bash
./UdpClinet <ip> <port>
```

若修改了代码，请使用`make clean`清理。

---

修改时间：2026/6/16
> 如无意外，将不再更新