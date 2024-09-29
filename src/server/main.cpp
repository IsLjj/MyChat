#include<iostream>
#include "MyChatServer.hpp"
using namespace std;
int main(int argc, char **arg)
{
    if (argc < 3)
    {
        cerr << "input error,example ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }
    char *ip = arg[1];
    int port = atoi(arg[2]);
    EventLoop Loop;
    InetAddress addr(ip,port);
    MyChatServer myChatServer(&Loop,addr,"ChatServer");
    myChatServer.Start();
    Loop.loop();
    return 0;
}