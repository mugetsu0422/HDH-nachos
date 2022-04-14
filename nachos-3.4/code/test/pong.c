#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int i, n = 1000;
    char b = 'B';
    for(i = 0; i < n; i++)
    {
        Write(&b, 1, output);
    }
    return 0;
}
