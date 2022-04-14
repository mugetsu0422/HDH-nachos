#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int pingID, pongID;
    char a = '\n';
    Write("Ping-Pong test starting ...\n\n", 32, output);
    pingID = Exec("./test/ping");
    pongID = Exec("./test/pong");
    Write("Start of scheduler\n", 32, output);
    Sleep(pingID);
    Sleep(pongID);
    Write(&a, 1, output);
    Write("End of scheduler\n", 32, output);
    return 0;
}
