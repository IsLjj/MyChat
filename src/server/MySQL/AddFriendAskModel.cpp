#include "AddFriendAskModel.hpp"
#include "UserModel.hpp"
bool AddFriendAskModel::insert(int id,int friendid,std::string desc){
    UserModel um;
    bool bt=um.Verifyid(friendid);
    if(bt){
        MySQL ms;
        char updatesql[1024];
        sprintf(updatesql,"insert into AddFriendAsk(id,friendid,Adddscr) values(%d,%d,'%s')",
        id,friendid,desc.c_str());
        if(ms.Connect()&&ms.Update(updatesql)){
            return true;
        }
    }
    return false;
}
bool AddFriendAskModel::VerifyResult(){
   MySQL ms;
   char querysql[1024];
   sprintf(querysql,"select count(*) from AddFriendAsk where result='-1'");
   if(ms.Connect()){
    MYSQL_RES*mrs=ms.Query(querysql);
    MYSQL_ROW mrw;
    if(mrw=mysql_fetch_row(mrs)){
       return atoi(mrw[0]);
    }
   }
   return 0;
}
bool AddFriendAskModel::updateField(int id,int friendid,std::string (&srh)[4]){
  char UpdateFieldSql[1024];
  //sprintf(UpdateFieldSql,"update User set %s='%s' where id=%d",field.c_str(),fieldvalue.c_str(),id);
  sprintf(UpdateFieldSql,"update AddFriendAsk set %s='%s',%s='%s' where friendid=%d and id=%d and result='-1'",srh[0].c_str(),
  srh[1].c_str(),srh[2].c_str(),srh[3].c_str(),id,friendid);
  MySQL ms;
  if(ms.Connect()&&ms.Update(UpdateFieldSql)){
    //LOG_INFO<<"t";
    return true;
  }
  return false;
}
