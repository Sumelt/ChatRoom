
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

void DataBase::ChangeValue( unsigned int id ) {
    SQLString str = "update usr.user set name = \"su\" where id =";
    str += to_string( id );
    stmt = conn->createStatement();
    try {
        stmt->executeUpdate( str );
        cout << "数据更新成功" << endl;
    } catch (...) {
        cout << "数据更新失败" << endl;
    }
}

void DataBase::InsertValue( struct package &message, int clientSock  ) {
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
    str += to_string( id ) + ',' + name + ',' + password + ',' + to_string( sockfd ) + ',' + 
                to_string( loginStatus ) + ',' + to_string( identity ) + ");";
    cout << str << endl;
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
    }
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

