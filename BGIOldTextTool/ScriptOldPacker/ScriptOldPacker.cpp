#include <Windows.h>
#include <WinFile.h>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include "Instruction.h"
#include <clocale>

using std::string;
using std::wstring;
using std::fstream;
using std::vector;


typedef struct NamePair
{
	WCHAR* CNName;
	WCHAR* JPName;
}NamePair;

static NamePair Name[] =
{
	{L"真白",  L"ましろ" },
	{L"裕理",  L"裕理"}
};


/*
None Signature Version:

BGI ByteCode
BGI treats each dword as a bytecode.
Ususally, range from 0 to 0xFF refers to basic operator, such as add.
from 0 to 0x00FFFFFF(Maybe 0x00FFFFFF is big enough to holds all extended functions for a AVG game)

For example, there are some operations as follow:
push dword 0x00000003

In This way, just search 0x00000003 and treats as a push string operator may cause some peoblems.


樱空的文本是swap过的，所以这里需要再次swap
平常按照Referred Order则不需要此步骤

PushString ("「ねえ、ましろ。次は何を買わなきゃいけないんだっけ？」")
PushString ("裕理")
*/


BOOL JPMode = TRUE;

typedef struct PackInfo
{
	ULONG  StringOffset;
	string StringInfo;     //GBK
	ULONG  ByteCodeOffset; //rewrite the next offset
	ULONG  NewStrOffset;
	PackInfo() : StringOffset(0), NewStrOffset(0){}

	PackInfo& operator=(PackInfo& lhs)
	{
		this->StringOffset = lhs.StringOffset;
		this->StringInfo = lhs.StringInfo;
		this->ByteCodeOffset = lhs.ByteCodeOffset;
		this->NewStrOffset = lhs.NewStrOffset;
		return *this;
	}
}PackInfo;

//「
static BYTE TalkName[] = { 0xE3, 0x80, 0x8C };

//Return string count
ULONG DisasmProc(PBYTE Buffer, ULONG Size, vector<PackInfo>& PackPool)
{
	ULONG Count = 0;
	ULONG iPos = 0;
	ULONG StreamSize = Size;

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

	StreamSize = LastOffset;
	while (iPos < StreamSize)
	{
		string OpString;
		if (GetInstructionInfo(Buffer, iPos, OpString) == S_OK)
		{
			PackInfo info;
			info.ByteCodeOffset = iPos - 8;
			info.StringOffset = *(PDWORD)(Buffer + iPos - 4);
			info.StringInfo = "";

			PackPool.push_back(info);
			Count++;
		}
	}
	return Count++;
}

