#include "GroupUserModel.hpp"
#include "MySQL.hpp"
bool GroupUserModel::insert(int groupid,int id,std::string role){
    MySQL ms;
    char updatesql[1024];
    sprintf(updatesql,"insert into GroupUser(groupid,userid,grouprole) values(%d,%d,'%s')",
    groupid,id,role.c_str());
    return ms.Connect()&&ms.Update(updatesql);
}