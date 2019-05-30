

#include "chatroom.h"

Server::Server( uint16_t port ) {
    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );

    if( sockfd < 0 ) {
        perror( "sock creat faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "sock creath OK" << endl;

    Bind( port );
    cout << "服务器已就绪，等待客户端连接......." << endl;
}

Server::~Server() {
    close( sockfd );
}

void Server::Bind( uint16_t port) {
    int ret;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( port );
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );
    //sockaddr_in *ptr = &servaddr;
    ret = bind( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 ) {
        perror( "bind faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "bing Ok" << endl;

    Listen();
}

void Server::Listen() {
    int ret = listen( sockfd, 10 );
    if( ret != 0 ) {
        perror( "listen faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "listen Ok" << endl;
}

int Server::Accept() {
    int connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof( cliaddr );

    connfd = accept( sockfd, ( struct sockaddr* )&cliaddr, &clilen );
    if( connfd == -1 ) {
        perror( "accept faile" );
        exit( EXIT_FAILURE );
    }
    string IP = string( inet_ntoa( cliaddr.sin_addr ) );
    cout << "成功接收一个客户端: " << IP << endl;
    
    return connfd;
}

void Server::CreatePthread( int clientSock ) {
    int ret = pthread_create( &pid, nullptr, handleClient, static_cast< void* >( &clientSock ) );
    if( ret != 0 ) {
        cout << "pthread create faile" << endl;
        exit( EXIT_FAILURE );
    }
    pthread_detach( pid ); // 线程分离
}

void *handleClient( void *arg ) {
    int *clientSock = static_cast<int*>(arg);
    struct package message;
    
    while ( true ) {
        int ret = read( *clientSock, &message, sizeof ( message ) );
        if( ret == -1 ) {
            perror( "read message " );
            break;
        }
        else if( ret == 0 ) {
            cout << "客户端申请退出" << endl;
            break;
        }
        else {
            //客户端的操作
            switch ( message.cmd ) {
                case 1 :
                    registration( *clientSock, message ); //注册
                    break;
                    
                case 2 :
                    login(); //登录
                    break;                
            }
        }
    }
    close( *clientSock );
}

void registration( int clientSock, struct package &message ) {
    cout << message.fromname << " 申请注册" << endl;
}

void login() {
    
}
