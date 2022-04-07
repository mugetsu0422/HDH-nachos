#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int i, n = 1000;
    for(i = 0; i < n; i++)
    {
        Write('B', 1, output);
    }
    return 0;
}