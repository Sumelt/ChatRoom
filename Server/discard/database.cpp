
/*
 * 数据库
 */
#include "database.h"
#include "chatroom.h"
DataBase:: DataBase() {
    driver = get_driver_instance();
    try {
        conn = driver->connect( HOSTNAME, USERNAME, PASSWORD );
    } catch (...) {
        cout << "数据库连接失败" << endl;
    }        
}

DataBase::~DataBase() {
    delete conn;
    delete stmt;
    delete res;
}

void DataBase::Creatable() {
    //数据不存在则创建数据库usr
    const SQLString str1 = "create database if not exists usr;";
    //表不存在则创建表user
    // 用户ID 用户昵称 登录密码 套接字 登录状态 用户身份 
    const SQLString str2 = "create table if not exists\
   usr.user( id SMALLINT unsigned not null, name TEXT not null,\
        password TEXT not null, socket INTEGER not null,\
            loginSta INTEGER not null, identity INTEGER not null, primary key( id ) );";

    stmt = conn->createStatement();
    
    try {
        stmt->execute( str1 );
    } catch (...) {
        cout << "数据库创建失败" << endl;
    }
    
    try {
        stmt->execute( str2 );
    } catch (...) {
        cout << "表创建失败" << endl;
    }
}

bool DataBase::ChangeSocket( unsigned int id, int clientSock ) {
    SQLString str = "update usr.user set socket = " + to_string( clientSock ) + 
                        " where id = " + to_string( id );
    stmt = conn->createStatement();
    try {
        stmt->executeUpdate( str );
        cout << "套接字状态更新成功" << endl;
    } catch (...) {
        cout << "套接字状态更新失败" << endl;
        return false ;
    }
    return  true;
}

void DataBase::InsertValue( struct package &message, int clientSock ) {
    SQLString str = "insert into usr.user ( id, name, password, socket, loginSta, identity ) value(";
    stmt = conn->createStatement();
    //获取数据当前的用户数
    size_t nowCount = res->getRow();
    
    //当前的用户数加作为新用户的ＩＤ
    //表的各项属性
    size_t id = nowCount;
    string name = message.fromname;
    string password = message.msg;
    int sockfd = clientSock;
    int loginStatus = 0;
    int identity = message.identity;
    
    //构建ＳＱＬ语句
    str += to_string( id ) + ',' + "\"" + name + "\"," + "\"" + password + "\"," + to_string( sockfd ) + ',' + 
                to_string( loginStatus ) + ',' + to_string( identity ) + ");";
    try {
        stmt->execute( str );
        cout << "数据插入成功" << endl;
        message.cmd = 1001; //操作成功
        write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        cout << message.fromname << " 注册成功" << endl;
    } catch (...) {
        cout << "数据插入失败" << endl;
        message.cmd = -1; 
        write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        return;
    }
}

void DataBase::DeleteValue( unsigned int id ) {
    SQLString str = "delete from usr.user where id =";
    str += to_string( id );
    stmt = conn->createStatement();
    try {
        stmt->execute( str );
        cout << "数据删除成功" << endl;
    } catch (...) {
        cout << "删除数据失败" << endl;
        return;
    }
}

bool DataBase::CheckMesg( int clientSock, struct package &message ) {
    SQLString str = "select socket, loginSta, identity \
        from usr.user where name = " + string( message.fromname ) + 
                                    "\" and id = " + to_string( message.ID ) + 
                                    "\" and password = " + string( message.msg ) + "\"";
    stmt = conn->createStatement();
    try {
        res = stmt->executeQuery( str );
        cout << "数据库校验成功" << endl;
        
    } catch (...) {
        cout << "数据库校验失败" << endl;
        message.cmd = -1; 
        write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        return false;
    }
    
    return  true;
}

bool DataBase::LoginCheck( int clientSock, struct package& message ) {
    
    if( !CheckMesg( clientSock, message ) )
        return  false;
    
    while ( res->next() ) {
        if( res->getInt( 5 ) == 1 ) {
            cout << message.fromname << " 已经在别处登录" << endl;
            message.cmd = -4;
            write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
            return false;
        }
        if( res->getInt( 6 ) == 1 ) {
            cout << message.fromname << " 普通用户验证通过" << endl;
            message.cmd = 1002;
            message.identity = 1;
            write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        }
        else if( res->getInt( 6 ) == 0 ){
            cout << message.fromname << " 管理员验证通过" << endl;
            message.cmd = 1003;
            message.identity = 0;
            write( clientSock, &message, sizeof ( message ) ); //回送客户端告诉执行结果
        }
    }
    
    //更新在线状态
    if( this->ChangeSocket( message.ID, clientSock ) )
        cout << "状态修改成功" << endl;       
    else {
        cout << "状态修改失败" << endl;
        return false;
    }      
    return true;
}

bool DataBase::ChangeLoginSta( unsigned id, unsigned int status ) {
    SQLString str = "update usr.user set loginSta = " + to_string( status ) + 
                        " where id = " + to_string( id );
    stmt = conn->createStatement();
    try {
        stmt->executeUpdate( str );
        cout << "登录状态更新成功" << endl;
    } catch (...) {
        cout << "登录状态更新失败" << endl;
        return false ;
    }
    return  true;
}
int DataBase::Calculated() {
    const SQLString str = "select count( loginSta ) from usr.user where loginSta = 1;";
    stmt = conn->createStatement();
    try {
         res = stmt->executeQuery( str );
         cout << "查询当前在线人数成功" << endl;
    } catch (...) {
        cout << "查询当前在线人数失败" << endl;
        return -1;
    }
    return res->getInt( 1 );
}

bool DataBase::ChangePassWord( struct package& message ) {
    SQLString str = "update usr.user set password = \"" + string( message.msg ) + 
                        "\"where id = " + to_string( message.ID );
    stmt = conn->createStatement();
    try {
        stmt->executeUpdate( str );
        cout << "登录密码更新成功" << endl;
    } catch (...) {
        cout << "登录密码更新失败" << endl;
        return false ;
    }
    return  true;
}

void DataBase::Show() {
    const SQLString str = "select * from usr.user order by id asc;";
    
    stmt = conn->createStatement();
    res = stmt->executeQuery( str );
    while ( res->next() ) {
        cout << "id = " << res->getInt( 1 )
            << " name = " << res->getString( "name" ) << endl;
    }
}    

