#ifndef _BGI_Instruction_
#define _BGI_Instruction_

#include <Windows.h>
#include <string>
#include <vector>


using std::string;
using std::vector;


typedef string(WINAPI* ArgsStub)(PBYTE BufferStart, ULONG& Offset);

typedef struct BGIInsRecord
{
	ULONG OPCode;
	const char Name[100];
	ULONG ArgsCount;

	ArgsStub Stub[0x10];
}*pBGIInsRecord;


#define IsByteCode(x) ((ULONG)x & 0xFFFFFF00UL) == 0

HRESULT GetInstructionInfo(PBYTE BufferStart, ULONG& iPos, string& Name, ULONG& MinAddr);


//private:
string WINAPI GetString(PBYTE BufferStart, ULONG& Offset);
string WINAPI GetOffset(PBYTE BufferStart, ULONG& Offset);
string WINAPI GetDword(PBYTE BufferStart, ULONG& Offset);

#endif
