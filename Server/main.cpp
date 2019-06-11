/*
 *服务器
 */
#include <iostream>
#include "database.h"
#include "chatroom.h"
using namespace std;

int main()
{
    Server serv( 6666 );
    serv.Run();

    return 0;
}
