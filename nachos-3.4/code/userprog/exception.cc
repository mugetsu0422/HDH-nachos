// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"

#define MaxFileLength 32

//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

void increasePC()
{
	// Di chuyển thanh ghi PC
	int counter = machine->ReadRegister(PCReg);
	machine->WriteRegister(PrevPCReg, counter);
	counter = machine->ReadRegister(NextPCReg);
	machine->WriteRegister(PCReg, counter);
	machine->WriteRegister(NextPCReg, counter + 4);
}

char* User2System(int virtAddr,int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit +1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf,0,limit+1);
	//printf("\n Filename u2s:");
	for (i = 0 ; i < limit ;i++){
		machine->ReadMem(virtAddr+i,1,&oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}



int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i ++;
	}while(i < len && oneChar != 0);
	return i;
}




void ExceptionHandler(ExceptionType which)
{
    int syscallType = machine->ReadRegister(2);

	switch (which) 
	{
	case NoException:
		return;

	case SyscallException:
		switch (syscallType)
		{
		case SC_Halt:
			DEBUG('a',"\n Shutdown++ initiated by user program.");
			printf ("\n\n Shutdown__ initiated by user program.");
			interrupt->Halt();
			break;

		case SC_Create:
			int virtAddr;
			char* filename;
			DEBUG('a',"\n SC_Create call ...");
			DEBUG('a',"\n Reading virtual address of filename");
			// Lấy tham số tên tập tin từ thanh ghi r4
			virtAddr = machine->ReadRegister(4);
			DEBUG ('a',"\n Reading filename.");
			filename = User2System(virtAddr, MaxFileLength + 1);
			if (filename == NULL)
			{
				printf("\n Not enough memory in system");
				DEBUG('a',"\n Not enough memory in system");
				machine->WriteRegister(2,-1); // trả về lỗi cho chương trình người dùng
				delete filename;
				return;
			}
			DEBUG('a',"\n Finish reading filename.");
			//DEBUG(‘a’,"\n File name : '"<<filename<<"'");
			// Create file with size = 0
			// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
			// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
			// hành Linux, chúng ta không quản ly trực tiếp các block trên
			// đĩa cứng cấp phát cho file, việc quản ly các block của file
			// trên ổ đĩa là một đồ án khác
			if (!fileSystem->Create(filename,0))
			{
				printf("\n Error create file '%s'",filename);
				machine->WriteRegister(2,-1);
				delete filename;
				return;
			}
			machine->WriteRegister(2,0); // trả về cho chương trình người dùng thành công
			increasePC();
			delete filename;
			break;

		case SC_Open:
			// Đọc tham số thứ 1 từ thanh ghi r4
			int virtAddr = machine->ReadRegister(4);
			// Đọc tham số thứ 2 từ thanh ghi r5
			int type = machine->ReadRegister(5);
			// 0: đọc và ghi
			// 1: chỉ đọc
			// 2: console input
			// 3: console output
			if(type < 0 || type > 4)
			{
				machine->WriteRegister(2, -1);
				printf("\n Tham so type sai quy dinh");
				return;
			}
			char* filename;
			// Đọc tên file từ địa chỉa thanh ghi r4
			filename = User2System(virtAddr, MaxFileLength + 1);
			// Tìm ô còn trống
			int freeSlot = -1;
			for(int i = 2; i < 10; i++)
			{
				if(fileSystem->table[i] != NULL)
				{
					freeSlot = i;
					break;
				}
			}
			if(freeSlot != -1)
			{
				// Mở file đọc và ghi; và file chỉ đọc
				if(type == 0 || type == 1)
				{
					fileSystem->table[freeSlot] = fileSystem->Open(filename, type);
					if(fileSystem->table[freeSlot] != NULL)
						machine->WriteRegister(2, freeSlot);	// ghi OpenFileID vào thanh ghi r2
				}
				else if(type == 2)	// mở file console input
				{
					fileSystem->table[0] = fileSystem->Open("stdin", 2);
					machine->WriteRegister(2, 0);
				}
				// Mở file console output
				else
				{
					fileSystem->table[1] = fileSystem->Open("stdout", 3);
					machine->WriteRegister(2, 1);
				}
			}
			else
				machine->WriteRegister(2, -1);
			delete[] filename
			increasePC();
			break;

		case SC_Close:
			// lấy file id từ thanh ghi r4
			int fileID = machine->ReadRegister(4);
			if(fileID >= 0 && fileID <= 9)
			{
				if(fileSystem->table[fileID] != NULL)
				{
					delete fileSystem->table[fileID];
					fileSystem->table[fileID] = NULL;
					machine->WriteRegister(2, 0);
				}
			}
			else
			{
				machine->WriteRegister(2, -1);
			}
			increasePC();
			break;

		default:
			printf("\n Unexpected user mode exception (%d %d)", which,type);
			interrupt->Halt();
		}
	default:
		break;
		
	}

}
