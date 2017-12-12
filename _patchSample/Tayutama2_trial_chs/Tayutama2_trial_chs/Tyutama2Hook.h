#include <Windows.h>
#include "MyNative.h"
#include "detours.h"
#include "BGIraw.h"
#include "lz4.h"
#include <string>
#include <map>
#include "WinFile.h"
#include "Hash64.h"

#define BaseAppName L"[樱空汉化组]花色七芒星"

using std::wstring;
using std::map;
using std::pair;

#define CP_GB2312   936
#define CP_SHIFTJIS 932

#define MY_BURIKO_SCRIPT_MAGIC "STmoeSTmoeChu>_<" //@16


#pragma pack(push, 1)
typedef struct HeaderInfo
{
	ULONG Magic;
	ULONG Size;
	ULONG Count;
	ULONG Key;
}HeaderInfo;

typedef struct ChunkInfo
{
	ULONG   Magic;
	ULONG   Size;
	ULONG   Offset;
	ULONG64 HashName;
	WCHAR   lpFileName[MAX_PATH];
}ChunkInfo;
#pragma pack(pop)

typedef struct ChunkAtom
{
	ULONG   Size;
	ULONG   Offset;
	WCHAR   lpFileName[MAX_PATH];

	ChunkAtom& operator = (ChunkAtom& rhs)
	{
		this->Size = rhs.Size;
		this->Offset = rhs.Offset;
		RtlCopyMemory(this->lpFileName, rhs.lpFileName, MAX_PATH);
		return *this;
	}
}ChunkAtom;

typedef LONG(CDECL* StubDecompressFile)(PVOID  pvDecompressed,
	PULONG pOutSize,
	PVOID  pvCompressed,
	ULONG  InSize,
	ULONG  SkipBytes,
	ULONG  OutBytes);


typedef LONG(CDECL* StubLoadFile)(PVOID  pvDecompressed,
	PULONG pOutSize,
	LPCSTR lpFileName,
	ULONG  SkipBytes,
	ULONG  OutBytes);

class HanairoHook
{
	HanairoHook();
	static HanairoHook* Handle;

public:
	static HanairoHook* GetGlobalData();

	~HanairoHook();

	HRESULT WINAPI Init(HMODULE hSelf);
	HRESULT WINAPI UnInit(HMODULE hSelf);

	HFONT WINAPI DuplicateFontW(HDC hdc, UINT LangId);
	ULONG WINAPI AnsiToUnicode(LPCSTR lpAnsi, 
							   ULONG  Length, 
							   LPWSTR lpUnicodeBuffer, 
							   ULONG  BufferCount, 
							   ULONG  CodePage = CP_ACP);

	//@Once
	HRESULT WINAPI SetAppName();
	HRESULT WINAPI GetAppName(wstring& Name);
	wstring WINAPI GetPackageName(wstring& fileName);

	static UINT WINAPI ExitMessage(const WCHAR* Info  = L"遇到致命内部错误\n游戏将会立即退出",
								   const WCHAR* Title = L"花色七芒星");

	HRESULT WINAPI LoadFileBuffer(LPCSTR lpFileName, PBYTE& Buffer, ULONG& OutSize);
	HRESULT WINAPI LoadFileSystem();

	HFONT WINAPI GetDefFont(){ return DefFont; }

	static BOOL WINAPI WriteInfo(const WCHAR* lpInfo, BOOL ForRelease = FALSE);
	static BOOL WINAPI WriteInfo(const CHAR*  lpInfo, BOOL ForRelease = FALSE);

	static PVOID OldDecompressFile;
	static PVOID OldCheckOSDefaultLangID;
	static PVOID OldLoadFileImm;
	static PVOID OldCheckFont1;
	static PVOID OldCheckFont2;

	HWND MainWin;

private:

	WinFile SystemFile;
	HMODULE hSelfModule;
	wstring AppName;
	BOOL    InitFileSystem;
	PROC    pfTextOutA;
	PROC    pfTextOutW;
	PROC    pfCreateFontA;
	PROC    pfGetOEMCP;
	PROC    pfGetACP;
	PROC    pfCreateWindowExA;
	PROC    pfSetWindowTextA;
	PROC    pfCreateFileA;
	PROC    pfMessageBoxA;
	PROC    pfMultiByteToWideChar;

	HFONT   DefFont;
	ChunkInfo* IndexBuffer;
	map<ULONG64, ChunkAtom> ChunkList;
};

