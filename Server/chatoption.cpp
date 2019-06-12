/*
 * 聊天室功能选择
 * 
 */ 
#include "chatroom.h"
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


