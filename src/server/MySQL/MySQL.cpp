#include "MySQL.hpp"
#include<muduo/base/Logging.h>
MySQL::MySQL(){
    Connection=mysql_init(nullptr);
}
MySQL::~MySQL(){
    if(Connection!=nullptr){
        //onConnection=false;
        mysql_close(Connection);
    }
}
bool MySQL::Connect(){
    MYSQL*p=mysql_real_connect(Connection,hostip.c_str(),user.c_str(),password.c_str(),
    DBName.c_str(),port,nullptr,0);
    if(p!=nullptr){
        //onConnection=true;
        mysql_query(Connection,"set names gbk");
    }
    //LOG_INFO<<p;
    return p!=nullptr;
}
bool MySQL::Update(string UpdateSql){
    if(mysql_query(Connection,UpdateSql.c_str())){
        //LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<UpdateSql<<" Update failure";
        LOG_INFO<<UpdateSql<<" Update failure";
        return false;
    }
    //LOG_INFO<<"t2";
    return true;
}
MYSQL_RES* MySQL::Query(string Querys){
    if(mysql_query(Connection,Querys.c_str())){
       //LOG_INFO<<__FILE__<<":"<<__LINE__<<":"<<Querys<<"Query failure";
       LOG_INFO<<Querys<<" Query failure";
       return mysql_use_result(nullptr);
    }
    return mysql_use_result(Connection);
}
MYSQL*MySQL::GetConnection(){
    return Connection;
}