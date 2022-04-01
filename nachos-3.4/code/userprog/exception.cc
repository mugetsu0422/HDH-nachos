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

#define MaxFileLength 255
// type cho việc mở file
#define ReadWrite 0
#define ReadOnly 1

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

char* User2System(int virtAddr, int limit)
{
	int i;// index
	int oneChar;
	char* kernelBuf = NULL;
	kernelBuf = new char[limit + 1];//need for terminal string
	if (kernelBuf == NULL)
		return kernelBuf;
	memset(kernelBuf, 0, limit + 1);
	//printf("Filename u2s:\n");
	for (i = 0 ; i < limit ;i++){
		machine->ReadMem(virtAddr + i, 1, &oneChar);
		kernelBuf[i] = (char)oneChar;
		//printf("%c",kernelBuf[i]);
		if (oneChar == 0)
			break;
	}
	return kernelBuf;
}

int System2User(int virtAddr, int len, char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar= (int) buffer[i];
		machine->WriteMem(virtAddr + i, 1, oneChar);
		i++;
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

	case PageFaultException:
			DEBUG('a', "\nPage fault.\n");
			printf("Page fault.\n");
			interrupt->Halt();
			break;

	case ReadOnlyException:
		DEBUG('a', "\nPage marked read-only\n");
		printf("Page marked read-only\n");
		interrupt->Halt();
		break;

	case BusErrorException:
		DEBUG('a', "\nInvalid physical address\n");
		printf("Invalid physical address\n");
		interrupt->Halt();
		break;

	case AddressErrorException:
		DEBUG('a', "\n Address error.\n");
		printf(" Address error.\n");
		interrupt->Halt();
		break;

	case OverflowException:
		DEBUG('a', "\nOverflow !!!\n");
		printf("Overflow !!!\n");
		interrupt->Halt();
		break;

	case IllegalInstrException:
		DEBUG('a', "\nIllegal instr.\n");
		printf("Illegal instr.\n");
		interrupt->Halt();
		break;

	case NumExceptionTypes:
		DEBUG('a', "\nNumber exception types\n");
		printf("Number exception types\n");
		interrupt->Halt();
		break;

	case SyscallException:
		switch (syscallType)
		{
			case SC_Halt:
				DEBUG('a',"Shutdown++ initiated by user program.\n");
				printf ("Shutdown__ initiated by user program.\n");
				interrupt->Halt();
				break;

			// ---------------------------------------------------------------- //

			case SC_Create:
			{
				int virtAddr;
				char* filename;
				// Lấy tham số tên tập tin từ thanh ghi r4
				virtAddr = machine->ReadRegister(4);
				filename = User2System(virtAddr, MaxFileLength);
				if (filename == NULL)
				{
					printf("Create: He thong khong du bo nho\n");
					DEBUG('a',"\nCreate: He thong khong du bo nho\n");
					machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng
					delete[] filename;
					break;
				}
				// Create file with size = 0
				// Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
				// việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
				// hành Linux, chúng ta không quản ly trực tiếp các block trên
				// đĩa cứng cấp phát cho file, việc quản ly các block của file
				// trên ổ đĩa là một đồ án khác
				if (!fileSystem->Create(filename, 0))
				{
					printf("Create: Khong the tao file '%s'\n",filename);
					machine->WriteRegister(2, -1);
					delete[] filename;
					break;
				}
				machine->WriteRegister(2,0); // trả về cho chương trình người dùng thành công
				delete[] filename;
				break;
			}

			// ---------------------------------------------------------------- //

			case SC_Open:
			{
				// OpenFileId Open(char *name, int type);
				// Đọc tham số thứ 1 từ thanh ghi r4
				int virtAddr = machine->ReadRegister(4);
				// Đọc tham số thứ 2 từ thanh ghi r5
				int type = machine->ReadRegister(5);
				// 0: đọc và ghi
				// 1: chỉ đọc
				if(type < 0 || type > 2)
				{
					machine->WriteRegister(2, -1);
					printf("Open: Tham so type sai quy dinh\n");
					break;
				}
				char* filename;
				// Đọc tên file từ địa chỉ thanh ghi r4
				filename = User2System(virtAddr, MaxFileLength);
				// Tìm ô còn trống
				int freeSlot = -1;
				for(int i = 2; i < 10; i++)
				{
					if(fileSystem->table[i] == NULL)
					{
						freeSlot = i;
						break;
					}
				}
				if(freeSlot != -1)
				{
					// Mở file đọc và ghi; và file chỉ đọc
					if(type == ReadWrite || type == ReadOnly)
					{
						fileSystem->table[freeSlot] = fileSystem->Open(filename, type);
						if(fileSystem->table[freeSlot] != NULL)
							machine->WriteRegister(2, freeSlot);	// ghi OpenFileID vào thanh ghi r2
						else
						{
							printf("Open: File khong ton tai\n");
							machine->WriteRegister(2, -1);
						}
					}
				}
				else
					machine->WriteRegister(2, -1);
				delete[] filename;
				break;
			}

			// ---------------------------------------------------------------- //

			case SC_Close:
			{
				// int Close(OpenFileId id);
				// lấy file id từ thanh ghi r4
				int fileID = machine->ReadRegister(4);
				if(fileID >= 2 && fileID <= 9)
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
					printf("Close: FileID phai nam trong doan [2, 9]\n");
					machine->WriteRegister(2, -1);
				}
				break;
			}

			// ---------------------------------------------------------------- //

			case SC_Read:
			{
				// int Read(char *buffer, int charcount, OpenFileId id);
				int virtAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int fileID = machine->ReadRegister(6);
				char* buffer = new char[MaxFileLength];

				// fileID không hợp lệ
				if(fileID < 0 || fileID > 9)
				{
					printf("Read: FileID phai nam trong doan [0, 9]\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// File không tồn tại
				if(fileSystem->table[fileID] == NULL && fileID > 1)
				{
					printf("Read: File khong ton tai\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// Console input
				if(fileID == 0)
				{
					// Dùng hàm Read của SynchConsole để đọc từ console output
					int n = gSynchConsole->Read(buffer, charcount);
					// Nếu đọc đến cuối file
					// Chuyển chuỗi đọc được từ console output sang cho user
					System2User(virtAddr, n, buffer);
					machine->WriteRegister(2, n);
					break;
				}
				// Console output
				else if(fileID == 1)
				{
					printf("Read: Khong the doc console output\n");
					machine->WriteRegister(2, -1);
					break;
				}
				// File bình thường
				else
				{
					int n = fileSystem->table[fileID]->Read(buffer, charcount);
					// Nếu đọc đến cuối file
					if(n < charcount)
					{
						machine->WriteRegister(2, -2);
					}
					else
					{
						// Chuyển chuỗi đọc được từ console output sang cho user
						machine->WriteRegister(2, n);
						
					}
					System2User(virtAddr, n, buffer);
					break;
				}
				delete[] buffer;
			}

			// ---------------------------------------------------------------- //

			case SC_Write:
			{
				// int Write(char *buffer, int charcount, OpenFileId id);
				int virtAddr = machine->ReadRegister(4);
				int charcount = machine->ReadRegister(5);
				int fileID = machine->ReadRegister(6);
				char* buffer;

				// FileID không hợp lệ
				if(fileID < 0 || fileID > 9)
				{
					printf("Write: FileID phai nam trong doan [0, 9]\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// File không tồn tại
				if(fileSystem->table[fileID] == NULL && fileID > 1)
				{
					printf("Write: File khong ton tai\n");
					machine->WriteRegister(2, -1);
					break;
				}

				buffer = User2System(virtAddr, charcount);
				// Console input
				if(fileID == 0)
				{
					printf("Write: Khong the viet console input\n");
					machine->WriteRegister(2, -1);
					break;
				}
				// Console output
				else if(fileID == 1)
				{

					// Dùng hàm Write của SynchConsole để viết console input
					int n = gSynchConsole->Write(buffer, charcount);
					machine->WriteRegister(2, n);
					delete[] buffer;
					break;
				}
				// File bình thường
				else
				{
					// File chỉ đọc
					if(fileSystem->table[fileID]->type == ReadOnly)
					{
						printf("Write: Khong the viet duoc file chi doc\n");
						machine->WriteRegister(2, -1);
					}
					// File đọc và ghi
					else
					{
						int n = fileSystem->table[fileID]->Write(buffer, charcount);
						machine->WriteRegister(2, n);
						delete[] buffer;
					}
					break;
				}
			}

			// ---------------------------------------------------------------- //

			case SC_Seek:
			{
				// int Seek(int pos, OpenFileID id)
				int pos = machine->ReadRegister(4);
				int fileID = machine->ReadRegister(5);

				// pos không hợp lệ
				if(pos < 0)
				{
					printf("Seel: Pos phai lon hon 0\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// FileID không hợp lệ
				if(fileID < 0 || fileID > 9)
				{
					printf("Seek: FileID phai nam trong doan [0, 9]\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// Console
				if(fileID == 0 || fileID == 1)
				{
					printf("Seek: Khong the seek tren console\n");
					machine->WriteRegister(2, -1);
					break;
				}

				// File không tồn tại
				if(fileSystem->table[fileID] == NULL)
				{
					printf("Seek: File khong ton tai\n");
					machine->WriteRegister(2, -1);
				}
				// File tồn tại
				else
				{
					int endPos = fileSystem->table[fileID]->Length();
					// pos = -1 hoac pos > endPos thi seek den cuoi file
					if(pos == -1 || pos > endPos)
					{
						fileSystem->table[fileID]->Seek(endPos);
						machine->WriteRegister(2, endPos);
					}
					else
					{
						fileSystem->table[fileID]->Seek(pos);
						machine->WriteRegister(2, pos);
					}
				}
				break;
			}

			// ---------------------------------------------------------------- //

			case SC_Delete:
			{
				// int Delete(char *name);
				int virtAddr = machine->ReadRegister(4);
				char* name = User2System(virtAddr, MaxFileLength);
				// Kiểm tra file có đang mở không
				for(int i = 2; i < 9; i++)
				{
					if(fileSystem->table[i] != NULL)
					{
						if(strcmp(fileSystem->table[i]->filename, name) == 0)
						{
							printf("Delete: File dang mo, khong the xoa\n");
							machine->WriteRegister(2, -1);
							delete[] name;
							increasePC();
							return;
						}
					}
				}
				
				// Xóa thành công
				if(fileSystem->Remove(name) == 1)
				{
					printf("Delete: Xoa file \"%s\" thanh cong\n", name);
					machine->WriteRegister(2, 0);
				}
				// File không tồn tại
				else
				{
					printf("Delete: Khong ton tai file \"%s\"\n", name);
					machine->WriteRegister(2, -1);
				}
				delete[] name;
				break;
			}

			// ---------------------------------------------------------------- //

			default:
				break;
			// printf("Unexpected user mode exception (%d %d)", which, syscallType);
			// interrupt->Halt();
		}
		break;
	}
	increasePC();
}
	
