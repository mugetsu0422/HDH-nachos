#include "syscall.h"

void doDelete() 
{
    int i = 0;
    OpenFileID input = ConsoleInput;
    OpenFileID file = -1;
    char buffer[256];
    do 
    {
        Read(&buffer[i], 1, input); 
    } while((buffer[i] != '\n') && (buffer[i] != 13) && (buffer[i++] != 0));
    buffer[--i] = 0;
    Delete(buffer);
}

int main() {
    OpenFileID output = ConsoleOutput;
    Write("Nhap ten file muon xoa:\n", 32, output);
    doDelete();
    Halt();
    return 0;
}
