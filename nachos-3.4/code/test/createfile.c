#include "syscall.h"
#define MAXLENGTH 255

int main()
{
	OpenFileID input = ConsoleInput;	// 0
	OpenFileID output = ConsoleOutput;	// 1
	char filename[MAXLENGTH];
	int i = 0;

	Write("Nhap ten file:\n", 32, output);
	do 
    {
        Read(&filename[i], 1, input); 
    } while((filename[i] != '\n') && (filename[i] != 13) && (filename[i++] != 0));
    filename[--i] = 0;
	/*Create a file*/
	if (Create(filename) != -1)
	{
		Write("Tao file thanh cong\n", 32, output);
	}
	else
	{
		Write("Tao file khong thanh cong\n", 32, output);
	}
	Halt();
}
