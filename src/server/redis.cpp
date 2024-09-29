#include "redis.hpp"
#include <iostream>
using namespace std;
Redis::Redis():pubcontextp(nullptr),subcontextp(nullptr){}
Redis::~Redis()
{
   if(pubcontextp!=nullptr){
     redisFree(pubcontextp);
   }
   if(subcontextp!=nullptr){
    redisFree(subcontextp);
   }
}
void Redis::InitFuncHanMess(function<void(int,string)> fv){
   FuncHanMess=fv;
}
bool Redis::Connect(){
    pubcontextp=redisConnect("127.0.0.1",6379);
    if(pubcontextp==nullptr){
        cerr<<"connect redis failed"<<endl;
        return false;
    }
    subcontextp=redisConnect("127.0.0.1",6379);
    if(subcontextp==nullptr){
       cerr<<"connect redis failed"<<endl;
       return false;
    }
    thread t([&](){ReceChanMess();});
    t.detach();
    cout<<"Connect Redis success"<<endl;
    return true;
}
void Redis::ReceChanMess(){
    redisReply* replyp;
    while(redisGetReply(this->subcontextp,(void **)(&replyp))==REDIS_OK){
       if(replyp!=nullptr&&replyp->element[2]!=nullptr&&replyp->element[2]->str!=nullptr){
          FuncHanMess(atoi(replyp->element[1]->str),replyp->element[2]->str);
       }
       freeReplyObject(replyp); 
    }
}
bool Redis::PubMessage(int channel,string message){
    redisReply* replyp=(redisReply*)redisCommand(pubcontextp,"PUBLISH %d %s",channel,message.c_str());
    if(replyp==nullptr){
        cerr<<"redis publish message command failed"<<endl;
        return false;
    }
    freeReplyObject(replyp);
    return true;
}
bool Redis::SubMessage(int channel){
    //no redisGetReplay to block thread which working for chatserver
    if(redisAppendCommand(this->subcontextp,"SUBSCRIBE %d",channel)==REDIS_ERR){
        cerr<<"redis subscribe message command failed"<<endl;
        return false;
    }
    int done=0;
    while(!done){
        if(redisBufferWrite(this->subcontextp,&done)==REDIS_ERR){
            cerr<<"redis subscribe message command failed"<<endl;
            return false;
        }
    }
    return true;
}
bool Redis::UnSubMessage(int channel){
    //no redisGetReplay to block thread which working for chatserver
    if(redisAppendCommand(this->subcontextp,"UNSUBSCRIBE %d",channel)==REDIS_ERR){
        cerr<<"redis unsubscribe message command failed"<<endl;
        return false;
    }
    int done=0;
    while(!done){
        if(redisBufferWrite(this->subcontextp,&done)==REDIS_ERR){
            cerr<<"redis unsubscribe message command failed"<<endl;
            return false;
        }
    }
    return true;
}



