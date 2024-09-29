#ifndef MYCHATSERVER_H
#define MYCHATSERVER_H
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<string>
#include<functional>
using namespace std::placeholders;
using namespace muduo::net;
class MyChatServer{
private:
   TcpServer MyServer;
   EventLoop* MyEventLoopt;
   void MyConnectionCallBack(const TcpConnectionPtr&TCP);
   void MySetMessageCallBack(const TcpConnectionPtr&TCP,Buffer*bf,muduo::Timestamp ConnectionTime);
public:
   MyChatServer(EventLoop*Loop,const InetAddress&IA,const std::string&NameArg);
   void Start();
};
#endif