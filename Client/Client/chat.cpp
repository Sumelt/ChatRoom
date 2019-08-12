#include "chat.h"

Client::Client( string ip, uint16_t port ) {
    
    bzero( &server_addr, sizeof( server_addr ) );
    server_addr.sin_family = AF_INET;
    if( inet_pton( AF_INET, ip.c_str(), &server_addr.sin_addr ) == 0 ) {
        cout << "Server IP Address Error!";
        exit( EXIT_FAILURE );
    }
    
    server_addr.sin_port = htons( port );
    server_addr_len = sizeof( server_addr );
    
    // create socket
    sockfd = socket( AF_INET, SOCK_STREAM, 0 );
    if(sockfd < 0) {
        cout << "Create Socket Failed!";
        exit( EXIT_FAILURE );
    }
}

Client::~Client() {
    close( sockfd );
}

void Client::Connect()
{
    cout << "Connecting......" << endl;
    if( connect( sockfd, reinterpret_cast<struct sockaddr*>( &server_addr ), server_addr_len ) < 0 ) {
        cout << "Can not Connect to Server IP!";
        exit( EXIT_FAILURE );
    }
    cout << "Connect to Server successfully." << endl;
}


void Client::Event() {
    package mess;
    ssize_t size = 0;
    fd_set readfds;
    char buff[ BUFFER_SIZE ] = { 0 };

    int inputfd = open( "/dev/tty", O_RDONLY | O_NONBLOCK  );
    while ( true ) {
        FD_ZERO( &readfds );
        FD_SET( inputfd, &readfds );
        FD_SET( sockfd, &readfds );
        
        int cnt = select( max( inputfd, sockfd ) + 1, &readfds, nullptr, nullptr, nullptr );
        if( cnt < -1 )
            break;
        //监听标准输入
        if( FD_ISSET( inputfd, &readfds ) ) {
            if( ( size = read( inputfd, buff, BUFFER_SIZE  ) ) > 0 ) {
                mess.type = OTHER;
                strcpy( mess.RecvMessage, buff );
                mess.RecvMessage[ size ] = '\0';
                send( sockfd, &mess, sizeof ( package ), 0 );
                cout << "    --------->聊天消息已发送" << endl;
            }
        }
        //监听套接字
        if( FD_ISSET( sockfd, &readfds ) ) {            
            size = recv( sockfd, &mess, sizeof ( package ), 0 );
            if( mess.type != HEART ) {
                cout << "收到消息--------->:   ";
                cout << mess.RecvMessage;
            }               
        }
    }
}

void Client::Run() {
    pthread_t pid;    

    this->Connect();//创建连接后进行线程创建
    
    int ret = pthread_create( &pid, nullptr, send_heart, reinterpret_cast<void*>( this ) );
    if( ret != 0 ) {
        cout << "Can not create thread!";
        exit( EXIT_FAILURE );
    }
    if( pthread_detach( pid ) != 0 ) {
        perror( "pthread detach faile" );
        exit( EXIT_FAILURE );        
    }
    this->Event();//主线程执行事件处理
}

// thread function
void* send_heart(void* arg) {
    cout << "The heartbeat sending thread started listening .\n";
    Client* cpoint =  reinterpret_cast<Client*>( arg );
    package mess;
    bzero( &mess, sizeof ( mess ) );
    mess.type = HEART;
    
    while( true ) {      
        sleep( 30 );
        send( cpoint->sockfd, &mess, sizeof ( package ), 0 );
    }
}
