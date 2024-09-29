#ifndef ADDFRIENDASKMODEL_H
#define ADDFRIENDASKMODEL_H
#include<string>
class AddFriendAskModel{
public:
   bool insert(int id,int friendid,std::string desc);
   bool VerifyResult();
   bool updateField(int id,int friendid,std::string (&srh)[4]);
};

#endif