#include"syscall.h"
#define maxlength 255

int main()
{
	int fileID, temp;
	char* a;
	//Create("Samuel.txt");
	fileID = Open("Samuel.txt", 0);

	Write("Truong Samuel", 13, fileID);
	Seek(0, fileID);
	Read(a, 13, fileID);
	Write(a, 13, 1);

	Close(fileID);
	//Delete("Samuel.txt");
	return 0;
}
