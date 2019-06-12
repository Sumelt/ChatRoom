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
#include <poll.h>
#include <fcntl.h>

#include "database.h"
using namespace std;

const int MAXSIZE = 1024;
const int MAXUSERS = 5;
const int MAXFD = 65535;

struct package {
    unsigned int ID;         //账号ＩＤ
    char msg[1024];         // 消息内容
    int  cmd;               // 消息类型
    char filename[50];      // 保存文件名
    char toname[20];        // 接收者姓名
    char fromname[20];      // 发送者姓名
    int  identity;          // 用户状态（0：管理员、1：普通用户、2：被禁言）
};

struct UserStruct {
    struct sockaddr_in servaddr;
    char SendMessage[ MAXSIZE];
    char RecvMessage[ MAXSIZE ];
    
    UserStruct() {
        memset( &servaddr, 0, sizeof ( servaddr ) );
        memset( SendMessage, 0, MAXSIZE );
        memset( RecvMessage, 0, MAXSIZE );
    }
    void copyAddress( struct sockaddr_in &addr ) {
        servaddr = addr;
    }
};

static int SetNoBlock( int &fd ) {
    int oldOpt = fcntl( fd, F_GETFL );
    fcntl( fd, F_SETFL, oldOpt | O_NONBLOCK );
    return oldOpt;
}

class Server {

private:
    struct sockaddr_in servaddr;
    socklen_t servlen;
       
    static int sockfd;    
    static struct pollfd *userSet;
    static struct UserStruct *userMess;
    static  nfds_t curUserCnt;
    static pthread_mutex_t lock;
    static pthread_mutex_t lockCreat;
    
    static void SetPollEvent( int fd, short status, nfds_t index,bool opt ); 
    static void RemoveUser( int index );

    static void *PthreadAccept( void* );
    static void *PthreadRecvMess( void* );
    static void *PthreadClearError( void* );
    static void *PthreadBroadcast( void* );    
       
    void CreatePthread( void*( void* ), void* arg );
    void Bind( uint16_t );
    void Listen();
    void PollEvent();

    
    /*
    //---暂时不涉及方法
    static void *handleClient( void *arg );
    static void registration( int, struct package& );
    static void login( int, struct package& );
    
    static void UserTodo( int );
    static void QuitChatroom( int, struct package& );
    static void Display( int );
    static void GroupChat();
    static void PrivateChat();
    static void ConveyFile();
    static void AcceptFile();
    static void ConveyFileChose();
    static void ConveyFileComplete();
    static void RefuseFile();
    static void ChangePassWord( int, struct package& );
    static void DeleteUser();
    static void Silent();
    static void RemoveSilent();
    static void KickOut();
    */

public:
    Server( uint16_t );
    ~Server();
    void Run();

};




#endif // CHATROOM_H
