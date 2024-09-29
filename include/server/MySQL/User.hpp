#ifndef USER_H
#define USER_H
#include<string>
using namespace std;
class User{
private:
   int userid;
   string username,password,state;
public:
   User(int id=-1,string name="",string passwordt="",string status="offline");
   void setuserid(int id);
   void setusername(string name);
   void setrpassword(string password);
   void setstate(string status);
   int getuserid();
   string getusername();
   string getpassword();
   string getstate();
};
#endif