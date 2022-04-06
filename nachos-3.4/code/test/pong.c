#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int i;
    int n = 1000;
    for(int i = 0; i < n; i++)
    {
        Write('B', 1, output);
    }
    return 0;
}