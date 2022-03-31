#include "syscall.h"

OpenFileID openFile(int type) {
    int i = 0;
    OpenFileID input = ConsoleInput;
    OpenFileID file = -1;
    char buffer[256];
    do {
        Read(&buffer[i], 1, input); 
    } while((buffer[i] != '\n') && (buffer[i] != 13) && (buffer[i++] != 0));
    buffer[--i] = 0;
    if(type == 0){
        Create(buffer);
    }
    file = Open(buffer, type);
    return file;
}

int main() {
    OpenFileID output = ConsoleOutput;
    OpenFileID src = openFile(1);
    OpenFileID dst = openFile(0);
    char buffer[1];
    char *failMessage = "Unable to read the target file!";
    if((src < 0) || (dst < 0)){
        Write(failMessage, 1, output);
	Halt();
	return -1;
    }
    while(1){
        int status = Read(buffer, 1, src);
	if(status < 0){
	    Close(src);
	    Close(dst);
	    Halt();
	    return 0;
	}
	Write(buffer, 1, dst);
    }
}
