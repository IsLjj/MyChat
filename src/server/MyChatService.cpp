#include "MyChatService.hpp"
#include "mpublic.hpp"
using namespace std::placeholders;
#include<muduo/base/Logging.h>
#include<iostream>
#include "User.hpp"
#include "UserModel.hpp"
#include "AddFriendAskModel.hpp"
#include "FriendModel.hpp"
#include "OfflineMessageModel.hpp"
MyChatService::MyChatService(){
    MesHdl[(int)MessageType::reg]=std::bind(&MyChatService::reg,this,_1,_2,_3);
    MesHdl[(int)MessageType::login]=std::bind(&MyChatService::login,this,_1,_2,_3);
    MesHdl[(int)MessageType::loginOff]=std::bind(&MyChatService::loginOff,this,_1,_2,_3);
    MesHdl[(int)MessageType::addfrined]=std::bind(&MyChatService::AddFriendAsk,this,_1,_2,_3);
    MesHdl[(int)MessageType::handleaddfriend]=std::bind(&MyChatService::HandleAddFriend,this,_1,_2,_3);
    MesHdl[(int)MessageType::seerequst]=std::bind(&MyChatService::SeeRequst,this,_1,_2,_3);
    MesHdl[(int)MessageType::SeeAddFriendResultE]=std::bind(&MyChatService::SeeAddFriendResult,this,_1,_2,_3);
    MesHdl[(int)MessageType::ToAFriendChat]=std::bind(&MyChatService::ToAFriendMessage,this,_1,_2,_3);
    MesHdl[(int)MessageType::seeofflinemessage]=std::bind(&MyChatService::SeeOfflieMessage,this,_1,_2,_3);
    MesHdl[(int)MessageType::creategroup]=std::bind(&MyChatService::CreateGroup,this,_1,_2,_3);
    MesHdl[(int)MessageType::seemygroup]=std::bind(&MyChatService::SeeMyGroup,this,_1,_2,_3);
    MesHdl[(int)MessageType::joingroupask]=std::bind(&MyChatService::JoinGroupAsk,this,_1,_2,_3);
    MesHdl[(int)MessageType::seejoingroupid]=std::bind(&MyChatService::SeeJoinGroupid,this,_1,_2,_3);
    MesHdl[(int)MessageType::handlejoingroupask]=std::bind(&MyChatService::HandleJoinGroupAsk,this,_1,_2,_3);
    MesHdl[(int)MessageType::seejoinresult]=std::bind(&MyChatService::SeeJoinResult,this,_1,_2,_3);
    MesHdl[(int)MessageType::seefriends]=std::bind(&MyChatService::SeeFriends,this,_1,_2,_3);
    MesHdl[(int)MessageType::seeijoingroup]=std::bind(&MyChatService::SeeIJoinGroup,this,_1,_2,_3);
    MesHdl[(int)MessageType::sendgroupmessage]=std::bind(&MyChatService::SendGroupMessage,this,_1,_2,_3);
    MesHdl[(int)MessageType::seeresult]=std::bind(&MyChatService::SeeResult,this,_1,_2,_3);
    MesHdl[ErrorMsgid]=[](auto a,auto b,auto c){LOG_ERROR<<"msgid is not in handler map";};
    if(myredis.Connect()){
       myredis.InitFuncHanMess(std::bind(&MyChatService::ReceMessFRedis,this,_1,_2));
    }
}
MyChatService* MyChatService::GetMyChatService(){
   static MyChatService MCS;
   return &MCS;
}
void MyChatService::ReceMessFRedis(int channel,string message){
   unique_lock<mutex> thislock(FuncHandleMutex);
   auto it=LoginMap.find(channel);
   if(it!=LoginMap.end()){
      it->second->send(message);
   }
   else{
     MySQL ms;
     json responser=json::parse(message);
     char updatesql[1024],messagec[500];
     sprintf(messagec,string(responser["message"]).c_str());
     mreplaces(messagec,'\'',500);
     sprintf(updatesql,"insert into OfflineMessage(userid,message,fromid,groupid) values(%d,'%s',%d,%d)",
     channel,messagec,int(responser["fromid"]),int(responser["groupid"]));
     if(ms.Connect()){
       ms.Update(updatesql);
     }
   }
}
std::unordered_map<int,const TcpConnectionPtr&>& MyChatService::GetLoginMap(){
   return LoginMap;
}

