#include "User.hpp"
User::User(int id,string name,string passwordt,string status){
    userid=id;
    username=name;
    password=passwordt;
    state=status;
}
void User::setuserid(int id){
    userid=id;
}
void User::setusername(string name){
    username=name;
}
void User::setrpassword(string passwordt){
    password=passwordt;
}
void User::setstate(string status){
    state=status;
}
int User::getuserid(){
    return userid;
}
string User::getusername(){
    return username;
}
string User::getpassword(){
    return password;
}
string User::getstate(){
    return state;
}