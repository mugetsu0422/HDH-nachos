#include "syscall.h"
#define MAXLENGTH 255

int main() {
    OpenFileID input = ConsoleInput;
    OpenFileID output = ConsoleOutput;
    OpenFileID file = -1;
    char buffer[MAXLENGTH];
    char *failMessage = "Unable to read the target file!\n";
    int i = 0;

    do 
    {
        Read(&buffer[i], 1, input); 
    } while((buffer[i] != '\n') && (buffer[i] != 13) && (buffer[i++] != 0));
    buffer[--i] = 0;
    file = Open(buffer, 1);
    if(file < 0)
    {
        Write(failMessage, 64, output);
        Halt();
        return -1;
    }
    while(1)
    {
        int status = Read(buffer, 1, file);
	    if(status < 0)
        {
            Close(file);
            Halt();
            return 0;
    	}
	Write(buffer, 1, output);
    }
}
