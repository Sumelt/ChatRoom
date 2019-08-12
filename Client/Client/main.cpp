
#include "chat.h"

int main()
{
    Client client( "127.0.0.1", 6666 );
    client.Run();
    
    return 0;
}
