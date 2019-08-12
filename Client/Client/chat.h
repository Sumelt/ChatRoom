#ifndef CHAT_H
#define CHAT_H

#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>       //select
#include <sys/types.h>
#include <sys/select.h>
#include <pthread.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

#define BUFFER_SIZE 1024

//自定义的消息包体
enum Type {HEART, OTHER};
struct package{
    Type type;//包的类型，区别与心跳包和正常消息包
    int length;
    char RecvMessage[ BUFFER_SIZE ];//客户机接受的消息
    package() {
        bzero( &RecvMessage, sizeof ( RecvMessage ) );
    }
};

class Client {

friend void* send_heart(void* arg);
private:
    struct sockaddr_in server_addr;
    socklen_t server_addr_len;
    int sockfd;
        
    void Connect();
    void Event();
    
public:
    Client(string ip, uint16_t port);
    ~Client();
    void Run();
};

void *send_heart( void *arg );

#endif // CHAT_H
