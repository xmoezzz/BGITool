#include <WinFile.h>

#include <string>

using std::wstring;

typedef struct
{
	ULONG   HeaderSize;
	ULONG   InstructionSize; //FileSize  - HeaderSize, including StringPool
	ULONG   Reserve[2];      //Must be zero
} BGIBpHeader;


#define BpReturn 0x00000017UL


wstring RebuildString(wstring Str)
{
	wstring Result;
	for (auto it : Str)
	{
		if (it == 0x000A)
		{
			Result += L"\\n";
		}
		else
		{
			Result += it;
		}
	}
	return Result;
}


#define True 1
#define False 0

typedef BOOL Bool;
typedef size_t SizeT;
typedef PCHAR PCChar;
typedef BYTE Byte;
typedef CHAR Char;
typedef __int32 Int32;

Bool IsShiftJISChar(SizeT uChar)
{
	Byte LowByte, HighByte;

	LowByte = LOBYTE(uChar);
	HighByte = HIBYTE(uChar);
	if ((HighByte >= 0x80 && HighByte <= 0x9F) ||
		(HighByte >= 0xE0 && HighByte <= 0xFC))
	{
		if ((LowByte >= 0x40 && LowByte <= 0x7E) ||
			(LowByte >= 0x80 && LowByte <= 0xFC))
		{
			return True;
		}
	}
	/*
	else if (HighByte >= 0xA1 && HighByte <= 0xDF)
	return True;
	*/
	return False;
}


#if !defined(BREAK_IF)
#define BREAK_IF(c) if (c) break;
#endif /* BREAK_IF */

#if !defined(CONTINUE_IF)
#define CONTINUE_IF(c) if (c) continue;
#endif /* CONTINUE_IF */

#define SWAP2(v) (USHORT)(((ULONG32)(v) << 8) | ((USHORT)(v) >> 8))
#define SWAPCHAR(v) ((ULONG32)SWAP2(v))

Bool IsShiftJISString(PCChar pString, SizeT Length)
{
	Bool   bMBChar;
	PCChar p;

	if (pString == NULL)
		return False;

	bMBChar = False;
	p = pString;
	for (; Length; ++p, --Length)
	{
		Char  c;
		Int32 ch;

		c = *p;
		CONTINUE_IF(c > 0);
		BREAK_IF(c == 0 || --Length == 0);

		ch = *(PSHORT)p++;
		if (!IsShiftJISChar(SWAPCHAR(ch)))
			return False;
		else
			bMBChar = True;
	}
	return bMBChar;
}

int wmain(int argc, WCHAR* argv[])
{
	if (argc != 2)
		return 0;

	WinFile File;
	ULONG FileSize;
	PBYTE Buffer = nullptr;
	wstring OutFileName(argv[1]);
	OutFileName += L".txt";

	if (File.Open(argv[1], WinFile::FileRead) != S_OK)
		return 0;

	FileSize = File.GetSize32();
	Buffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, FileSize);

	if (!Buffer)
	{
		File.Release();
		return 0;
	}

	File.Read(Buffer, FileSize);
	File.Release();

	BGIBpHeader* Header = (BGIBpHeader*)Buffer;

	ULONG iPos = sizeof(BGIBpHeader);
	ULONG LastReturn = 0;
	
	while (iPos < Header->InstructionSize - sizeof(DWORD))
	{
		if (*(PDWORD)(Buffer + iPos) == BpReturn)
		{
			LastReturn = iPos;
			iPos += 4;
		}
		else
		{
			iPos++;
		}
	}

	iPos = LastReturn + sizeof(DWORD);
	if (iPos == sizeof(DWORD))
	{
		//no refer
		return 0;
	}


	FILE* fout = _wfopen(OutFileName.c_str(), L"wb");
	while (iPos < Header->InstructionSize)
	{
		WCHAR WideStr[1000] = { 0 };
		CHAR  UTF8Str[3000] = { 0 };

		MultiByteToWideChar(IsShiftJISString((PCHAR)(Buffer + iPos), lstrlenA((PCHAR)(Buffer + iPos))) ?
			932 : 936, 0, (PCHAR)(Buffer + iPos), lstrlenA((PCHAR)(Buffer + iPos)), WideStr, 1000);

		wstring Result = RebuildString(wstring(WideStr));

		WideCharToMultiByte(CP_UTF8, 0, Result.c_str(), Result.length(), UTF8Str, 3000, nullptr, nullptr);

		fprintf(fout, "[0x%08x]%s\r\n", iPos, UTF8Str);
		fprintf(fout, "[0x%08x]\r\n\r\n", iPos, UTF8Str);

		iPos += lstrlenA((PCHAR)(Buffer + iPos)) + 1;
	}

	fclose(fout);
	HeapFree(GetProcessHeap(), 0, Buffer);
	return 0;
}

