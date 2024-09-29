#include "UserModel.hpp"
#include <muduo/base/Logging.h>
bool UserModel::insert(User&usr){
    char insertsql[1024];//queryid[1024];
    sprintf(insertsql,"insert into User(name,password,state) values('%s','%s','%s')",
    usr.getusername().c_str(),usr.getpassword().c_str(),usr.getstate().c_str());
    //sprintf(queryid,"select id from User where name='%s'",usr.getusername());
    MySQL ms;
    if(ms.Connect()&&ms.Update(insertsql)){
        usr.setuserid(mysql_insert_id(ms.GetConnection()));
        return true;
    }
    return false;
}
bool UserModel::Verify(int id,string password){
   char Querysql[1024];
   sprintf(Querysql,"select id from User where id=%d and password='%s' and state='offline'",id,password.c_str());
   MySQL ms;
   if(ms.Connect()){
      MYSQL_RES*mres=ms.Query(Querysql);
      MYSQL_ROW row=mysql_fetch_row(mres);
      if(mres->row_count>0){
          return true;
      }
   }
   return false;
   /*sprintf(Querysql,"update User set state='online' where id=%d and password='%s'",
   id,password.c_str());
   MySQL ms;
   if(ms.Connect()){
    if(ms.Update(Querysql)){
      return true;
    }
   }*/ 
}

bool UserModel::Verifyid(int id){
   char VerifyidSql[1024];
   sprintf(VerifyidSql,"select count(*) from User where id=%d",id);
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

bool UserModel::SetField(int id,string field,string fieldvalue){
  char UpdateFieldSql[1024];
  //sprintf(UpdateFieldSql,"update User set %s='%s' where id=%d",field.c_str(),fieldvalue.c_str(),id);
  sprintf(UpdateFieldSql,"update User set %s='%s' where id=%d",field.c_str(),fieldvalue.c_str(),id);
  MySQL ms;
  if(ms.Connect()&&ms.Update(UpdateFieldSql)){
    //LOG_INFO<<"t";
    return true;
  }
  return false;
}
