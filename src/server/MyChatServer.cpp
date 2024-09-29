#include "MyChatServer.hpp"
#include "MyChatService.hpp"
#include "UserModel.hpp"
#include "MySQL.hpp"
#include<iostream>
#include<json.hpp>
#include<map>
#include<muduo/base/Logging.h>
#include "mpublic.hpp"
using json=nlohmann::json;
MyChatServer::MyChatServer(EventLoop*Loop,const InetAddress&IA,const std::string&NameArg):
MyServer(Loop,IA,NameArg),MyEventLoopt(Loop){
    MyServer.setConnectionCallback(std::bind(&MyChatServer::MyConnectionCallBack,this,_1));
    MyServer.setMessageCallback(std::bind(&MyChatServer::MySetMessageCallBack,this,_1,_2,_3));
    MyServer.setThreadNum(4);
}
void MyChatServer::MyConnectionCallBack(const TcpConnectionPtr&Tcp){
    /*if(Tcp->connected()){
       Tcp->send("connect success\n");
    }*/
   //std::cout<<Tcp->peerAddress().toIpPort()<<"->"<<Tcp->localAddress().toIpPort()<<"offline"<<std::endl;
    if(!Tcp->connected()){
    int id;
    UserModel um;
    std::unordered_map<int,const TcpConnectionPtr&> &tmap=MyChatService::GetMyChatService()->GetLoginMap();
    for(auto t=tmap.begin();t!=tmap.end();t++){
        LOG_INFO<<"t1";
        if(t->second==Tcp){
           LOG_INFO<<"t2";
           id=t->first;
           um.SetField(id,"state","offline");
           tmap.erase(t);
           MyChatService::GetMyChatService()->myredis.UnSubMessage(id);
           break;   
        }
    }
    //LOG_INFO<<"t2";
   
    Tcp->shutdown();
    }
}
void MyChatServer::MySetMessageCallBack(const TcpConnectionPtr&Tcp,Buffer*bf,muduo::Timestamp time){
      //std::cout<<Tcp->connected();
      std::string bfs=bf->retrieveAllAsString();
      /*std::cout<<"receive data"<<bfs<<"time"<<time.toString()<<std::endl;
      Tcp->send(bfs);*/
      json jbf;
      try{ 
       jbf=json::parse(bfs);
       }
      catch(...){
        //LOG_INFO<<"t";
        jbf["msgid"]=ErrorMsgid;
        Tcp->send("input exception\n");};
      //json jbf=json::parse(bfs);
      auto handfunca=MyChatService::GetMyChatService()->GetMessageHandler(jbf["msgid"].get<int>());
      handfunca(Tcp,jbf,time);
}
void MyChatServer::Start(){
    {
        MySQL ms;
        char ResetState[1024];
        sprintf(ResetState,"update User set state='%s'","offline");
        if(ms.Connect()){
        ms.Update(ResetState);}
    }
    MyServer.start();
}