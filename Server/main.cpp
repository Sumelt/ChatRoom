/*
 *服务器
 */
#include <iostream>
#include "chatroom.h"
using namespace std;

static uint16_t myPort = 6666;

int main()
{
    Server serv( myPort );
    serv.Run();

    return 0;
}
