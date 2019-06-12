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

const int MAXSIZE = 1024; //缓冲区最大的字节数
const int MAXUSERS = 5; //最大数量的用户
const int MAXFD = 65535; 

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

//设置不阻塞
static int SetNoBlock( int &fd ) {
    int oldOpt = fcntl( fd, F_GETFL );
    fcntl( fd, F_SETFL, oldOpt | O_NONBLOCK );
    return oldOpt;
}

class Server {

private:
    struct sockaddr_in servaddr;
    socklen_t servlen;
       
    int sockfd;    
    struct pollfd *userSet;
    struct UserStruct *userMess;
    int curUserCnt;
    pthread_mutex_t lock;
    
    void SetPollEvent( int fd, short status, int index, bool opt ); 
    void Accept( void* );
    void RecvMess( int );
    void ClearError( int );
    void Broadcast( int );    
    void RemoveUser( int index );   
    
    void Bind( uint16_t );
    void Listen();
    void PollEvent();   
    void CreatePthread( void*( void* ), void* arg );
    
public:
    Server( uint16_t );
    ~Server();
    void Run();

};




#endif // CHATROOM_H
