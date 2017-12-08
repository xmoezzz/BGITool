#include <Windows.h>
#include "Instruction.h"
#include "WinFile.h"
#include <string>
#include "ScriptHeader.h"

using std::wstring;

BOOL JPMode = FALSE;


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
		JPMode = TRUE;
	}

	if (wcsstr(argv[1], L".txt"))
	{
		//防止玩乌龙233
		return 0;
	}

	WinFile File;
	if (FAILED(File.Open(argv[1], WinFile::FileRead)))
	{
		return 0;
	}

	PBYTE Buffer = (PBYTE)HeapAlloc(GetProcessHeap(), 0, File.GetSize32());
	File.Read(Buffer, File.GetSize32());

	HRESULT Result = DisasmProc((wstring(argv[1]) + L".txt"), Buffer, File.GetSize32());
	
	HeapFree(GetProcessHeap(), 0, Buffer);
	File.Release();
	return 0;
}


//没有记录字节码长度，长度是保存在VM里的= =
//目前几种扫描字节码长度的方式：
//1.VM字节码开始地方有一个运行时的记录
//2.扫描Return(一个文件内可能有多个Return), 扫描最后一个DWORD对齐的Return
//字节码最后一句一定是Return 接下来就是String Pool 嗯，幸好没有sort

//综上，使用方案2
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
	
	StreamSize = LastOffset;
	while (iPos < StreamSize)
	{
		string OpString;
		GetInstructionInfo(Buffer, iPos, OpString);
		File.Write((PBYTE)OpString.c_str(), OpString.length());
		File.Write((PBYTE)"\r\n", 2);
	}

	File.Release();
	return S_OK;
}

