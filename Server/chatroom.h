#ifndef CHATROOM_H
#define CHATROOM_H

#include <iostream>
#include <cstring>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>       //select
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include <pthread.h>

#include "database.h"
using namespace std;

struct package {
    char msg[1024];         // 消息内容
    int  cmd;               // 消息类型
    char filename[50];      // 保存文件名
    char toname[20];        // 接收者姓名
    char fromname[20];      // 发送者姓名
    int  identity;               // 用户状态（0：管理员、1：普通用户、2：被禁言）
};

class Server {

private:
    struct sockaddr_in servaddr;
    socklen_t servlen;
    int sockfd;
    pthread_t pid; 

public:
    Server( uint16_t );
    ~Server();
    void Bind( uint16_t );
    void Listen();
    int Accept();
    void Run();
    void CreatePthread( int  );

};

void *handleClient( void *arg );
void registration( int, struct package& );
void login();
#endif // CHATROOM_H
