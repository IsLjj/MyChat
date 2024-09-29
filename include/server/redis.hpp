#ifndef REDIS_H
#define REDIS_H
#include<hiredis/hiredis.h>
#include<thread>
#include<functional>
using namespace std;
class Redis{
public:
  Redis();
  ~Redis();
  //connect redis
  bool Connect();
  //publish message to redis channel
  bool PubMessage(int channel,string message);
  //subscribe channel
  bool SubMessage(int channel);
  //cancel subscribe channel
  bool UnSubMessage(int channel);
  //function of special thread to receive message from channel
  void ReceChanMess();
  //initialize function
  void InitFuncHanMess(function<void(int,string)> fv);
private:
  //context to publish and subscribe message
  redisContext* pubcontextp;
  redisContext* subcontextp;
  //function to handle message from redis
  function<void(int,string)> FuncHanMess;
};
#endif