MyChatService::handfunc MyChatService::GetMessageHandler(int msgid){
    auto t=MesHdl.find(msgid);
    if(t==MesHdl.end()){
        return MesHdl[ErrorMsgid];
    }
    return MesHdl[msgid];
}
//{"msgid":1,"name":,"password":}
void MyChatService::reg(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   json response;
   response["msgid"]=int(MessageType::reg_ack);
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select count(*) from User where name='%s'",string(js["name"]).c_str());
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      if((mrw=mysql_fetch_row(mrs))&&(atoi(mrw[0])==1)){
         response["message"]="name is repetition";
         TCP->send(response.dump());
         return;
      }
   }
   User usr;
   UserModel um;
   usr.setusername(js["name"]);
   usr.setrpassword(js["password"]);
   usr.setstate("offline");
   if(um.insert(usr)){
     response["errno"]=0;
     response["Message"]="register success";
     response["id"]=usr.getuserid();
   }
   else{
    response["errno"]=1;
    response["ErrorMessage"]="register failure";
   }
   TCP->send(response.dump());
}
//{"msgid":3,"id":,"password":}
void MyChatService::login(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   User usr;
   UserModel um;
   json response;
   response["msgid"]=MessageType::log_ack;
   if(um.Verify(js["id"],js["password"])){
     {
        std::unique_lock<mutex> loginMutex(FuncHandleMutex);
        LoginMap.emplace(js["id"],TCP);
        
     }
     um.SetField(js["id"],"state","online");
     myredis.SubMessage(int(js["id"]));
     response["errno"]=0;
     response["Message"]="login success";
   }
   else{
    response["errno"]=1;
    response["Message"]="login failure";
   }
   TCP->send(response.dump());
   //TCP->send("\n");
}
//{"msgid":5}
void MyChatService::loginOff(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
  UserModel um;
  int id;
  for(auto it=LoginMap.begin();it!=LoginMap.end();it++){
   if(it->second==TCP){
      id=it->first;
      myredis.UnSubMessage(id);
      um.SetField(id,"state","offline");
   }
   {
      unique_lock<mutex> loginoutMutex(FuncHandleMutex);
      LoginMap.erase(it);
   }
   break;
  }
  json response;
  response["msgid"]=MessageType::loginOff_ack;
  response["message"]="login off success";
  TCP->send(response.dump());
  TCP->shutdown();
}
//{"msgid":7,"id":,"friendid":,"desc":}
void MyChatService::AddFriendAsk(const TcpConnectionPtr&TCP,json &js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select count(*) from AddFriendAsk where id=%d and friendid=%d and result='-1'",
   int(js["id"]),int(js["friendid"]));
   int flag;
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      if(mrw=mysql_fetch_row(mrs)){
         flag=atoi(mrw[0]);
      }
   }
   if(flag){
      json response;
      response["msgid"]=int(MessageType::addfriendack);
      response["message"]="submit failure,you have submitted";
      TCP->send(response.dump());
      return;
   }
   FriendModel fm;
   UserModel um;
   bool btfriendid=fm.VerifyFriendid(js["id"],js["friendid"]);
   bool btfriendidu=um.Verifyid(js["friendid"]);
   //js["desc"]=" ";
   json response;
   response["msgid"]=int(MessageType::addfriendack);
   if(btfriendid==0&&btfriendidu==1){
      AddFriendAskModel am;
      auto t=js.find("desc");
      if(t==js.end()){
         js["desc"]=" ";
      }
      bool bt=am.insert(js["id"],js["friendid"],js["desc"]);
      if(bt){
         response["message"]="submit success";
         //TCP->send("submit success\n");
      }
      else{
         response["message"]="submit failure";
         //TCP->send("submit failure\n");
      }
   }
   else if (btfriendid==1&&btfriendidu==1){
       response["message"]="you have add this friend";
       //TCP->send("you have add this friend\n");
   }
   else if(btfriendid==0&&btfriendidu==0){
       response["message"]="friendid is inexistence";
       //TCP->send("friendid is inexistence\n");
   }
   TCP->send(response.dump());
}
//{"msgid":8,"id":,"friendid":,"handleflag":,"handledesc":}
void MyChatService::HandleAddFriend(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   int HandleFlag=js["handleflag"];
   AddFriendAskModel afam;
   json response;
   if(!afam.VerifyResult()){
      response["msgid"]=int(MessageType::handlefriendack);
      response["message"]="No request";
      TCP->send(response.dump());
      return;
   }
   response["msgid"]=int(MessageType::handlefriendack);
   try{
     js["handledesc"];
   }
   catch(...){
      js["handledesc"]=" ";
   }
   if(HandleFlag){
      FriendModel fm;
      string srh[4]={"result","1","handledesc",js["handledesc"]};
      afam.updateField(js["id"],js["friendid"],srh);
      fm.insert(js["id"],js["friendid"]);
   }
   else{
      string srh[4]={"result","0","handledesc",js["handledesc"]};
      afam.updateField(js["id"],js["friendid"],srh);
   }
   MySQL ms;
   char updatesql[1024];
   sprintf(updatesql,"delete from AddFriendAsk where id=%d and frinedid=%d",
   int(js["id"]),int(js["friendid"]));
   if(ms.Connect()){
      ms.Update(updatesql);
   }
   response["message"]="handle success";
   TCP->send(response.dump());
}
//{"msgid":9,"id":}
void MyChatService::SeeRequst(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select id,Adddscr,result from AddFriendAsk where friendid=%d order by result",
   int(js["id"]));
   if(ms.Connect()){
      MYSQL_RES*mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::seeaddmeack);
      char s='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
        response["id"]=atoi(mrw[0]);
        response["desc"]=mrw[1];
        response["result"]=mrw[2];
        num=s;
        responseA[num]=response;
        s++;
      }
      TCP->send(responseA.dump());
   }
   MySQL ms2;
   char querysql2[1024];
   sprintf(querysql2,"select * from JoinGroup where groupid in(select groupid from GroupUser where userid=%d and grouprole='creator') order by groupid",
   int(js["id"]));
   if(ms2.Connect()){
      MYSQL_RES*mrs=ms2.Query(querysql2);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::seejoingroupack);
      char s='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
        response["id"]=mrw[0];
        response["groupid"]=mrw[1];
        response["JoinDesc"]=mrw[2];
        response["result"]=mrw[3];
        num=s;
        responseA[num]=response;
        s++;
      }
      TCP->send(responseA.dump());
   }
}
//{"msgid":10,"id":}
void MyChatService::SeeAddFriendResult(const TcpConnectionPtr&TCP,json &js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select friendid,result from AddFriendAsk where id=%d order by result",int(js["id"]));
   //sprintf(querysql,"select * from AddFriendAsk");
   json response;
   json responseA;
   responseA["msgid"]=int(MessageType::seeresultfack);
   if(ms.Connect()){
      MYSQL_RES*mrp=ms.Query(querysql);
      MYSQL_ROW r;
      char s='1';
      string num;
      while(r=mysql_fetch_row(mrp)){
         response["friendid"]=r[0];
         response["result"]=r[1];
         num=s;
         responseA[num]=response;
         response.clear();
      }
      TCP->send(responseA.dump());
   }
}
//{"msgid":11,"id":,"friendid":,"Message":}
void MyChatService::ToAFriendMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
    FriendModel fm;
    bool tfm=fm.VerifyFriendid(js["id"],js["friendid"]);
    auto t=LoginMap.find(js["friendid"]);
    json response;
    response["msgid"]=int(MessageType::sendmessagetofriendack);
    string message=js["Message"];
    char messagec[500];
    sprintf(messagec, "%s %s", message.c_str(), string(js["time"]).c_str());
    if (!mreplaces(messagec, '\'', 500))
    {
        response["message"]="text content more than 500 characters";
        TCP->send(response.dump());
        return;
    }
    if(tfm){
      if(t!=LoginMap.end()){
         json jst;
         jst["msgid"]=int(MessageType::recvffriend);
         jst["id"]=js["id"];
         jst["message"]=js["Message"];
         jst["time"]=js["time"];
         t->second->send(jst.dump());
         response["message"]="send success";
         TCP->send(response.dump());
         return;
      }
      MySQL ms;
      char querysql[1024];
      sprintf(querysql,"select count(*) from User where id=%d and state='online'",int(js["friendid"]));
      if(ms.Connect()){
         MYSQL_RES*mrs=ms.Query(querysql);
         MYSQL_ROW mrw;
         if(mrw=mysql_fetch_row(mrs)){
            if(atoi(mrw[0])>0){
               json jst;
               jst["msgid"]=int(MessageType::recvffriend);
               jst["fromid"]=js["id"];
               jst["message"]=js["Message"];
               jst["groupid"]=0;
               jst["time"]=js["time"];
               myredis.PubMessage(int(js["friendid"]),jst.dump());
               response["message"]="send success";
               TCP->send(response.dump());
               return;
            }
         }
      }
      OfflineMessageModel om;
      message=messagec;
      om.insert(js["friendid"],js["id"],message);
      response["message"]="send success";
   }
   else{
      response["message"]="send error,examining friendid";
   }
   TCP->send(response.dump());
}
//{"msgid":12,"id":}
void MyChatService::SeeOfflieMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select fromid,message from OfflineMessage where userid=%d and groupid=%d order by fromid",
   int(js["id"]),0);
   if(ms.Connect()){
      MYSQL_RES*mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::OfflineMessagefack);
      string num;
      char s='1';
      while(mrw=mysql_fetch_row(mrs)){
         response["fromid"]=atoi(mrw[0]);
         response["message"]=mrw[1];
         num=s;
         responseA[num]=response;
         s++;
      }
      TCP->send(responseA.dump());
   }
   MySQL ms2;
   char querysql2[1024];
   sprintf(querysql2,"select fromid,groupid,message from OfflineMessage where userid=%d and groupid>%d order by groupid",
   int(js["id"]),0);
   if(ms2.Connect()){
      MYSQL_RES*mrs=ms2.Query(querysql2);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::OfflineMessagegack);
      string num;
      char s='1';
      while(mrw=mysql_fetch_row(mrs)){
         response["fromid"]=atoi(mrw[0]);
         response["groupid"]=atoi(mrw[1]);
         response["message"]=mrw[2];
         num=s;
         responseA[num]=response;
         s++;
      }
      TCP->send(responseA.dump());
   }
}
//{"msgid":13,"id":,"groupname":,"groupdesc":}
void MyChatService::CreateGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
    MySQL ms;
    char updatesql[1024];
    auto t=js.find("groupdesc");
    if(t==js.end()){
      js["groupdesc"]=" ";
    }
    sprintf(updatesql,"insert into AllGroup(groupname,groupdesc) values('%s','%s')",
    std::string(js["groupname"]).c_str(),std::string(js["groupdesc"]).c_str());
    json response;
    if(ms.Connect()){
      ms.Update(updatesql);
      response["groupid"]=mysql_insert_id(ms.GetConnection());
      response["msgid"]=int(MessageType::creategroupack);
      char updatesql2[1024];
      sprintf(updatesql2,"insert into GroupUser(groupid,userid,grouprole) values('%d','%d','creator')",
      int(response["groupid"]),int(js["id"]));
      ms.Update(updatesql2);
      TCP->send(response.dump());
    }
}
//{"msgid":14,"id":}
void MyChatService::SeeMyGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char QuerySql[1024];
   sprintf(QuerySql,"select a.groupid,a.groupname,a.groupdesc from AllGroup a inner join GroupUser b on a.groupid=b.groupid where b.userid=%d",
   int(js["id"]));
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(QuerySql);
      MYSQL_ROW mrw;
      json response;
      while(mrw=mysql_fetch_row(mrs)){
         response["groupid"]=mrw[0];
         response["groupname"]=mrw[1];
         response["groupdesc"]=mrw[2];
         TCP->send(response.dump());
         TCP->send("\n");
      }
   }
}
//{"msgid":15,"id":,"groupid":,JoinDesc":}
void MyChatService::JoinGroupAsk(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms3;
   char querysql3[1024];
   sprintf(querysql3,"select count(*) from JoinGroup where id=%d and groupid=%d and result='-1'",
   int(js["id"]),int(js["groupid"]));
   if(ms3.Connect()){
      MYSQL_RES* mrs=ms3.Query(querysql3);
      MYSQL_ROW mrw;
      mrw=mysql_fetch_row(mrs);
      if(atoi(mrw[0])>0){
         json response;
         response["msgid"]=int(MessageType::joingroupack);
         response["message"]="there have your ask";      
         TCP->send(response.dump());
         return;
      }
   }
   json response;
   response["msgid"]=int(MessageType::joingroupack);
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select count(*) from GroupUser where groupid=%d and userid=%d",
   int(js["groupid"]),int(js["id"]));
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      if(mrw=mysql_fetch_row(mrs)){
         if(atoi(mrw[0])>0){
            response["message"]="you have in the group";      
            TCP->send(response.dump());
            return;
         }
      }
   }
   MySQL ms1;
   char insertquery[1024];
   auto t=js.find("JoinDesc");
   if(t==js.end()){
      js["JoinDesc"]=" ";
   }
   string joindesc=string(js["JoinDesc"]);
   //replaceAll(string(js["JoinDesc"]),"'","''");
   //mreplaces(joindesc,string("'"),string("''"));
   //mreplaces(joindesc,string("\""),string("\\\""));
   sprintf(insertquery,"insert into JoinGroup(groupid,id,JoinDesc,result) values(%d,%d,'%s','-1')",
   int(js["groupid"]),int(js["id"]),joindesc.c_str());
   if(ms1.Connect()){
      if(ms1.Update(insertquery)){
      response["message"]="submit success";
      TCP->send(response.dump());}
   }
}
//{"msgid":16,"id":}
void MyChatService::SeeJoinGroupid(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select a.id,a.groupid,a.JoinDesc from JoinGroup a inner join GroupUser b on a.groupid=b.groupid where b.userid=%d and b.grouprole='creator'",
   int(js["id"]));
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      json response;
      if(mrw=mysql_fetch_row(mrs)){
         response["Applicantid"]=mrw[0];
         response["groupid"]=mrw[1];
         response["JoinDesc"]=mrw[2];
         TCP->send(response.dump());
         TCP->send("\n");
         response.clear();
      }
   }
}
//{"msgid":17,"id":,"groupid":,"Applicantid":,"result":,"handledesc"}
void MyChatService::HandleJoinGroupAsk(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms1;
   char querysql[1024];
   sprintf(querysql,"select userid from GroupUser where groupid=%d",int(js["groupid"]));
   if(ms1.Connect()){
      MYSQL_RES*mrs=ms1.Query(querysql);
      MYSQL_ROW mrw;
      mrw=mysql_fetch_row(mrs);
      if(mrw==nullptr||atoi(mrw[0])!=int(js["id"])){
         json response;
         response["msgid"]=int(MessageType::handlejoingroupack);
         response["message"]="groupid is wrong";
         TCP->send(response.dump());
         return;
      }
   }
   MySQL ms;
   int tresult=js["result"];
   char UpdateSql[1024];
   sprintf(UpdateSql,"update JoinGroup set result='%d',handledesc='%s' where id=%d and groupid=%d and result='-1'",
   tresult,string(js["handledesc"]).c_str(),int(js["Applicantid"]),int(js["groupid"]));
   if(ms.Connect()){
      ms.Update(UpdateSql);
   }
   if(tresult>0){
      char UpdateSql2[1024];
      sprintf(UpdateSql2,"insert into GroupUser(groupid,userid,grouprole) values(%d,%d,'normal')",
      int(js["groupid"]),int(js["Applicantid"]));
      ms.Update(UpdateSql2);
   }
   json response;
   response["msgid"]=int(MessageType::handlejoingroupack);
   response["message"]="handle success";
   TCP->send(response.dump());
}
//{"msgid":18,"id":}
void MyChatService::SeeJoinResult(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms2;
   char Querysql2[1024];
   sprintf(Querysql2,"select groupid,result from JoinGroup where id=%d order by result",int(js["id"]));
   if(ms2.Connect()){
      MYSQL_RES*mrs=ms2.Query(Querysql2);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::seeresultgack);
      char s='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
         response["groupid"]=mrw[0];
         response["result"]=mrw[1];
         num=s;
         responseA[num]=response;
         s++;
         response.clear();
      }
      TCP->send(response.dump());
   }
}
//{"msgid":19,"id":}
void MyChatService::SeeFriends(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char QuerySql[1024];
   sprintf(QuerySql,"select b.id,b.name,b.state from Friend a inner join User b on a.friendid=b.id where a.userid=%d order by b.state",
   int(js["id"]));
   if(ms.Connect()){
      MYSQL_RES* mrs=ms.Query(QuerySql);
      MYSQL_ROW mrw;
      json responsem;
      json responseA;
      responseA["msgid"]=int(MessageType::seefriendsack);
      char s='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
         responsem["id"]=atoi(mrw[0]);
         responsem["name"]=mrw[1];
         responsem["state"]=mrw[2];
         num=s;
         responseA[num]=responsem;
         s++;
         responsem.clear();
      }
      TCP->send(responseA.dump());
   }
}
//{"msgid":21,"id":}
void MyChatService::SeeIJoinGroup(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select b.groupid,b.groupname,b.groupdesc,a.grouprole from GroupUser a inner join AllGroup b on a.groupid=b.groupid where a.userid=%d"
   ,int(js["id"]));
   json responseA;
   responseA["msgid"]=int(MessageType::seeijoingroupack);
   json response;
   if(ms.Connect()){
      MYSQL_RES*mrs=ms.Query(querysql);
      MYSQL_ROW mrw;
      char i='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
         response["groupid"]=atoi(mrw[0]);
         response["groupname"]=mrw[1];
         response["groupdesc"]=mrw[2];
         response["grouprole"]=mrw[3];
         num=i;
         responseA[num]=response;
         i++;
         response.clear();
      }
      TCP->send(responseA.dump());
   }
}
//{"msgid":24,"id":,"groupid":,"message":}
void MyChatService::SendGroupMessage(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   //LoginMap
   MySQL ms;
   char querygroupmi[1024];
   sprintf(querygroupmi,"select count(*) from GroupUser where groupid=%d and userid=%d",
   int(js["groupid"]),int(js["id"]));
   json response;
   response["msgid"]=int(MessageType::sendgroupmessageack);
   if(ms.Connect()){
      MYSQL_RES*mrs=ms.Query(querygroupmi);
      MYSQL_ROW mrw;
      while(mrw=mysql_fetch_row(mrs)){
         if(atoi(mrw[0])<1){
           response["message"]="Error groupid or userid not in group";
           TCP->send(response.dump());
           return;
         }
      }
   }
   char querymember[1024];
   sprintf(querymember,"select a.userid,b.state from GroupUser a inner join User b on a.userid=b.id where groupid=%d",
   int(js["groupid"]));
   MySQL ms2;
   if(ms2.Connect()){
      MYSQL_RES*mrs=ms2.Query(querymember);
      MYSQL_ROW mrw;
      json responser;
      responser["groupid"]=js["groupid"];
      responser["fromid"]=js["id"];
      responser["time"]=js["time"];
      string meg=js["message"];
      char megc[500];
      sprintf(megc,meg.c_str());
      if(!mreplaces(megc,'\'',500)){
         response["MessageError"]="text is longer than 500 characters";
         TCP->send(response.dump());
         return;
      }
      while(mrw=mysql_fetch_row(mrs)){
         if(atoi(mrw[0])!=int(js["id"])){
            auto it=LoginMap.find(atoi(mrw[0]));
            responser["msgid"]=int(MessageType::recvfgroup);
            if(string(mrw[1])=="online"&&it!=LoginMap.end()){
               responser["message"]=js["message"];
               it->second->send(responser.dump());
               response["message"]="send success";
            }
            else if(string(mrw[1])=="online"&&it==LoginMap.end()){
               responser["message"]=js["message"];
               myredis.PubMessage(atoi(mrw[0]),responser.dump());
               response["message"]="send success";
            }
            else if(string(mrw[1])!="online"){
               char insertsql[1024];
               string message=megc;
               message.append(" ");
               message.append(string(js["time"]));
               if(message.size()>500){
                  response["MessageError"]="text is longer than 500 characters";
                  TCP->send(response.dump());
                  return;
               }
               sprintf(insertsql,"insert into OfflineMessage(userid,message,fromid,groupid) values(%d,'%s',%d,%d)",
               atoi(mrw[0]),message.c_str(),int(js["id"]),int(js["groupid"]));
               MySQL ms3;
               if(ms3.Connect()){
                  ms3.Update(insertsql);
                  response["message"]="send success";
               }
            }
         }
      }
   }
   TCP->send(response.dump());
}
//{"msgid":seeresult,"id":}
void MyChatService::SeeResult(const TcpConnectionPtr&TCP,json&js,muduo::Timestamp time){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select friendid,result from AddFriendAsk where id=%d order by result",int(js["id"]));
   //sprintf(querysql,"select * from AddFriendAsk");
   json response;
   json responseA;
   responseA["msgid"]=int(MessageType::seeresultfack);
   if(ms.Connect()){
      MYSQL_RES*mrp=ms.Query(querysql);
      MYSQL_ROW r;
      char s='1';
      string num;
      while(r=mysql_fetch_row(mrp)){
         response["friendid"]=r[0];
         response["result"]=r[1];
         num=s;
         responseA[num]=response;
         s++;
         response.clear();
      }
      TCP->send(responseA.dump());
   }
   MySQL ms2;
   char Querysql2[1024];
   sprintf(Querysql2,"select groupid,result from JoinGroup where id=%d order by result",int(js["id"]));
   if(ms2.Connect()){
      MYSQL_RES*mrs=ms2.Query(Querysql2);
      MYSQL_ROW mrw;
      json response;
      json responseA;
      responseA["msgid"]=int(MessageType::seeresultgack);
      char s='1';
      string num;
      while(mrw=mysql_fetch_row(mrs)){
         response["groupid"]=mrw[0];
         response["result"]=mrw[1];
         num=s;
         responseA[num]=response;
         s++;
         response.clear();
      }
      TCP->send(responseA.dump());
   }
}