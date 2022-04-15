#include "syscall.h"
#define MAXLENGTH 255

int main() 
{
	OpenFileID input = ConsoleInput;	// 0
	OpenFileID output = ConsoleOutput;	// 1
	char srcName[MAXLENGTH], dstName[MAXLENGTH];
	OpenFileID src;
	OpenFileID dst;
	char buffer[1];
	char *failMessage = "Unable to read the target file!";
    int i = 0;

	// Lay ten file
	Write("Nhap ten file goc:\n", 32, output);
	do 
    {
        Read(&srcName[i], 1, input); 
    } while((srcName[i] != '\n') && (srcName[i] != 13) && (srcName[i++] != 0));
    srcName[--i] = 0;

	Write("\nNhap ten file dich:\n", 32, output);
    i = 0;
	do 
    {
        Read(&dstName[i], 1, input); 
    } while((dstName[i] != '\n') && (dstName[i] != 13) && (dstName[i++] != 0));
    dstName[--i] = 0;

	// Mo src file
	src = Open(srcName, 1);
	// Tao va mo des file 
	Create(dstName);
	dst = Open(dstName, 0);


	if((src < 0) || (dst < 0))
	{
		Write(failMessage, 64, output);
		Halt();
		return -1;
	}
	// Doc noi dung file goc va chuyen den file dich
	while(1)
	{
		int status = Read(buffer, 1, src);
		if(status < 0)
		{
			Close(src);
			Close(dst);
			Halt();
			return 0;
		}
	Write(buffer, 1, dst);
	}
	return 0;
}