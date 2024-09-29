#ifndef USERMODEL_H
#define USERMODEL_H
#include "MySQL.hpp"
#include "User.hpp"
class UserModel{
private:
   //MySQL ms;
public:
   bool insert(User&usr);
   bool Verify(int id,string password);
   bool Verifyid(int id);
   bool SetField(int id,string field,string fieldvalue);
};

#endif