//Instruction Order
int wmain(int argc, WCHAR* argv[])
{
	if (argc != 2)
		return 0;

	setlocale(LC_ALL, "chs");
	vector<PackInfo> PackPool;

	char szPath[260] = { 0 };
	WideCharToMultiByte(CP_ACP, 0, argv[1], lstrlenW(argv[1]), szPath, 260, nullptr, nullptr);
	lstrcatA(szPath, ".txt");

	fstream fin(szPath);
	if (!fin)
	{
		MessageBoxW(NULL, L"不能打开输入", L"Error", MB_OK);
		return -1;
	}

	printf("Input ok\n");
	vector<string> RawText;
	string ReadLine;
	while (getline(fin, ReadLine))
	{
		if (ReadLine.length())
		{
			RawText.push_back(ReadLine);
			ReadLine.clear();
		}
	}
	fin.close();

	/*******************************************/
	FILE* BinFile = nullptr;
	ULONG BinSize = 0;
	PBYTE BinBuffer = nullptr;

	BinFile = _wfopen(argv[1], L"rb");
	if (BinFile == nullptr)
	{
		MessageBoxW(NULL, L"无法打开脚本文件", nullptr, MB_OK);
		return 0;
	}
	fseek(BinFile, 0, SEEK_END);
	BinSize = ftell(BinFile);
	rewind(BinFile);
	BinBuffer = new BYTE[BinSize];
	fread(BinBuffer, 1, BinSize, BinFile);
	fclose(BinFile);

	ULONG OriCount = DisasmProc(BinBuffer, BinSize, PackPool);

	if (OriCount != (ULONG)RawText.size())
	{
		MessageBoxW(NULL, L"The count of text is different!!", 0, 0);
		return 0;
	}

	vector<string> TextPool;

	for (int i = 0; i < RawText.size(); i++)
	{
		size_t Pos = RawText[i].find_first_of(">");
		if (Pos == string::npos)
		{
			WCHAR Info[1000] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, RawText[i].c_str(), RawText[i].length(), Info, 1000);
			wprintf(L"Text Anth Error : %s\n", Info);
			getchar();
			return 0;
		}
		string UTF8Str = RawText[i].substr(Pos + 1, RawText[i].length());

		WCHAR WideStr[2000] = { 0 };
		CHAR  GBKString[2000] = { 0 };

		BOOL Ret;
		MultiByteToWideChar(CP_UTF8, 0, UTF8Str.c_str(), UTF8Str.length(), WideStr, 2000);
		
		ULONG CodePage = 936;

#if 0
		for (ULONG Index = 0; Index < _countof(Name); Index++)
		{
			if (!lstrcmpW(WideStr, Name[Index].CNName))
			{
				wmemset(WideStr, 0, _countof(WideStr));
				lstrcpyW(WideStr, Name[Index].JPName);
				CodePage = 932;
				break;
			}
		}
#endif

		WideCharToMultiByte(CodePage, 0, WideStr, lstrlenW(WideStr), GBKString, 2000, nullptr, &Ret);
		if (Ret)
		{
			wprintf(L"Lost Char : %s\n", WideStr);
			if (i > 0)
			{
				WCHAR WideStr2[2000] = { 0 };
				MultiByteToWideChar(CP_UTF8, 0, RawText[i - 1].c_str(), 
					RawText[i - 1].length(), WideStr2, 2000);
				wprintf(L"Lost Char : %s\n", WideStr2);
			}
			getchar();
		}

		//swap again...
		if (!memcmp(UTF8Str.c_str(), TalkName, 3))
		{
			//ingored some problems, such as i is equal to 0
			PackPool[i].StringInfo = PackPool[i - 1].StringInfo;
			PackPool[i - 1].StringInfo = GBKString;
		}
		else
		{
			PackPool[i].StringInfo = GBKString;
		}
	}

	//开始计算新的Offset
	ULONG iPos = 0;

	ULONG Finder = iPos;
	ULONG LastOffset = BinSize;
	while (Finder < BinSize)
	{
		if (*(PULONG)(BinBuffer + Finder) == 0x0000001B)
		{
			LastOffset = Finder + 4;
		}
		Finder += 4;
	}

	ULONG FinderAll = iPos;
	ULONG MinAddr = 0x7FFFFFFFUL;
	string OpString;
	while (iPos < BinSize)
	{
		ULONG Ins = *(PULONG)(BinBuffer + iPos);
		if (Ins & 0xFFF00000)
		{
			break;
		}
		GetInstructionInfo(BinBuffer, iPos, OpString, MinAddr);
	}

	iPos = 0;
	ULONG StreamSize = MinAddr;
	vector<BYTE> StringPool;
	ULONG StrOffset = StreamSize;

	for (ULONG i = 0; i < PackPool.size(); i++)
	{
		for (ULONG j = 0; j < PackPool[i].StringInfo.length(); j++)
		{
			StringPool.push_back((BYTE)PackPool[i].StringInfo[j]);
		}
		StringPool.push_back(0);

		PackPool[i].NewStrOffset = StrOffset;
		StrOffset += PackPool[i].StringInfo.length() + 1;
	}

	iPos = 0;
	//开始重新写入Offset
	ULONG Index = 0;
	ULONG Dummy;
	while (iPos < StreamSize)
	{
		ULONG Ins = *(PULONG)(BinBuffer + iPos);
		//Push String
		if (Ins == 0x00000003UL)
		{
			ULONG Offset = *(PULONG)(BinBuffer + iPos + 4);
			memcpy((BinBuffer + iPos + 4), &(PackPool[Index].NewStrOffset), sizeof(DWORD));
			Index++;
		}
		GetInstructionInfo(BinBuffer, iPos, OpString, Dummy);
	}


	FILE* BinOut = _wfopen((wstring(argv[1]) + L".out").c_str(), L"wb");
	fwrite(BinBuffer, 1, StreamSize, BinOut);
	fwrite(&(StringPool[0]), 1, StringPool.size(), BinOut);

	fclose(BinOut);
	delete[] BinBuffer;
	return 0;
}

