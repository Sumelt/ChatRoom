
#include "chat.h"

int main()
{
//    Client client( "127.0.0.1", 6666 );
//    client.Run();
    
    Client *array[ 100 ];
    for ( int i = 0; i< 50; ++i ) {
        pid_t pid;
        pid = fork();
        if( pid == 0 ) {
            array[ i ] = new Client ( "127.0.0.1", 6666 );
            array[ i ]->Run();
            cout << "now: " << i << endl;           
        }
    }
    return 0;
}
