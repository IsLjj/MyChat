#ifndef PUBLIC_H
#define PUBLIC_H
#include<string>
#include<iostream>
using namespace std;
#include<muduo/base/Logging.h>
const int maxofflinemessage=500;
inline bool mreplaces(char (&olds)[],char from,int maxsize){
   int num=0,oldsl=strlen(olds);
   for(int i=0;i<oldsl;i++){
       if(olds[i]==from){
          num++;
       }
   }
   if(num+oldsl<=maxsize){
      int i=0;
      while(i<=num+oldsl-1){
         if(olds[i]==from){
            for(int it=oldsl-1;it>i-1;it--){
               olds[it+1]=olds[it];
            }
            olds[i]='\\';
            oldsl++;
             i+=2;
         }
         else{
            i++;
         }
      }
      olds[oldsl]='\0';
      return true;
   }
   else{
      return false;
   }
}
/*
inline void Erased(string&olds,char a){
   cout<<olds;
   int oldsl=olds.size();
   cout<<oldsl;
   olds.erase(0,1);
   olds.erase(olds.size()-1,1);
   cout<<olds;
   for(int i=0;i<olds.size()-1;i++){
      if(olds[i]==a){
         olds.erase(i,1);
      }
      cout<<olds;
   }
}*/
#endif