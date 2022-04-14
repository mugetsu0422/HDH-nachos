#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int pingID, pongID;
    Write("Ping-Pong test starting ...\n\n", 32, output);
    pingID = Exec("./test/ping");
    pongID = Exec("./test/pong");
    while(1);
    return 0;
}
