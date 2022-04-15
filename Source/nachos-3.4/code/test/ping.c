#include "syscall.h"

int main()
{
    OpenFileID input = ConsoleInput;	// 0
    OpenFileID output = ConsoleOutput;	// 1
    int i, n = 1000;
    char a = 'A';
    for(i = 0; i < n; i++)
    {
        Write(&a, 1, output);
    }
    return 0;
}
