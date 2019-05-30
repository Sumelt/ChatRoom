/*
 *服务器
 */
#include <iostream>
#include "database.h"
#include "chatroom.h"
using namespace std;

int main()
{
    DataBase base;
    base.Creatable();
    //base.InsertValue();
    struct package mes;
    base.Show();
    strcpy( mes.fromname, "xiaomin" );
    strcpy( mes.msg, "666" );
    mes.identity = 1;
    base.InsertValue( mes,99 );
    base.Show();

    return 0;
}
