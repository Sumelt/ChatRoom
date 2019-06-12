

#include "chatroom.h"

int Server::sockfd;
int Server::curUserCnt;
struct pollfd *Server::userSet;
struct UserStruct *Server::userMess;
pthread_mutex_t Server::lock;
pthread_mutex_t Server::lockCreat;

Server::Server( uint16_t port ) {
    memset( &servaddr, 0, sizeof( servaddr ) );
    Server::curUserCnt = 0;
    servlen = sizeof( servaddr );
    
    pthread_mutex_init( &lock, nullptr ); //初始化线程锁
    pthread_mutex_init( &lockCreat, nullptr ); //初始化线程锁
    
    userMess = new UserStruct[ MAXFD ] (); //创建用户的数据结构
    userSet = new pollfd[ MAXUSERS + 1 ] (); //用户的事件集合
    
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if( sockfd < 0 ) {
        perror( "Socket Creat Faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "Socket Creat OK" << endl;

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
    ret = bind( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 ) {
        perror( "Bind Faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "Bing Ok" << endl;

    Listen();
}

void Server::Listen() {
    int ret = listen( Server::sockfd, 10 );
    if( ret != 0 ) {
        perror( "Listen Faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "Listen Ok" << endl;
}

void* Server::PthreadAccept( void *arg ) {
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof( cliaddr );

    int connfd = accept( sockfd, ( struct sockaddr* )&cliaddr, &clilen );
    if( connfd == -1 ) {
        perror( "Accept Faile" );
        pthread_exit ( nullptr );
    }
    //用户数达到上限不执行后续操作
    if( curUserCnt >= MAXUSERS ) {
        const char *str = "User reached the upper limit\n";
        send( connfd, str, strlen( str ), 0 );//线程发送消息后结束
        close( connfd );
        pthread_exit ( nullptr );
    }
    
    string IP = string( inet_ntoa( cliaddr.sin_addr ) );
    cout << "成功接收一个客户端: " << IP << endl;
    
    pthread_mutex_lock( &lock ); //线程加锁
    ++curUserCnt; //新增客户
    userMess[ curUserCnt ].copyAddress( cliaddr ); //服务器保存客户端地址
    SetNoBlock( connfd ); //设置非阻塞
    SetPollEvent( connfd, POLLIN | POLLRDHUP | POLLERR, curUserCnt, false );
    pthread_mutex_unlock( &lock ); //线程解锁
    
    pthread_exit ( nullptr ); //线程结束
}

void* Server::PthreadClearError( void *arg ) {
    int index = *static_cast<int*>( arg );
    cout << "Get an error from" << userSet[ index ].fd << endl;
    
    char errors[ 100 ];
    memset( errors, '\0', 100 );
    socklen_t length = sizeof( errors );
    
    //清除该套接字的错误
    if( getsockopt( userSet[ index ].fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 ) 
       cout << "Get socket option failed" << endl;
    
    pthread_exit( nullptr );
}

void* Server::PthreadRecvMess( void *arg ) {
    int *index = ( int* )arg;
    cout << "index: " << *index << endl;
    cout << "count: " << curUserCnt << endl;
    
    int connfd = userSet[ *index ].fd; //发送消息者
    ssize_t byte = read( connfd, userMess[ connfd ].RecvMessage, MAXSIZE );
    if( byte < 0 && errno != EAGAIN  ) {
        perror( "PthreadRecvMess Faile" );
        RemoveUser( *index );
        close( connfd );
    }
    else if( byte > 0 ){
        cout << "接收到一条消息" << endl;
        for ( int i = 1; i <= MAXUSERS; ++i ) {
            int toconnfd = userSet[ i ].fd; //要广播的用户
            if( connfd != toconnfd ) {
                strcpy( userMess[ toconnfd ].SendMessage, userMess[ connfd ].RecvMessage );
                SetPollEvent( toconnfd, POLLOUT, i, true );
            }
            else continue;
        }
    }
    else {
        cout << "来到这" << endl;
    }
    pthread_exit( nullptr );
}

void* Server::PthreadBroadcast( void *arg ) {
    int index = *static_cast<int*>( arg );
    int connfd = userSet[ index ].fd; //发送消息者
    
    send( connfd, userMess[ connfd ].SendMessage, MAXSIZE, 0 );
    SetPollEvent( connfd, POLLIN, index, true );
    pthread_exit( nullptr );
}

void Server::RemoveUser( int index ) {
    
    int connfd = userSet[ index ].fd;
    userMess[ connfd ] = userMess[ userSet[ curUserCnt ].fd ];
    close( connfd );
    --curUserCnt;
    cout << "A Client Left" << endl;
}

void Server::CreatePthread( void *fnc( void* ), void* arg = nullptr ) {
    pthread_t pid;
    int ret = pthread_create( &pid, nullptr, fnc, arg );
    if( ret != 0 ) {
        cout << "Pthread Create Faile" << endl;
        exit( EXIT_FAILURE );
    }
    pthread_detach( pid ); // 线程分离
}

void Server::SetPollEvent( int fd, short status, int index = curUserCnt, bool opt = false ) {
    //新增事件
    if( !opt ) {
        userSet[ index ].fd = fd; 
        userSet[ index ].events = status; //监听可读事件
    }
    //修改事件
    else {
        if( status & POLLIN )
            userSet[ index ].events |= ~POLLOUT;
        else if( status & POLLOUT )
            userSet[ index ].events |= ~POLLIN;       
        userSet[ index ].events |= status;
    }
}

void Server::PollEvent() {
    memset( userSet, 0, sizeof ( *userSet ) * MAXUSERS );
    //服务器事件加入
    SetPollEvent( sockfd, POLLIN | POLLERR );
       
    while ( true ) {
        int ret = poll( userSet, curUserCnt + 1, -1 ); //堵塞
        if( ret < 0 ) {
            perror( "Poll faile" );
            break;
        }
        for ( int i = 0 ; i <= MAXUSERS; ++i ) {
            //新连接请求
            if( userSet[ i ].fd == sockfd && userSet[ i ].revents & POLLIN ) {
                userSet[ i ].revents = 0;
                CreatePthread( PthreadAccept );
            }               
            //套接字出错
            else if( userSet[ i ].revents & POLLERR ) {
                userSet[ i ].revents = 0;
                int arg = i;
                CreatePthread( PthreadClearError, &arg );
            }
            //有消息读取
            else if( userSet[ i ].revents & POLLIN ) {
                if( pthread_mutex_trylock( &lockCreat ) ) {
                    userSet[ i ].revents = 0;
                    cout << "收到一条消息" << endl;
                    int arg = i;
                    CreatePthread( PthreadRecvMess, &arg );
                    pthread_mutex_unlock( &lockCreat );
                }                    
            }
            //离开
           else if( userSet[ i ].revents & POLLRDHUP ) {
                userSet[ i ].revents = 0;
                RemoveUser( i );
            }               
           //发送广播
           else if( userSet[ i ].revents & POLLOUT ) {
                userSet[ i ].revents = 0;
                int arg = i;
                CreatePthread( PthreadBroadcast, &arg );
            }
        }
    }   
}

void Server::Run() {
    PollEvent();
}

/*
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
*/

/*
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
                QuitChatroom( clientSock, message );
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
*/

/*
void Server::registration( int clientSock, struct package &message ) {
    cout << message.fromname << " 请求注册" << endl;
    DataBase database;
    database.InsertValue( message, clientSock );
}

void Server::login( int clientSock, struct package &message ) {
    cout << message.fromname << " 请求登录" << endl;
    DataBase database;
    if( database.LoginCheck( clientSock, message ) ) {
        
    }
}
*/
