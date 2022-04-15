#include "syscall.h"
#define MAXLENGTH 255

int main() 
{
    //Based on shell.c
    OpenFileID input = ConsoleInput;
    OpenFileID output = ConsoleOutput;
    char buffer[MAXLENGTH];
    int i;

    while(1)
    {
	i = 0;
	do 
    {
	    Read(&buffer[i], 1, input); 
	} while((buffer[i] != '\n') && (buffer[i] != 13) && (buffer[i++] != 0));

	buffer[--i] = '\n';
	buffer[++i] = '\n';
	buffer[++i] = '\0';

	Write(buffer, i, output);
	// Dat lai chuoi ve 0
	while(i >= 0)
	{
		buffer[i] = 0;
		i--;
	}
	
    }
}
