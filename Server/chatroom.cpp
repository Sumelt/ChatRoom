

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

void* Server::handleClient( void *arg ) {
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
                    login( *clientSock, message ); //登录
                    break;                
            }
        }
    }
    close( *clientSock );
}

void Server::UserTodo( int clientSock ) {
    struct package message;
    ssize_t ret;
    while ( true ) {
        ret = read( clientSock, &message, sizeof ( message ) );
        if( ret == -1 ) {
            perror( "读取用户报文操作失败" );
            return;
        }
        else if( ret == 0 ) {
            cout << "用户返回登录界面" << endl;
            break;
        }
        switch ( message.cmd ) {
            case 10 : 
                QuitChatroom();
                break;
            
            case 1 :
                Display();
                break;
                
            case 2 :
                GroupChat();
                break;
                
            case 3 :
                PrivateChat();
                break;
                
            case 5 :
                ConveyFile();
                break;
                
            case 6 :
                ChangePassWord();
                break;
                
            case 8 :
                DeleteUser();
                break;
                
            case 9005 :
                RefuseFile();
                break;
                
            case 9006 :
                AcceptFile();
                break;
                
            case 9007 :
                ConveyFileChose();
                break;
                
            case 9008:
                ConveyFileComplete();
                break;
                
            case 9011:
                Silent();
                break;
            case 9012:
                RemoveSilent();
                break;
                
            case 9013:
                KickOut();
                break;      
        }
    }
}

void Server::registration( int clientSock, struct package &message ) {
    cout << message.fromname << " 请求注册" << endl;
    DataBase database;
    database.InsertValue( message, clientSock );
}

void Server::login( int clientSock, struct package &message ) {
    cout << message.fromname << " 请求登录" << endl;
    DataBase database;
    if( database.CheckValue( message, clientSock ) ) {
        
    }
}
