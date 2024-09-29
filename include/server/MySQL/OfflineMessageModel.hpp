#ifndef OFFLINEMESSAGEMODEL_H
#define OFFLINEMESSAGEMODEL_H
#include<string>
class OfflineMessageModel{
public:
   bool insert(int userid,int fromid,std::string Message);
};
#endif