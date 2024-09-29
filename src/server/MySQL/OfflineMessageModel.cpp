#include"OfflineMessageModel.hpp"
#include "MySQL.hpp"
bool OfflineMessageModel::insert(int id,int fromid,string Message){
   MySQL ms;
   char insertsql[1024];
   sprintf(insertsql,"insert into OfflineMessage(userid,message,fromid) values(%d,'%s',%d)",id,Message.c_str(),fromid);
   if(ms.Connect()&&ms.Update(insertsql)){
     return true;
   }
   return false;
}