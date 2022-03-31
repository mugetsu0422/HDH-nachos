#include "syscall.h"

int main() {
    //Based on shell.c
    OpenFileID input = ConsoleInput;
    OpenFileID output = ConsoleOutput;
    char buffer[256];
    int i;
    while(1){
	i = 0;
	do {
	    Read(&buffer[i], 1, input); 
	} while((buffer[i] != '\n') && (buffer[i] != 13) && (buffer[i++] != 0));

	buffer[--i] = '\0';
	Write(buffer, i, output);
    }
}
