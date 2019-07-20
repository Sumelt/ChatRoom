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
#include <fcntl.h>
#include <sys/epoll.h>

#include <list>

using namespace std;

const int MAXSIZE = 1024; //缓冲区最大的字节数
const int MAXUSERS = 5; //最大数量的用户
const int MAXFD = 65535; 

struct UserStruct {
    struct sockaddr_in save_addr; //客户机的地址信息
    int sockfd;//客户机的套接字
    char SendMessage[ MAXSIZE];//客户机要发送的消息
    char RecvMessage[ MAXSIZE ];//客户机接受的消息
    
    UserStruct() : sockfd( 0 ) {
        bzero( &save_addr, sizeof ( save_addr ) );
        bzero( SendMessage, sizeof ( SendMessage ) );
        bzero( &RecvMessage, sizeof ( RecvMessage ) );
    }
};

//设置不阻塞
int Setnoblock( int &fd );

class Server {

private:
    struct sockaddr_in servaddr;//服务器信息
    struct epoll_event ep_ctl;//epoll event结构体
    struct UserStruct *OnlineUsers;//当前在线的用户
    list<int>que; //当前在线用户的fd
    socklen_t servlen;
       
    int sockfd, ep_fd;
    int curUserCnt;
    
    void SetEpollEvent( int fd, int op, uint32_t status );

    void RecvMess( int );
    void Broadcast( int );   
    void ClearError( int );    
    void RemoveUser( int index );   

    void Accept( void* );    
    void Bind( uint16_t );
    void Listen();
    
    void Event();   
    
public:
    Server( uint16_t );
    ~Server();
    void Run();

};




#endif // CHATROOM_H
