/*
 * 聊天室功能选择
 * 
 */ 
#include "chatroom.h"

/**以下代码涉及繁杂的业务逻辑，暂时不给予考虑***/

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


/*
void Server::QuitChatroom( int clientSock, struct package &message ) {
    cout << message.fromname << " 申请退出聊天室" << endl;
    DataBase database;
    if( database.ChangeLoginSta( message.ID, 0 ) ) {
        cout << message.fromname << " 退出聊天室成功" << endl;
        write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
    }
    else {
        cout << message.fromname << " 退出聊天室失败" << endl;
    }
}

void Server::Display( int clientSock ) {
    DataBase database;
    struct package message;
    int count = database.Calculated();
    if( count != -1 )
        strcpy( message.msg, to_string( count ).c_str() );
    else
        strcpy( message.msg, to_string( 0 ).c_str() );
    write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
}

void Server::ChangePassWord( int clientSock, struct package &message ) {
    cout << message.fromname << " 申请修改密码" << endl;
    DataBase database;
    if( !database.CheckMesg( clientSock, message ) ) {
        cout << message.fromname << " 验证不通过，申请修改密码失败" << endl;
        message.cmd = 9010;
        write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        return;
    }
    else {
         message.cmd = 9009;//验证通过
         write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
         ssize_t ret = read( clientSock, &message, sizeof ( message ) ); //读取新密码
         if( ret > 0 && database.ChangePassWord( message ) ) {
             cout << message.fromname << " 申请修改密码成功" << endl;
         }
    }

} 
*/




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

/*
struct package {
    unsigned int ID;         //账号ＩＤ
    char msg[1024];         // 消息内容
    int  cmd;               // 消息类型
    char filename[50];      // 保存文件名
    char toname[20];        // 接收者姓名
    char fromname[20];      // 发送者姓名
    int  identity;          // 用户状态（0：管理员、1：普通用户、2：被禁言）
};
*/
