/*
 *服务器
 */
#include <iostream>
#include "chatroom.h"
using namespace std;

int main()
{
    Server serv( 5555 );
    serv.Run();

    return 0;
}
