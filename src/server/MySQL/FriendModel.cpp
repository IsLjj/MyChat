#include "FriendModel.hpp"
#include "MySQL.hpp"
bool FriendModel::insert(int id,int friendid){
    char insertsql[1024];
    sprintf(insertsql,"insert into Friend(userid,friendid) values(%d,%d),(%d,%d)",id,friendid,friendid,id);
    MySQL ms;
    if(ms.Connect()&&ms.Update(insertsql)){
        return true;
    }
    return false;
}

bool FriendModel::VerifyFriendid(int id,int friendid){
   char VerifyidSql[1024];
   sprintf(VerifyidSql,"select count(*) from Friend where userid=%d and friendid=%d",id,friendid);
   MySQL ms;
   if(ms.Connect()){
     MYSQL_RES* mres=ms.Query(VerifyidSql);
     if(mres!=nullptr){
       MYSQL_ROW mr=mysql_fetch_row(mres);
       if(atoi(mr[0])>0){
         return true;
       }
     }  
   }
   return false; 
}