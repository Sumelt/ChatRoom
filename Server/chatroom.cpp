

#include "chatroom.h"

//设置不阻塞
int Setnoblock( int &fd ) {
    int oldOpt = fcntl( fd, F_GETFL );
    fcntl( fd, F_SETFL, oldOpt | O_NONBLOCK );
    return oldOpt;
}

Server::Server( uint16_t port ) {

    memset( &servaddr, 0, sizeof( servaddr ) );
    servlen = sizeof( servaddr );    
    this->curUserCnt = 0;

    OnlineUsers = new UserStruct[ MAXFD ] (); //创建用户的数据结构,0位服务器占据
    
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
    delete []OnlineUsers;
}

void Server::Bind( uint16_t port) {
    int ret;
    
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons( port );
    servaddr.sin_addr.s_addr = htonl( INADDR_ANY );//绑定地址
    
    ret = bind( sockfd, ( struct sockaddr* )&servaddr, servlen );
    if( ret != 0 ) {
        perror( "Bind Faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "Bing Ok" << endl;

    Listen();
}

void Server::Listen() {
    int ret = listen( this->sockfd, 10 );
    if( ret != 0 ) {
        perror( "Listen Faile" );
        exit( EXIT_FAILURE );
    }
    else cout << "Listen Ok" << endl;
}

void Server::Accept( void *arg ) {
    struct sockaddr_in cliaddr;
    socklen_t clilen = sizeof( cliaddr );

    int connfd = accept( sockfd, ( struct sockaddr* )&cliaddr, &clilen );
    if( connfd == -1 )
        perror( "Accept a new client faile" );
    //用户数达到上限不执行后续操作
    else if( curUserCnt >= MAXUSERS ) {
        cout << "用户数量达到上限拒绝连接" << endl;
        const char *str = "User reached the upper limit\n";
        send( connfd, str, strlen( str ), 0 );//线程发送消息后结束
        close( connfd );
    }
    else {
        string IP = string( inet_ntoa( cliaddr.sin_addr ) );
        cout << "成功接收一个客户端: " << IP << endl;
        
        ++curUserCnt; //新增客户
        OnlineUsers[ connfd ].save_addr = cliaddr; //服务器保存客户端地址
        OnlineUsers[ connfd ].sockfd = connfd;
        que.push_front( connfd );
        Setnoblock( connfd ); //设置非阻塞
        SetEpollEvent( connfd, EPOLL_CTL_ADD, EPOLLIN | EPOLLET ); //开启ET模式
    }
}

void Server::RecvMess( int connfd ) {
   
    ssize_t byte = read( connfd, OnlineUsers[ connfd ].RecvMessage, MAXSIZE );
    OnlineUsers[ connfd ].RecvMessage[ byte ] = '\0';
    char *ptr = OnlineUsers[ connfd ].RecvMessage;
    if( byte <= 0 && errno != EAGAIN  ) {
        SetEpollEvent( connfd, EPOLL_CTL_DEL, 0 );       
        RemoveUser( connfd );
    }
    else if( byte > 0 ){
        cout << "服务器接受到: " << byte << "字节" << endl;
        for ( auto iter = que.begin(); iter != que.end(); ++iter ) {
            if( *iter != connfd ) {
                SetEpollEvent( *iter, EPOLL_CTL_MOD, EPOLLOUT );
                strcpy( OnlineUsers[ *iter ].SendMessage, ptr );
            }
        }
        memset( OnlineUsers[ connfd ].SendMessage, 0, MAXSIZE ); //清空发送区        
    }
    else cout << "读到了0字节" << endl;
}

void Server::Broadcast( int fd ) {
    
    send( fd, OnlineUsers[ fd ].SendMessage, MAXSIZE, 0 );
    SetEpollEvent( fd, EPOLL_CTL_MOD, EPOLLIN );
    memset( OnlineUsers[ fd ].SendMessage, 0, MAXSIZE ); //清空发送区
}

void Server::RemoveUser( int fd ) {
    bzero( &OnlineUsers[ fd ], sizeof ( UserStruct) );
    --curUserCnt;
    close( fd );
    que.remove( fd );
    cout << "A Client Left" << endl;
}

void Server::SetEpollEvent( int fd, int op, uint32_t status ) {
    ep_ctl.data.fd = fd;
    ep_ctl.events = status;
    epoll_ctl( ep_fd, op, fd, &this->ep_ctl );
}

void Server::Event() {
    
    struct epoll_event ep_wait[ MAXUSERS + 1 ];//就绪队列
    //服务器事件加入
    SetEpollEvent( sockfd, EPOLL_CTL_ADD, EPOLLIN );
    OnlineUsers[ curUserCnt ].sockfd = this->sockfd;//保存服务器
       
    while ( true ) {
        int cnt = epoll_wait( ep_fd, ep_wait, MAXUSERS + 1, -1 ); //堵塞
        if( cnt < 0 ) {
            perror( "Poll Faile" );
            break;
        }        
        for ( int i = 0; i < cnt ; ++i ) {
            //新连接
            if( this->sockfd == ep_wait[ i ].data.fd && ep_wait[ i ].events == EPOLLIN )
                Accept( nullptr );
            //接受消息
            else if ( ep_wait[ i ].events & EPOLLIN )
                RecvMess( ep_wait[ i ].data.fd );
            //广播消息
            else if ( ep_wait[ i ].events & EPOLLOUT )
                Broadcast( ep_wait[ i ].data.fd );
        }
    }   
}

void Server::Run() {
    this->ep_fd = epoll_create( MAXUSERS + 1 );
    Event();
}


