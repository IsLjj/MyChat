#ifndef MYCHATSERVICE_H
#define MYCHATSERVICE_H
#include<unordered_map>
#include<functional>
#include<muduo/net/TcpServer.h>
#include<json.hpp>
#include<mutex>
#include "redis.hpp"
using json=nlohmann::json;
using namespace muduo::net;
const int ErrorMsgid=666;
enum class MessageType{
    reg=1,
    reg_ack,
    login,
    log_ack,
    loginOff,
    loginOff_ack,
    addfrined,
    handleaddfriend,
    seerequst,
    SeeAddFriendResultE,
    ToAFriendChat,
    seeofflinemessage,
    creategroup,
    seemygroup,
    joingroupask,
    seejoingroupid,
    handlejoingroupask,
    seejoinresult,
    seefriends,
    seefriendsack,
    seeijoingroup,
    seeijoingroupack,
    OfflineMessagefack,
    sendgroupmessage,
    recvfgroup,
    sendgroupmessageack,
    OfflineMessagegack,
    seeaddmeack,
    seejoingroupack,
    seeresult,
    seeresultfack,
    seeresultgack,
    addfriendack,
    joingroupack,
    recvffriend,
    sendmessagetofriendack,
    handlefriendack,
    handlejoingroupack,
    creategroupack
};
class MyChatService{
private:
   MyChatService();
   using handfunc=std::function<void(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time)>;
   std::unordered_map<int,handfunc> MesHdl;
   std::unordered_map<int,const TcpConnectionPtr&> LoginMap;
   std::mutex FuncHandleMutex;
public:
   static MyChatService*GetMyChatService();
   std::unordered_map<int,const TcpConnectionPtr&>& GetLoginMap();
   handfunc GetMessageHandler(int msgid);
   Redis myredis;
   void login(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void reg(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void loginOff(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void AddFriendAsk(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void HandleAddFriend(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeRequst(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeAddFriendResult(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void ToAFriendMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeOfflieMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void CreateGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeMyGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void JoinGroupAsk(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeJoinGroupid(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void HandleJoinGroupAsk(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeJoinResult(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeFriends(const TcpConnectionPtr&TCP,json &js,muduo::Timestamp time);
   void SeeIJoinGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SendGroupMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time);
   void SeeResult(const TcpConnectionPtr&TCP,json &js,muduo::Timestamp time);
   void ReceMessFRedis(int channel,string message);
};
#endif