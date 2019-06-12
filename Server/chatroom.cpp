

#include "chatroom.h"

//类内静态变量声明
int Server::sockfd;
nfds_t Server::curUserCnt;
pthread_mutex_t Server::lock;
struct pollfd *Server::userSet;
struct UserStruct *Server::userMess;


Server::Server( uint16_t port ) {
    memset( &servaddr, 0, sizeof( servaddr ) );
    Server::curUserCnt = 0;
    servlen = sizeof( servaddr );
    
    pthread_mutex_init( &lock, nullptr ); //初始化线程锁    
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
        cout << "用户数量达到上限拒绝连接" << endl;
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
    //SetNoBlock( connfd ); //设置非阻塞
    SetPollEvent( connfd, POLLIN | POLLRDHUP | POLLERR, curUserCnt, false );
    pthread_mutex_unlock( &lock ); //线程解锁
    
    pthread_exit ( nullptr ); //线程结束
}

void* Server::PthreadClearError( void *arg ) {
    int index = *static_cast<int*>( arg );
    cout << "Get An Error From Client: " << userSet[ index ].fd << endl;
    
    char errors[ 100 ];
    memset( errors, '\0', 100 );
    socklen_t length = sizeof( errors );
    
    //清除该套接字的错误
    if( getsockopt( userSet[ index ].fd, SOL_SOCKET, SO_ERROR, &errors, &length ) < 0 ) 
       cout << "Get Socket Option Fail" << endl;
    
    //pthread_exit( nullptr );
}

void* Server::PthreadRecvMess( void *arg ) {
    
    int *index = ( int* )arg;   
    int connfd = userSet[ *index ].fd; //发送消息者
    
    ssize_t byte = read( connfd, userMess[ connfd ].RecvMessage, MAXSIZE );
    if( byte < 0 && errno != EAGAIN  ) {
        perror( "PthreadRecvMess Faile" );
        RemoveUser( *index );
        close( connfd );
    }
    else if( byte > 0 ){
        cout << "服务器收到一条从客户端发来的消息" << endl;
        for ( nfds_t i = 1; i <= MAXUSERS; ++i ) {
            int toconnfd = userSet[ i ].fd; //要广播的用户
            if( connfd != toconnfd ) {
                strcpy( userMess[ toconnfd ].SendMessage, userMess[ connfd ].RecvMessage );
                SetPollEvent( toconnfd, POLLOUT, i, true );
            }
            else continue;
        }
    }
    else cout << "读到了0字节" << endl;
    
    //pthread_exit( nullptr );
}

void* Server::PthreadBroadcast( void *arg ) {
    nfds_t index = *static_cast<nfds_t*>( arg );
    int connfd = userSet[ index ].fd; //发送消息者
    
    send( connfd, userMess[ connfd ].SendMessage, MAXSIZE, 0 );
    SetPollEvent( connfd, POLLIN, index, true );
    memset( userMess[ connfd ].SendMessage, 0, MAXSIZE ); //清空发送区
    pthread_exit( nullptr );
}

void Server::RemoveUser( int index ) {
    
    int connfd = userSet[ index ].fd;
    userMess[ connfd ] = userMess[ userSet[ curUserCnt ].fd ]; //最后一位用户数据覆盖
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

void Server::SetPollEvent( int fd, short status, nfds_t index = curUserCnt, bool opt = false ) {
    //新增事件
    if( !opt ) {
        userSet[ index ].fd = fd; 
        userSet[ index ].events = status; //监听可读事件
        userSet[ index ].revents = 0;
    }
    //修改事件
    else {
        if( status == POLLIN )
            userSet[ index ].events |= ~POLLOUT;
        else if( status == POLLOUT )
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
            perror( "Poll Faile" );
            break;
        }
        for ( int i = 0 ; i <= MAXUSERS; ++i ) {
            //新连接请求
            if( ( userSet[ i ].fd == sockfd ) && ( userSet[ i ].revents & POLLIN ) ) {
                userSet[ i ].revents = 0;
                CreatePthread( PthreadAccept, nullptr );
                //PthreadAccept( nullptr );
            }               
            //套接字出错
            else if( userSet[ i ].revents & POLLERR ) {
                userSet[ i ].revents = 0;
                PthreadClearError( &i );
            }
            //离开
           else if( userSet[ i ].revents & POLLRDHUP ) {
                userSet[ i ].revents = 0;
                RemoveUser( i );
                --i;
            }             
            //有消息读取
            else if( userSet[ i ].revents & POLLIN ) {
                userSet[ i ].revents = 0;
                //int tempIndex = i;
                //CreatePthread( PthreadRecvMess, &tempIndex );
                PthreadRecvMess( &i );
            }              
           //发送广播
           else if( userSet[ i ].revents & POLLOUT ) {
                userSet[ i ].revents = 0;
                //int tempIndex = i;
                //CreatePthread( PthreadBroadcast, &tempIndex );
                PthreadBroadcast( &i );
            }
        }
    }   
}

void Server::Run() {
    PollEvent();
}


