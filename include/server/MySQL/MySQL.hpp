#ifndef MYSQL_H
#define MYSQL_H
#include<mysql/mysql.h>
#include<string>
using namespace std;
static string hostip="127.0.0.1";
static string DBName="chat";
static string user="root";
static string password="201311";
static int port=3306;
class MySQL{
private:
    MYSQL* Connection;
    //bool onConnection=false;
public:
    MySQL();
    ~MySQL();
    bool Connect();
    bool Update(string UpdateSql);
    MYSQL_RES* Query(string sql);
    MYSQL*GetConnection();
};
#endif