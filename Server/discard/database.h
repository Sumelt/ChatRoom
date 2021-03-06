#ifndef DATABASE_H
#define DATABASE_H

#include <mysql_connection.h>
#include <mysql_driver.h>
#include <mysql_error.h>
#include <cppconn/sqlstring.h>
#include <cppconn/statement.h>
using namespace sql;
using namespace std;
const SQLString HOSTNAME = "tcp://127.0.0.1:3306";
const SQLString USERNAME = "root";
const SQLString PASSWORD = "best";

class DataBase
{
private:
    Connection *conn;
    Driver *driver;
    Statement *stmt;
    ResultSet *res;
public:
    DataBase();
    ~DataBase();
    void Creatable();   
    bool ChangeSocket( unsigned int id, int );
    bool ChangeLoginSta( unsigned int id, unsigned int status );
    bool ChangePassWord( struct package& );
    void InsertValue( struct package&, int );
    void DeleteValue( unsigned int id );
    bool CheckMesg( int, struct package& );
    bool LoginCheck( int, struct package& );
    int Calculated();
    void Show();

};

#endif // DATABASE_H
