#include <WinFile.h>
#include <string>
#include "ScriptHeader.h"
#include "Instruction.h"

using std::string;
using std::wstring;

#define POST_NAME L"_BinOrder.txt"

BOOL JPMode = TRUE;
HRESULT DisasmProc(wstring& FileName, PBYTE Buffer, ULONG Size);

int wmain(int argc, WCHAR* argv[])
{
	if (argc != 2 && argc != 3)
	{
		printf("exe file [mode]\n");
		getchar();
		return 0;
	}

	if (argc == 3)
	{
		JPMode = FALSE;
	}

	if (wcsstr(argv[1], L".txt") || wcsstr(argv[1], L".exe") || wcsstr(argv[1], L".bat"))
	{
		//·ÀÖ¹ÍæÎÚÁú233
		return 0;
	}

	WinFile File;
	if (FAILED(File.Open(argv[1], WinFile::FileRead)))
	{
		return 0;
	}

	PBYTE Buffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, File.GetSize32());
	File.Read(Buffer, File.GetSize32());

	static const CHAR Mask[] = "BurikoCompiledScriptVer1.00";

	if (memcmp(Buffer, Mask, sizeof(Mask)))
		return 0;

	HRESULT Result = DisasmProc((wstring(argv[1]) + POST_NAME), Buffer, File.GetSize32());

	HeapFree(GetProcessHeap(), 0, Buffer);
	File.Release();
	return 0;
}

wstring FixStringW(wstring& Line)
{
	wstring Name;
	for (auto it : Line)
	{
		if (it == 0x000A)
		{
			Name += L"\\n";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

#include "my.h"

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

		ch = *(PUInt16)p++;
		if (!IsShiftJISChar(SWAPCHAR(ch)))
			return False;
		else
			bMBChar = True;
	}

	return bMBChar;
}

HRESULT DisasmProc(wstring& FileName, PBYTE Buffer, ULONG Size)
{
	WinFile File;
	if (File.Open(FileName.c_str(), WinFile::FileWrite) != S_OK)
	{
		printf("Failed to open\n");
		getchar();
		return E_FAIL;
	}

	ScriptHeader Header = { 0 };
	ULONG iPos = 0;
	ULONG StreamSize = Size;

	memcpy(&Header, Buffer, sizeof(ScriptHeader));
	iPos = sizeof(ScriptHeader) - sizeof(ULONG);
	iPos += Header.HeaderSize;

	ULONG Finder = iPos;
	ULONG LastOffset = Size;
	while (Finder < Size)
	{
		if (*(PULONG)(Buffer + Finder) == 0x0000001B)
		{
			LastOffset = Finder + 4;
		}
		Finder += 4;
	}

	ULONG FinderAll = iPos;
	ULONG MinAddr = 0x7FFFFFFFUL;
	string OpString;
	while (iPos < Size)
	{
		ULONG Ins = *(PULONG)(Buffer + iPos);
		if (Ins & 0xFFF00000)
		{
			break;
		}
		GetInstructionInfo(Buffer, iPos, OpString, MinAddr);
	}

	ScriptHeader* pHeader = (ScriptHeader*)Buffer;
	ULONG HeaderSize = pHeader->HeaderSize - sizeof(ULONG) + sizeof(ScriptHeader);

	iPos = HeaderSize;
	//StreamSize = max(LastOffset, MinAddr);
	StreamSize = MinAddr;
	iPos += StreamSize;

	PBYTE BlockPtr = (Buffer + iPos);
	while (iPos < Size)
	{
		string JPString;
		CHAR   UTF8String[2000] = {0};
		WCHAR  WideString[2000] = { 0 };
		CHAR PrefixLineNo[100] = { 0 };
		wsprintfA(PrefixLineNo, "[0x%08x]", iPos);

		JPString = (PCHAR)(Buffer + iPos);

		if (!JPMode)
			JPMode = IsShiftJISString(JPString.c_str(), JPString.length());

		MultiByteToWideChar(JPMode ? 932 : 936, 0, JPString.c_str(), JPString.length(), WideString, 1500);

		wstring WideFixedStr = FixStringW(wstring(WideString));
		WideCharToMultiByte(CP_UTF8, 0, WideFixedStr.c_str(), WideFixedStr.length(), UTF8String, 2000, nullptr, nullptr);

		File.Write((PBYTE)PrefixLineNo, lstrlenA(PrefixLineNo));
		File.Write((PBYTE)UTF8String, lstrlenA(UTF8String));
		File.Write((PBYTE)"\r\n", 2);
		File.Write((PBYTE)";", 1);
		File.Write((PBYTE)PrefixLineNo, lstrlenA(PrefixLineNo));

		File.Write((PBYTE)UTF8String, lstrlenA(UTF8String));

		File.Write((PBYTE)"\r\n\r\n", 4);

		ULONG Len = lstrlenA((PCHAR)BlockPtr) + 1;
		BlockPtr += Len;
		iPos += Len;
	}
	File.Release();
	return S_OK;
}
