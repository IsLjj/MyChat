#include <iostream>
#include "json.hpp"
#include "mpublic.hpp"
#include <thread>
#include <semaphore.h>
#include <atomic>
#include <vector>
#include <map>
#include <algorithm>
#include <ctime>
using json = nlohmann::json;
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
using namespace std;
sem_t semp;
atomic_bool IsLogin = false;
int Login_id, allrecv = 0;
json SeeFriendList, SeeGroupList, OfflineMessageF, OfflineMessageG, SeeAddme, SeeJoinGroup, SeeResultF,
SeeResultG, addfriendjs, joingroupjs, sendmessagefjs, sendmessagegjs, handlefriendjs, handlejoingroupjs,
creategroupjs, registerjsack;
multimap<int, json> friendmessage, groupmessage;
enum class MessageType
{
   reg = 1,
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
string& getCurrentTime()
{
   time_t currentTime = std::time(0);
   tm *localTime = localtime(&currentTime);
   char NowLocalTime[100];
   sprintf(NowLocalTime, "%d-%d-%d %d:%d:%d", localTime->tm_year + 1900, localTime->tm_mon + 1,
           localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
   static string NowLocalTimes=NowLocalTime;
   return NowLocalTimes;
}
void recvdataf(int cltfd)
{
   for (;;)
   {
      char buf[1024];
      int len = recv(cltfd, buf, 1024, 0);
      if (len == -1 || len == 0)
      {
         cerr << "\nreceive data error,try to restart";
         // exit(0);
         break;
      }
      else
      {
         json js = json::parse(buf);
         if (int(js["msgid"]) == int(MessageType::recvffriend))
         {
            // string times=getCurrentTime();
            // cout<<times;
            friendmessage.emplace(int(js["fromid"]), js);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::recvfgroup))
         {
            groupmessage.emplace(int(js["groupid"]), js);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::log_ack))
         {
            if (int(js["errno"]) == 0)
            {
               IsLogin = true;
            }
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::seefriendsack))
         {
            SeeFriendList.clear();
            SeeFriendList = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::seeijoingroupack))
         {
            SeeGroupList.clear();
            SeeGroupList = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::OfflineMessagefack) || int(js["msgid"]) == int(MessageType::OfflineMessagegack))
         {
            if (int(js["msgid"]) == int(MessageType::OfflineMessagefack))
            {
               OfflineMessageF.clear();
               OfflineMessageF = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (int(js["msgid"]) == int(MessageType::OfflineMessagegack))
            {
               OfflineMessageG.clear();
               OfflineMessageG = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (allrecv == 2)
            {
               sem_post(&semp);
               allrecv = 0;
            }
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::seeaddmeack) || int(js["msgid"]) == int(MessageType::seejoingroupack))
         {
            if (int(js["msgid"]) == int(MessageType::seeaddmeack))
            {
               SeeAddme.clear();
               SeeAddme = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (int(js["msgid"]) == int(MessageType::seejoingroupack))
            {
               SeeJoinGroup.clear();
               SeeJoinGroup = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (allrecv == 2)
            {
               sem_post(&semp);
               allrecv = 0;
               continue;
            }
         }
         if (int(js["msgid"]) == int(MessageType::seeresultfack) || int(js["msgid"]) == int(MessageType::seeresultgack))
         {
            if (int(js["msgid"]) == int(MessageType::seeresultfack))
            {
               SeeResultF.clear();
               SeeResultF = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (int(js["msgid"]) == int(MessageType::seeresultgack))
            {
               SeeResultG.clear();
               SeeResultG = js;
               memset(buf, '\0', sizeof(buf));
               allrecv++;
            }
            if (allrecv == 2)
            {
               sem_post(&semp);
               allrecv = 0;
               continue;
            }
         }
         if (int(js["msgid"]) == int(MessageType::addfriendack))
         {
            addfriendjs.clear();
            addfriendjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::joingroupack))
         {
            joingroupjs.clear();
            joingroupjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::sendmessagetofriendack))
         {
            sendmessagefjs.clear();
            sendmessagefjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::sendgroupmessageack))
         {
            sendmessagegjs.clear();
            sendmessagegjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::handlefriendack))
         {
            handlefriendjs.clear();
            handlefriendjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::handlejoingroupack))
         {
            handlejoingroupjs.clear();
            handlejoingroupjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::creategroupack))
         {
            creategroupjs.clear();
            creategroupjs = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::reg_ack))
         {
            registerjsack.clear();
            registerjsack = js;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
         if (int(js["msgid"]) == int(MessageType::loginOff_ack))
         {
            IsLogin=false;
            sem_post(&semp);
            memset(buf, '\0', sizeof(buf));
            continue;
         }
      }
   }
}
void ChatFriend(int cltfd)
{
   cout << "=============ChatFriend===============" << endl;
   json FriendList;
   FriendList["msgid"] = int(MessageType::seefriends);
   FriendList["id"] = Login_id;
   string QFriendMessage = FriendList.dump();
   int len = send(cltfd, QFriendMessage.c_str(), strlen(QFriendMessage.c_str()) + 1, 0);
   if (len == -1)
   {
      cerr << "send query friend list error" << endl;
      return;
   }
   else
   {
      sem_wait(&semp);
      SeeFriendList.erase("msgid");
      int num = 1;
      cout << "=============Friend List==============" << endl;
      for (json &js : SeeFriendList)
      {
         cout << num << "." << js << endl;
         num++;
      }
      cout << "======================================" << endl;
      cout << "========Who send OfflineMessage=======" << endl;
      int numid = 1;
      map<int, int> countid;
      for (json &js : OfflineMessageF)
      {
         countid[int(js["fromid"])]++;
      }
      for (pair<const int, int> &acd : countid)
      {
         cout << numid << "." << "friend id:" << acd.first << "," << "message count:" << acd.second << endl;
         numid++;
      }
      cout << "======================================" << endl;
   }
   cout << "======================================" << endl;
   for (;;)
   {
      char friendidc[100];
      cout << "choose friend id,b is back:";
      cin.getline(friendidc, 100);
      int friendid;
      if (friendidc[0] == 'b')
      {
         return;
      }
      else
      {
         friendid = atoi(friendidc);
      }
      bool flag = true;
      string friendname;
      while (flag)
      {
         for (json &js : SeeFriendList)
         {
            if (friendid == int(js["id"]))
            {
               friendname = js["name"];
               flag = false;
               break;
            }
         }
         if (flag)
         {
            cout << "input right friendid:";
            char getfriendidc[100];
            cin.getline(getfriendidc,100);
            friendid=atoi(getfriendidc);
            if(getfriendidc[0]=='b'){
               return;
            }
         }
         
      }
      int align = 38;
      int eqsize = (align - friendname.size()) / 2;
      char eqline[50];
      for (int i = 0; i < eqsize; i++)
      {
         eqline[i] = '=';
      }
      if (((align - friendname.size()) & 0x1) == 1)
      {
         cout << eqline << friendname << eqline << "=" << endl;
      }
      else
      {
         cout << eqline << friendname << eqline << endl;
      }
      for (json &js : OfflineMessageF)
      {
         if (int(js["fromid"]) == friendid)
         {
            cout << string(js["message"]) << endl;
         }
      }
      cout << "================ChatMenu==============" << endl;
      cout << "1.send message" << endl
           << "2.receive message" << endl
           << "3.back" << endl;
      for (;;)
      {
         cout << "input your choice:";
         int chatch;
         cin >> chatch;
         cin.get();
         while (chatch <= 0 && chatch > 3)
         {
            cout << "invaild choice,write correct choice:";
            cin >> chatch;
            cin.get();
         }
         switch (chatch)
         {
         case 1:
         {
            json TalktoFriend;
            TalktoFriend["msgid"] = int(MessageType::ToAFriendChat);
            TalktoFriend["id"] = Login_id;
            char talkmessage[500];
            cout << "input your talk message:";
            cin.getline(talkmessage, 500);
            /*while(!mreplaces(talkmessage,'\'',500)){
               cout<<"message is too long,input shorter:";
               cin.getline(talkmessage,500);
            }*/
            // cout<<talkmessage;
            TalktoFriend["friendid"] = friendid;
            TalktoFriend["Message"] = talkmessage;
            string talktime=getCurrentTime();
            TalktoFriend["time"]=talktime;
            string TalktoFriends = TalktoFriend.dump();
            int len = send(cltfd, TalktoFriends.c_str(), strlen(TalktoFriends.c_str()) + 1, 0);
            if (len < 0)
            {
               cerr << "send message error" << endl;
            }
            else
            {
               sem_wait(&semp);
               sendmessagefjs.erase("msgid");
               cout << "=========SendMessageResponse=========" << endl;
               cout << sendmessagefjs << endl;
               cout << "=====================================" << endl;
            }
         }
         break;
         case 2:
         {
            for (auto is : friendmessage)
            {
               if (is.first == friendid)
               {
                  string message = is.second["message"];
                  string time=is.second["time"];
                  cout << time<<" "<<message<<endl;
               }
            }
         }
         break;
         case 3:
            break;
         }
         if (chatch == 3)
         {
            break;
         }
      }
      cout << "======================================" << endl;
   }
}
void ChatGroup(int cltfd)
{
   cout << "=============ChatGroup================" << endl;
   json GroupListAsk;
   GroupListAsk["msgid"] = int(MessageType::seeijoingroup);
   GroupListAsk["id"] = Login_id;
   string GroupListAsks = GroupListAsk.dump();
   int len = send(cltfd, GroupListAsks.c_str(), strlen(GroupListAsks.c_str()) + 1, 0);
   if (len < 0)
   {
      cerr << "send message error";
      return;
   }
   else
   {
      sem_wait(&semp);
      SeeGroupList.erase("msgid");
      int num = 1;
      cout << "=============Group List===============" << endl;
      for (json &v : SeeGroupList)
      {
         cout << num << "." << v << endl;
         num++;
      }
      cout << "======================================" << endl;
      cout << "========Who send OfflineMessage=======" << endl;
      int numid = 1;
      map<pair<int, int>, int> countgid;
      for (json &js : OfflineMessageG)
      {
         countgid[make_pair(int(js["groupid"]), int(js["fromid"]))]++;
      }
      for (auto acd : countgid)
      {
         cout << numid << "." << "group id:" << acd.first.first << "," << "user id:" << acd.first.second << "," << "message count:" << acd.second << endl;
         numid++;
      }
      cout << "======================================" << endl;
   }
   for (;;)
   {
      char groupidc[100];
      cout << "choose group id,b is back:";
      cin.getline(groupidc, 100);
      int groupid;
      if (groupidc[0] == 'b')
      {
         return;
      }
      else
      {
         groupid = atoi(groupidc);
      }
      bool flag = true;
      string groupname;
      while (flag)
      {
         for (json &js : SeeGroupList)
         {
            if (groupid == int(js["groupid"]))
            {
               groupname = js["groupname"];
               flag = false;
               break;
            }
         }
         if (flag)
         {
            cout << "input right groupid:";
            char groupidc[100];
            cin.getline(groupidc,100);
            groupid=atoi(groupidc);
            if(groupidc[0]=='b'){
               return;
            }
         }
      }
      int align = 38;
      int eqsize = (align - groupname.size()) / 2;
      char eqline[50];
      for (int i = 0; i < eqsize; i++)
      {
         eqline[i] = '=';
      }
      if (((align - groupname.size()) & 0x1) == 1)
      {
         cout << eqline << groupname << eqline << "=" << endl;
      }
      else
      {
         cout << eqline << groupname << eqline << endl;
      }
      for (json &js : OfflineMessageG)
      {
         if (int(js["groupid"]) == groupid)
         {
            cout << "id:" << js["fromid"] << ",message:" << string(js["message"]) << endl;
         }
      }
      cout << "======================================" << endl;
      cout << "================ChatMenu==============" << endl;
      cout << "1.send message" << endl
           << "2.receive message" << endl
           << "3.back" << endl;
      for (;;)
      {
         cout << "input your choice:";
         int chatch;
         cin >> chatch;
         cin.get();
         while (chatch <= 0 && chatch > 3)
         {
            cout << "invaild choice,write correct choice:";
            cin >> chatch;
            cin.get();
         }
         switch (chatch)
         {
         case 1:
         {
            json TalktoGroup;
            TalktoGroup["msgid"] = int(MessageType::sendgroupmessage);
            TalktoGroup["id"] = Login_id;
            char talkmessage[500];
            cout << "input your talk message:";
            cin.getline(talkmessage, 500);
            TalktoGroup["groupid"] = groupid;
            TalktoGroup["message"] = talkmessage;
            TalktoGroup["time"]=getCurrentTime();
            string TalktoGroups = TalktoGroup.dump();
            int len = send(cltfd, TalktoGroups.c_str(), strlen(TalktoGroups.c_str()) + 1, 0);
            if (len < 0)
            {
               cerr << "send message error" << endl;
            }
            else
            {
               sem_wait(&semp);
               sendmessagegjs.erase("msgid");
               cout << "=========SendMessageResponse==========" << endl;
               cout << sendmessagegjs << endl;
               cout << "======================================" << endl;
            }
         }
         break;
         case 2:
         {
            for (auto is : groupmessage)
            {
               if (is.first == groupid)
               {
                  string message = is.second["message"];
                  string time=is.second["time"];
                  cout << "userid:" << is.second["fromid"] << "," << "message:" << message<< 
                  ","<<time<< endl;
               }
            }
         }
         break;
         case 3:
            break;
         }
         if (chatch == 3)
         {
            break;
         }
      }
   }
   cout << "=====================================" << endl;
}
void FeatureMenu(int cltfd)
{
   cout << "==========welcome to MyChat===========" << endl;
   vector<string> Feature = {"ChatFriend", "ChatGroup", "See request", "See result", "Add friend",
                             "Join group", "Handle friend request", "Handle Join group request", "Create group", "Back"};
   // Feature.emplace_back(string("Friend List"));
   int i = 1;
   for (string &v : Feature)
   {
      cout << i << "." << v << endl;
      i++;
   }
   cout << "======================================" << endl;
   int ChooseFeature;
   for (;;)
   {
      cout << "your choice:";
      cin >> ChooseFeature;
      cin.get();
      while (ChooseFeature <= 0 && ChooseFeature > 12)
      {
         cout << "invaild choice,write correct choice:";
         cin >> ChooseFeature;
         cin.get();
      }
      switch (ChooseFeature)
      {
      case 1:
      {
         ChatFriend(cltfd);
      }
      break;
      case 2:
      {
         ChatGroup(cltfd);
      }
      break;
      case 3:
      {
         json SeeRequst;
         SeeRequst["msgid"] = int(MessageType::seerequst);
         SeeRequst["id"] = Login_id;
         string SeeRequsts = SeeRequst.dump();
         int len = send(cltfd, SeeRequsts.c_str(), strlen(SeeRequsts.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            SeeAddme.erase("msgid");
            int num = 1;
            cout << "===========FriendRequestList==========" << endl;
            for (json &v : SeeAddme)
            {
               // string a=v["desc"];
               // Erased(a,'\\');
               cout << num << "." << v << endl;
               num++;
            }
            cout << "======================================" << endl;
            SeeJoinGroup.erase("msgid");
            num = 1;
            cout << "=========JoinGroupRequestList=========" << endl;
            for (json &v : SeeJoinGroup)
            {
               cout << num << "." << v << endl;
               num++;
            }
            cout << "======================================" << endl;
         }
      }
      break;
      case 4:
      {
         json SeeRsult;
         SeeRsult["msgid"] = int(MessageType::seeresult);
         SeeRsult["id"] = Login_id;
         string SeeRsults = SeeRsult.dump();
         int len = send(cltfd, SeeRsults.c_str(), strlen(SeeRsults.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            SeeResultF.erase("msgid");
            int num = 1;
            cout << "============AddFriendList=============" << endl;
            for (json &v : SeeResultF)
            {
               cout << num << "." << v << endl;
               num++;
            }
            cout << "======================================" << endl;
            SeeResultG.erase("msgid");
            num = 1;
            cout << "============JoinGroupList=============" << endl;
            for (json &v : SeeResultG)
            {
               cout << num << "." << v << endl;
               num++;
            }
            cout << "======================================" << endl;
         }
      }
      break;
      case 5:
      {
         json AddFriend;
         AddFriend["msgid"] = int(MessageType::addfrined);
         AddFriend["id"] = Login_id;
         int friendid = Login_id;
         while (friendid == Login_id)
         {
            cout << "input friendid:";
            cin >> friendid;
            cin.get();
            if (friendid == Login_id)
            {
               cout << "don't add yourself" << endl;
            }
         }
         char desc[200];
         cout << "input your introduce:";
         cin.getline(desc, 200);
         AddFriend["friendid"] = friendid;
         mreplaces(desc, '\'', 200);
         AddFriend["desc"] = desc;
         string AddFriends = AddFriend.dump();
         int len = send(cltfd, AddFriends.c_str(), strlen(AddFriends.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            addfriendjs.erase("msgid");
            cout << "==========AddFriendResponse===========" << endl;
            cout << addfriendjs << endl;
            cout << "======================================" << endl;
         }
      }
      break;
      case 6:
      {
         json JoinGroupjs;
         JoinGroupjs["msgid"] = int(MessageType::joingroupask);
         JoinGroupjs["id"] = Login_id;
         int groupid;
         cout << "input groupid:";
         cin >> groupid;
         cin.get();
         char desc[100];
         cout << "input your introduce:";
         cin.getline(desc, 100);
         JoinGroupjs["groupid"] = groupid;
         JoinGroupjs["JoinDesc"] = desc;
         string JoinGroupjss = JoinGroupjs.dump();
         int len = send(cltfd, JoinGroupjss.c_str(), strlen(JoinGroupjss.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            joingroupjs.erase("msgid");
            cout << "==========JoinGroupResponse==========" << endl;
            cout << joingroupjs << endl;
            cout << "======================================" << endl;
         }
      }
      break;
      case 7:
      {
         json handlejs;
         handlejs["id"] = Login_id;
         handlejs["msgid"] = int(MessageType::handleaddfriend);
         int friendid;
         cout << "input friend id:";
         cin >> friendid;
         cin.get();
         handlejs["friendid"] = friendid;
         cout << "input handle result(0 is disagree,1 is agree):";
         int flag = 0;
         cin >> flag;
         cin.get();
         handlejs["handleflag"] = flag;
         char handledesc[500];
         cout << "input handle reason:";
         cin.getline(handledesc, 500);
         mreplaces(handledesc, '\'', 500);
         handlejs["handledesc"] = handledesc;
         string handlejss = handlejs.dump();
         int len = send(cltfd, handlejss.c_str(), strlen(handlejss.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            handlefriendjs.erase("msgid");
            cout << "=========HandleResponse==============" << endl;
            cout << handlefriendjs << endl;
            cout << "======================================" << endl;
         }
      }
      case 8:
      {
         json handlejs;
         handlejs["id"] = Login_id;
         handlejs["msgid"] = int(MessageType::handlejoingroupask);
         int groupid;
         cout << "input group id:";
         cin >> groupid;
         cin.get();
         handlejs["groupid"] = groupid;
         int applicantid;
         cout << "input applicant id:";
         cin >> applicantid;
         cin.get();
         handlejs["Applicantid"] = applicantid;
         cout << "input handle result(0 is disagree,1 is agree):";
         int flag = 0;
         cin >> flag;
         cin.get();
         handlejs["result"] = flag;
         char handledesc[500];
         cout << "input handle reason:";
         cin.getline(handledesc, 500);
         mreplaces(handledesc, '\'', 500);
         handlejs["handledesc"] = handledesc;
         string handlejss = handlejs.dump();
         int len = send(cltfd, handlejss.c_str(), strlen(handlejss.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            handlejoingroupjs.erase("msgid");
            cout << "=========HandleResponse===============" << endl;
            cout << handlejoingroupjs << endl;
            cout << "======================================" << endl;
         }
      }
      break;
      case 9:
      {
         json CreateGroupjs;
         CreateGroupjs["id"] = Login_id;
         char name[100];
         CreateGroupjs["msgid"] = int(MessageType::creategroup);
         cout << "input group name:";
         cin.getline(name, 100);
         mreplaces(name, '\'', 100);
         CreateGroupjs["groupname"] = name;
         char desc[500];
         cout << "input group descrition:";
         cin.getline(desc, 500);
         mreplaces(desc, '\'', 500);
         CreateGroupjs["groupdesc"] = desc;
         string CreateGroupjss = CreateGroupjs.dump();
         int len = send(cltfd, CreateGroupjss.c_str(), strlen(CreateGroupjss.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error";
         }
         else
         {
            sem_wait(&semp);
            creategroupjs.erase("msgid");
            cout << "=========CreateResponse===============" << endl;
            cout << creategroupjs << endl;
            cout << "======================================" << endl;
         }
      }
      break;
      case 10:
      {
         return;
      }
      }
   }
}
int main(int argc, char **arg)
{
   // std::cout<<argc<<std::endl;
   // std::cout<<arg[0];//./ChatServer
   if (argc < 3)
   {
      cerr << "input error,example ./ChatServer 127.0.0.1 6000" << endl;
      exit(-1);
   }
   char *ip = arg[1];
   int port = atoi(arg[2]);
   int clientfd = socket(AF_INET, SOCK_STREAM, 0);
   if (clientfd == -1)
   {
      cerr << "create socket failure" << endl;
      exit(-1);
   }
   sockaddr_in saiserver;
   memset(&saiserver, 0, sizeof(sockaddr_in));
   saiserver.sin_family = AF_INET;
   saiserver.sin_port = htons(port);
   saiserver.sin_addr.s_addr = inet_addr(ip);
   if (connect(clientfd, (sockaddr *)&saiserver, sizeof(sockaddr_in)) == -1)
   {
      cerr << "connect server failure";
      close(clientfd);
      exit(-1);
   }
   thread RecvThread(recvdataf, clientfd);
   RecvThread.detach();
   sem_init(&semp, 0, 0);
   for (;;)
   {
      cout << "======================================" << endl;
      cout << "1.login" << endl
           << "2.register" << endl
           << "3.exit" << endl;
      cout << "======================================" << endl;
      int handleNum;
      cout << "input your choice:";
      cin >> handleNum;
      cin.get();
      switch (handleNum)
      {
      case 1:
      {
         if (IsLogin)
         {
            break;
         }
         int id;
         char password[50];
         cout << "id:";
         cin >> id;
         Login_id = id;
         cin.get();
         // cout<<endl;
         cout << "password:";
         cin.getline(password, 50);
         json js;
         js["msgid"] = int(MessageType::login);
         js["id"] = id;
         js["password"] = password;
         string loginMessage = js.dump();
         int len = send(clientfd, loginMessage.c_str(), strlen(loginMessage.c_str()) + 1, 0);
         if (len == -1)
         {
            cerr << "send login message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            cout << "===========LoginResponse==============" << endl;
            if (IsLogin)
            {
               cout << "Login success" << endl;
            }
            else
            {
               cout << "login failure,input error message or have logged in" << endl;
               cout << "======================================" << endl;
               break;
            }
            cout << "======================================" << endl;
            json OfflineMessageAsk;
            OfflineMessageAsk["msgid"] = int(MessageType::seeofflinemessage);
            OfflineMessageAsk["id"] = Login_id;
            string OfflineMessageAsks = OfflineMessageAsk.dump();
            int len2 = send(clientfd, OfflineMessageAsks.c_str(), strlen(OfflineMessageAsks.c_str()) + 1, 0);
            if (len2 < 0)
            {
               cerr << "send message error";
            }
            else
            {
               sem_wait(&semp);
               OfflineMessageF.erase("msgid");
               OfflineMessageG.erase("msgid");
               FeatureMenu(clientfd);
            }
         }
         break;
      }
      case 2:
      {
         json registerjs;
         char name[50], password[50];
         cout << "input user name:";
         cin.getline(name, 50);
         cout << "input password:";
         cin.getline(password, 50);
         mreplaces(name, '\'', 50);
         mreplaces(password, '\'', 50);
         registerjs["msgid"] = int(MessageType::reg);
         registerjs["name"] = name;
         registerjs["password"] = password;
         string registerjss = registerjs.dump();
         int len = send(clientfd, registerjss.c_str(), strlen(registerjss.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else
         {
            sem_wait(&semp);
            registerjsack.erase("msgid");
            cout << "=========RegisterResponse=============" << endl;
            cout << registerjsack << endl;
            cout << "======================================" << endl;
         }
         break;
      }
      case 3:
      {
         json loginoff;
         loginoff["id"]=Login_id;
         loginoff["msgid"]=int(MessageType::loginOff);
         string loginoffs = loginoff.dump();
         int len = send(clientfd, loginoffs.c_str(), strlen(loginoffs.c_str()) + 1, 0);
         if (len < 0)
         {
            cerr << "send message error" << endl;
         }
         else{
            sem_wait(&semp);
            if(!IsLogin){
               cout<<"login off success"<<endl;
            }
         }
         sem_destroy(&semp);
         exit(0);
      }
      }
   }
}
