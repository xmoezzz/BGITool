#include "Tayutama2Hook.h"
#include "ImageEx.h"
#include "Compressor.h"
#include "TTFData.h"
#include "TTFStaticRelease.h"

//Allocator
//0x00488A7B


PVOID CDECL HostAlloc(LONG Size)
{
	return HeapAlloc(GetProcessHeap(), 0, Size);
}

VOID CDECL HostFree(PVOID lpMem)
{
	if (!lpMem)
		return;

	__try
	{
		HeapFree(GetProcessHeap(), 0, lpMem);
	}
	__except (EXCEPTION_CONTINUE_EXECUTION)
	{
	}
}
//==========================
//sub_46F6F0
BOOL CDECL CheckOSDefaultLangID(ULONG PrimaryLangID)
{
	UNREFERENCED_PARAMETER(PrimaryLangID);
	return TRUE;
}

BOOL WINAPI HookTextOutA(HDC hDC, int X, int Y, LPCSTR lpString, int cbCount)
{
	BOOL    Result;
	WCHAR   c, ch[0x400];
	ULONG   CodePage;
	HFONT   hFont, hFontOld;

	hFont = NULL;
	hFontOld = NULL;

	CodePage = CP_GB2312;
	if (cbCount > 2)
	{
		if (IsShiftJISString((PCHAR)lpString, cbCount))
			CodePage = CP_SHIFTJIS;

		goto MBYTE_TO_WCHAR;
	}
	else if (cbCount == 2)
	{
		c = *(PWCHAR)lpString;
		switch ((ULONG)c)
		{
		case SWAPCHAR(L'⑨'):
			ch[0] = 0x266A;
			break;

		default:
			//                if (IsShiftJISString((LPSTR)&c, 2))
			//                    CodePage = CP_SHIFTJIS;

			goto MBYTE_TO_WCHAR;
		}

		hFont = Tayutama2Hook::GetGlobalData()->DuplicateFontW(hDC, SHIFTJIS_CHARSET);
		if (hFont != NULL)
		{
			hFontOld = (HFONT)SelectObject(hDC, hFont);
			if (hFontOld == NULL)
			{
				DeleteObject(hFont);
				hFont = NULL;
			}
		}
		cbCount = 1;
		goto SKIP_CONV;
	}

MBYTE_TO_WCHAR:
	cbCount = MultiByteToWideChar(CodePage, 0, lpString, cbCount, ch, _countof(ch));

SKIP_CONV:

	Tayutama2Hook::GetGlobalData()->WriteInfo(ch);
	Result = TextOutW(hDC, X, Y, ch, cbCount);

	if (hFont != NULL)
	{
		SelectObject(hDC, hFontOld);
		DeleteObject(hFont);
	}

	return Result;
}


BOOL WINAPI HookTextOutW(
	_In_ HDC     hdc,
	_In_ int     nXStart,
	_In_ int     nYStart,
	_In_ LPWSTR  lpString,
	_In_ int     cchString
	)
{
	//Tayutama2Hook::GetGlobalData()->WriteInfo(lpString);
	BOOL Result;

	if (cchString == 1)
	{
		if (lpString[0] == (WORD)0x2468)
		{
			lpString[0] = (WORD)0x266A;
			LOGFONTW* lplf = (LOGFONTW*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOGFONTW));

			HFONT hOldFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
			GetObject(hOldFont, sizeof(LOGFONTW), lplf);
			LOGFONTW Info = { 0 };

			Info.lfHeight = lplf->lfHeight;
			Info.lfWidth = lplf->lfWidth;
			Info.lfEscapement = lplf->lfEscapement;
			Info.lfOrientation = lplf->lfOrientation;
			Info.lfWeight = lplf->lfWeight;
			Info.lfItalic = lplf->lfItalic;
			Info.lfUnderline = lplf->lfUnderline;
			Info.lfStrikeOut = lplf->lfStrikeOut;
			Info.lfOutPrecision = lplf->lfOutPrecision;
			Info.lfClipPrecision = lplf->lfClipPrecision;
			Info.lfQuality = lplf->lfQuality;
			Info.lfPitchAndFamily = lplf->lfPitchAndFamily;
			lstrcpyW(Info.lfFaceName, L"MS Gothic");
			lplf->lfCharSet = SHIFTJIS_CHARSET;

			HFONT hFont = CreateFontIndirectW(&Info);

			hOldFont = (HFONT)SelectObject(hdc, hFont);
			//6A 26 
			Result = TextOutW(hdc, nXStart, nYStart, lpString, cchString);

			SelectObject(hdc, hOldFont);
			DeleteObject(hFont);
			HeapFree(GetProcessHeap(), 0, lplf);
			return Result;
		}
	}
	return TextOutW(hdc, nXStart, nYStart, lpString, cchString);
}


HFONT WINAPI HookCreateFontA(
int     cHeight,
int     cWidth,
int     cEscapement,
int     cOrientation,
int     cWeight,
DWORD   bItalic,
DWORD   bUnderline,
DWORD   bStrikeOut,
DWORD   iCharSet,
DWORD   iOutPrecision,
DWORD   iClipPrecision,
DWORD   iQuality,
DWORD   iPitchAndFamily,
LPCSTR  pszFaceName
)
{
	LOGFONTW lf = { 0 };

	UNREFERENCED_PARAMETER(iCharSet);
	UNREFERENCED_PARAMETER(iQuality);

	if (Tayutama2Hook::GetGlobalData()->GetDefFont())
	{
		lf.lfHeight = cHeight;
		lf.lfWidth = cWidth;
		lf.lfEscapement = cEscapement;
		lf.lfOrientation = cOrientation;
		lf.lfWeight = cWeight;
		lf.lfItalic = bItalic;
		lf.lfUnderline = bUnderline;
		lf.lfStrikeOut = bStrikeOut;
		lf.lfCharSet = GB2312_CHARSET;
		lf.lfOutPrecision = 0;
		lf.lfClipPrecision = iClipPrecision;
		lf.lfQuality = iQuality;
		lf.lfPitchAndFamily = iPitchAndFamily;

		//DFPYuanW5-GB
		lstrcpyW(lf.lfFaceName, L"华康圆体W5(P)");

		return CreateFontIndirectW(&lf);
	}
	else
	{
		lf.lfHeight = cHeight;
		lf.lfWidth = cWidth;
		lf.lfEscapement = cEscapement;
		lf.lfOrientation = cOrientation;
		lf.lfWeight = cWeight;
		lf.lfItalic = bItalic;
		lf.lfUnderline = bUnderline;
		lf.lfStrikeOut = bStrikeOut;
		lf.lfCharSet = GB2312_CHARSET;
		lf.lfOutPrecision = iOutPrecision;
		lf.lfClipPrecision = iClipPrecision;
		lf.lfQuality = CLEARTYPE_QUALITY;
		lf.lfPitchAndFamily = iPitchAndFamily;

#if 0
		Tayutama2Hook::GetGlobalData()->AnsiToUnicode(
			pszFaceName, 
			-1, 
			lf.lfFaceName, 
			_countof(lf.lfFaceName), 
			IsShiftJISString(pszFaceName, -1) ? CP_SHIFTJIS : CP_GB2312);
#else
		lstrcpyW(lf.lfFaceName, L"黑体");
#endif
		return CreateFontIndirectW(&lf);
	}
}

/*
List of windows, item 4
Handle = 0032080E
Text = 壴怓僿僾僞僌儔儉
Parent = Topmost
WinProc =
ID/menu =
Type = ASCII
Style = 94CA0000 WS_POPUP|WS_MINIMIZEBOX|WS_CAPTION|WS_SYSMENU|WS_VISIBLE|WS_CLIPSIBLINGS
ExtStyle = 00040100 WS_EX_WINDOWEDGE|WS_EX_APPWINDOW
Thread = Main
ClsProc = 00478330
ClsName = BGI - Main window
*/

HWND WINAPI HookCreateWindowExA(
	_In_     DWORD     dwExStyle,
	_In_opt_ LPCSTR   lpClassName,
	_In_opt_ LPCSTR   lpWindowName,
	_In_     DWORD     dwStyle,
	_In_     int       x,
	_In_     int       y,
	_In_     int       nWidth,
	_In_     int       nHeight,
	_In_opt_ HWND      hWndParent,
	_In_opt_ HMENU     hMenu,
	_In_opt_ HINSTANCE hInstance,
	_In_opt_ LPVOID    lpParam
	)
{
	HWND Result = nullptr;
	if (!lstrcmpA(lpClassName, "BGI - Main window"))
	{
		Result = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
		Tayutama2Hook::GetGlobalData()->MainWin = Result;
	}
	else
	{
		Result = CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, x, y,
			nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);
	}
	return Result;
}

BOOL WINAPI HookSetWindowTextA(
	_In_     HWND    hWnd,
	_In_opt_ LPCSTR lpString
	)
{
	if (Tayutama2Hook::GetGlobalData()->MainWin == hWnd)
	{
		wstring QueryInfo;
		Tayutama2Hook::GetGlobalData()->GetAppName(QueryInfo);
		return SetWindowTextW(hWnd, QueryInfo.c_str());
	}
	return SetWindowTextA(hWnd, lpString);
}



wstring WINAPI Tayutama2Hook::GetPackageName(wstring& fileName)
{
	wstring temp(fileName);
	wstring::size_type pos = temp.find_last_of(L"\\");

	if (pos != wstring::npos)
	{
		temp = temp.substr(pos + 1, temp.length());
	}

	wstring temp2(temp);
	wstring::size_type pos2 = temp2.find_last_of(L"/");
	if (pos2 != wstring::npos)
	{
		temp2 = temp2.substr(pos + 1, temp2.length());
	}
	return temp2;
}


HANDLE
WINAPI
HookCreateFileA(
LPCSTR                  lpFileName,
DWORD                   dwDesiredAccess,
DWORD                   dwShareMode,
LPSECURITY_ATTRIBUTES   lpSecurityAttributes,
DWORD                   dwCreationDisposition,
DWORD                   dwFlagsAndAttributes,
HANDLE                  hTemplateFile
)
{
	WCHAR   szFile[MAX_PATH] = {0};
	ULONG   Length;

	Length = Tayutama2Hook::GetGlobalData()->AnsiToUnicode(lpFileName, lstrlenA(lpFileName), szFile, countof(szFile));
	
	return CreateFileW(szFile, dwDesiredAccess, dwShareMode, lpSecurityAttributes,
			dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


int WINAPI HookMessageBoxA(
	_In_opt_ HWND    hWnd,
	_In_opt_ LPCSTR lpText,
	_In_opt_ LPCSTR lpCaption,
	_In_     UINT    uType
	)
{
	PWCHAR WideText    = nullptr;
	PWCHAR WideCaption = nullptr;
	int    Result = 0;
	ULONG TextAllocSize = 0, CaptionAllocSize = 0;

	TextAllocSize = (lstrlenA(lpText) + 1) * 2;
	CaptionAllocSize = (lstrlenA(lpCaption) + 1) * 2;

	WideText    = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, TextAllocSize);
	WideCaption = (PWCHAR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, CaptionAllocSize);

	if (WideText && WideCaption)
	{
		Tayutama2Hook::GetGlobalData()->AnsiToUnicode(lpText,    lstrlenA(lpText),
			WideText, TextAllocSize,       IsShiftJISString(lpText,    lstrlenA(lpText))    ? 932 : 936);
		Tayutama2Hook::GetGlobalData()->AnsiToUnicode(lpCaption, lstrlenA(lpCaption),
			WideCaption, CaptionAllocSize, IsShiftJISString(lpCaption, lstrlenA(lpCaption)) ? 932 : 936);

		Result = MessageBoxW(hWnd, WideText, WideCaption, uType);
	}
	else
	{
		TextAllocSize = MAX_PATH * 2;
		CaptionAllocSize = MAX_PATH * 2;
		
		WCHAR StackText[MAX_PATH * 2] = { 0 };
		WCHAR StackCaption[MAX_PATH * 2] = { 0 };

		Tayutama2Hook::GetGlobalData()->AnsiToUnicode(lpText,    lstrlenA(lpText),
			StackText,    TextAllocSize,    IsShiftJISString(lpText,    lstrlenA(lpText))    ? 932 : 936);
		Tayutama2Hook::GetGlobalData()->AnsiToUnicode(lpCaption, lstrlenA(lpCaption),
			StackCaption, CaptionAllocSize, IsShiftJISString(lpCaption, lstrlenA(lpCaption)) ? 932 : 936);

		Result = MessageBoxW(hWnd, StackText, StackCaption, uType);
	}
	
	if (WideText)
		HeapFree(GetProcessHeap(), 0, WideText);
	if (WideCaption)
		HeapFree(GetProcessHeap(), 0, WideCaption);

	return Result;
}

int WINAPI HookMultiByteToWideChar(
	_In_      UINT   CodePage,
	_In_      DWORD  dwFlags,
	_In_      LPCSTR lpMultiByteStr,
	_In_      int    cbMultiByte,
	_Out_opt_ LPWSTR lpWideCharStr,
	_In_      int    cchWideChar
	)
{
	if (CodePage == 932)
	{
		CodePage = 936;
	}

	return MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar);
}


//==DecompressInfo
//sub_465320
//Checked
LONG CDECL HookDecompressFile(
	PVOID  pvDecompressed, //64M 预先分配的
	PULONG pOutSize,
	PVOID  pvCompressed,
	ULONG  InSize,
	ULONG  SkipBytes,
	ULONG  OutBytes
)
{
	ULONG Result, Magic;

	Result = 5;
	LOOP_ONCE
	{
		Magic = *(PULONG)pvCompressed;
		if ((Magic & 0x00FFFFFF) == TAG3('UCI'))
		{
			UCI_INFO uci;
			UCI_META_INFO *pMeta = NULL;
			IMG_BITMAP_HEADER *pHeader;

			if (UCIDecodeEx(pvCompressed, InSize, &uci, TRUE) < 0)
				break;

			BGIBitmap bmp = { 0 };
			bmp.Width = uci.Width;
			bmp.Height = uci.Height;

			pHeader = (IMG_BITMAP_HEADER *)pvDecompressed;
			pMeta = (UCI_META_INFO *)uci.ExtraInfo;
			if (pMeta != NULL && pMeta->Magic == UCI_META_INFO_MAGIC)
				ConvertRawToBitMap(&uci, pMeta->Width, pMeta->Height, pMeta->BitsPerPixel, pvDecompressed, -1);
			else
				ConvertRawToBitMap(&uci, uci.Width, uci.Height, uci.BitsPerPixel, pvDecompressed, -1);

			UCIFreeEx(&uci);

			if (pOutSize != NULL)
				*pOutSize = pHeader->dwFileSize;
		}
		else if (Magic == TAG4('XMOE'))
		{
			XmoeImage* Header = (XmoeImage*)pvCompressed;
			PBYTE Data = (PBYTE)pvCompressed + sizeof(XmoeImage);

			LZ4_uncompress((char*)Data, (char*)pvDecompressed, Header->RawSize);
			if (pOutSize != NULL)
				*pOutSize = Header->RawSize;
		}
		else if (InSize > sizeof(MY_BURIKO_SCRIPT_MAGIC) &&
			sizeof(MY_BURIKO_SCRIPT_MAGIC) == 
			RtlCompareMemory(pvCompressed, MY_BURIKO_SCRIPT_MAGIC, sizeof(MY_BURIKO_SCRIPT_MAGIC)))
		{
			UINT OutSize;

			OutSize = UCL_NRV2E_DecompressASMFast32((PBYTE)pvCompressed + sizeof(MY_BURIKO_SCRIPT_MAGIC), pvDecompressed);
			if (pOutSize != NULL)
				*pOutSize = OutSize;
		}
		else
		{
			break;
		}

		Result = 0;
	}

	//尝试原始解析
	return Result == 0 ? Result : 
	((StubDecompressFile)Tayutama2Hook::OldDecompressFile)(pvDecompressed, pOutSize, pvCompressed, InSize, SkipBytes, OutBytes);
}


LONG CDECL HookDecompressFile_FASTCALL(
	PVOID  pvCompressed,
	PULONG pOutSize,
	ULONG  InSize,
	ULONG  SkipBytes,
	ULONG  OutBytes
	)
{
	ULONG Result, Magic;
	PVOID  pvDecompressed;

	__asm mov pvDecompressed, ecx;

	Result = 5;
	LOOP_ONCE
	{
		Magic = *(PULONG)pvCompressed;
		if ((Magic & 0x00FFFFFF) == TAG3('UCI'))
		{
			UCI_INFO uci;
			UCI_META_INFO *pMeta;
			IMG_BITMAP_HEADER *pHeader;

			if (UCIDecodeEx(pvCompressed, InSize, &uci, TRUE) < 0)
				break;

			BGIBitmap bmp = { 0 };
			bmp.Width = pMeta->Width;
			bmp.Height = pMeta->Height;

			pHeader = (IMG_BITMAP_HEADER *)pvDecompressed;
			pMeta = (UCI_META_INFO *)uci.ExtraInfo;
			if (pMeta != NULL && pMeta->Magic == UCI_META_INFO_MAGIC)
				ConvertRawToBitMap(&uci, pMeta->Width, pMeta->Height, pMeta->BitsPerPixel, pvDecompressed, -1);
			else
				ConvertRawToBitMap(&uci, uci.Width, uci.Height, uci.BitsPerPixel, pvDecompressed, -1);

			UCIFreeEx(&uci);

			if (pOutSize != NULL)
				*pOutSize = pHeader->dwFileSize;
		}
		else if (Magic == TAG4('XMOE'))
		{
			XmoeImage* Header = (XmoeImage*)pvCompressed;
			PBYTE Data = (PBYTE)pvCompressed + sizeof(XmoeImage);

			LZ4_uncompress((char*)Data, (char*)pvDecompressed, Header->RawSize);
			if (pOutSize != NULL)
				*pOutSize = Header->RawSize;
		}
		else if (InSize > sizeof(MY_BURIKO_SCRIPT_MAGIC) &&
			sizeof(MY_BURIKO_SCRIPT_MAGIC) ==
			RtlCompareMemory(pvCompressed, MY_BURIKO_SCRIPT_MAGIC, sizeof(MY_BURIKO_SCRIPT_MAGIC)))
		{
			UINT OutSize;

			OutSize = UCL_NRV2E_DecompressASMFast32((PBYTE)pvCompressed + sizeof(MY_BURIKO_SCRIPT_MAGIC), pvDecompressed);
			if (pOutSize != NULL)
				*pOutSize = OutSize;
		}
		else
		{
			break;
		}

		Result = 0;
	}

		if (Result != 0)
		{
			__asm
			{
				push 0
					push 0
					push InSize
					push pOutSize
					push pvCompressed
					mov ecx, pvDecompressed
					call Tayutama2Hook::OldDecompressFile
					add esp, 14h
					mov Result, eax
			}
		}
	return Result;
}

LONG CDECL HookDecompressFile_USERDATA(
	PVOID  pvDecompressed,
	PVOID  pvCompressed,
	PULONG pOutSize,
	ULONG  InSize,
	ULONG  SkipBytes,
	ULONG  OutBytes
	)
{
	ULONG Result, Magic;

	Result = 5;
	LOOP_ONCE
	{
		Magic = *(PULONG)pvCompressed;
		if ((Magic & 0x00FFFFFF) == TAG3('UCI'))
		{
			UCI_INFO uci;
			UCI_META_INFO *pMeta;
			IMG_BITMAP_HEADER *pHeader;

			if (UCIDecodeEx(pvCompressed, InSize, &uci, TRUE) < 0)
				break;

			BGIBitmap bmp = { 0 };
			
			bmp.Width = uci.Height;
			bmp.Height = uci.Width;

			pHeader = (IMG_BITMAP_HEADER *)pvDecompressed;
			pMeta = (UCI_META_INFO *)uci.ExtraInfo;
			if (pMeta != NULL && pMeta->Magic == UCI_META_INFO_MAGIC)
				ConvertRawToBitMap(&uci, pMeta->Width, pMeta->Height, pMeta->BitsPerPixel, pvDecompressed, -1);
			else
				ConvertRawToBitMap(&uci, uci.Width, uci.Height, uci.BitsPerPixel, pvDecompressed, -1);

			UCIFreeEx(&uci);

			if (pOutSize != NULL)
				*pOutSize = pHeader->dwFileSize;
		}
		else if (Magic == TAG4('XMOE'))
		{
			XmoeImage* Header = (XmoeImage*)pvCompressed;
			PBYTE Data = (PBYTE)pvCompressed + sizeof(XmoeImage);

			LZ4_uncompress((char*)Data, (char*)pvDecompressed, Header->RawSize);
			if (pOutSize != NULL)
				*pOutSize = Header->RawSize;
		}
		else if (InSize > sizeof(MY_BURIKO_SCRIPT_MAGIC) &&
			sizeof(MY_BURIKO_SCRIPT_MAGIC) ==
			RtlCompareMemory(pvCompressed, MY_BURIKO_SCRIPT_MAGIC, sizeof(MY_BURIKO_SCRIPT_MAGIC)))
		{
			UINT OutSize;

			OutSize = UCL_NRV2E_DecompressASMFast32((PBYTE)pvCompressed + sizeof(MY_BURIKO_SCRIPT_MAGIC), pvDecompressed);
			if (pOutSize != NULL)
				*pOutSize = OutSize;
		}
		else
		{
			CopyMemory(pvDecompressed, pvCompressed, InSize);
			if (pOutSize != NULL)
				*pOutSize = InSize;
		}
		Result = 0;
	}
	return Result;
}




ULONG UnicodeWin32FindDataToAnsi(LPWIN32_FIND_DATAW pwfdW, LPWIN32_FIND_DATAA pwfdA)
{
	pwfdA->dwFileAttributes = pwfdW->dwFileAttributes;
	pwfdA->ftCreationTime   = pwfdW->ftCreationTime;
	pwfdA->ftLastAccessTime = pwfdW->ftLastAccessTime;
	pwfdA->ftLastWriteTime  = pwfdW->ftLastWriteTime;
	pwfdA->nFileSizeHigh    = pwfdW->nFileSizeHigh;
	pwfdA->nFileSizeLow     = pwfdW->nFileSizeLow;
	pwfdA->dwReserved0      = pwfdW->dwReserved0;
	pwfdA->dwReserved1      = pwfdW->dwReserved1;

	WideCharToMultiByte(CP_ACP, 0, pwfdW->cFileName, -1, pwfdA->cFileName, sizeof(pwfdA->cFileName), 0, 0);
	return WideCharToMultiByte(CP_ACP, 0, pwfdW->cAlternateFileName, -1, pwfdA->cAlternateFileName, sizeof(pwfdA->cAlternateFileName), 0, 0);
}


//sub_465580
//Checked
//ReadFile Outer Firstly
LONG 
CDECL
LoadFileBuffer(
PVOID  pvDecompressed, //64M 预先分配的
PULONG pOutSize,
ULONG  SkipBytes,
ULONG  OutBytes
)
{
	LPCSTR lpFileName;
	__asm mov lpFileName, ecx;

	LONG   RetValue = 5;
	PBYTE  InBuffer = nullptr;
	ULONG  InSize;

	//Tayutama2Hook::GetGlobalData()->WriteInfo(lpFileName, 1);

	if (Tayutama2Hook::GetGlobalData()->LoadFileBuffer(lpFileName, InBuffer, InSize) == S_OK)
	{

		//Tayutama2Hook::GetGlobalData()->WriteInfo(lpFileName, 1);

		__asm mov ecx, pvDecompressed;

#if 0
		RetValue = HookDecompressFile_FASTCALL(
			InBuffer,
			pOutSize,
			InSize,
			0,
			0);
#else
#if 0
		__asm
		{
			push 0
				push 0
				push InSize
				push pOutSize
				push InBuffer
				call HookDecompressFile_FASTCALL
				mov RetValue, eax
				add esp, 14h
		}
#else
		RetValue = HookDecompressFile_USERDATA(pvDecompressed, InBuffer, pOutSize, InSize, 0, 0);
#endif
#endif

		if (InBuffer)
			HostFree(InBuffer);

		return RetValue;
	}
	else
	{
#if 0
		Tayutama2Hook::WriteInfo(L"Ori:");
		Tayutama2Hook::WriteInfo(lpFileName);

		__asm
		{
			mov ecx, pvDecompressed;
			push 0
				push 0
				push InSize
				push pOutSize
				push InBuffer
				call Tayutama2Hook::OldLoadFileImm
				mov RetValue, eax
				add esp, 10h
		}
#else
		RetValue = 1;
#endif
		return RetValue;
	}
}

//==========================
//GBK Environment Support

/*

/4A5A20
Stub 1
00C95A20                                                /$  55             push ebp                                        ; Tayutama2_trial_WEB.00C95A20(guessed Arg1)
00C95A21                                                |.  8BEC           mov ebp,esp
00C95A23                                                |.  8A45 08        mov al,byte ptr [ebp+8]
00C95A26                                                |.  3C 80          cmp al,80
00C95A28                                                |.  73 06          jae short 00C95A30
00C95A2A                                                |.  33C0           xor eax,eax
00C95A2C                                                |.  5D             pop ebp
00C95A2D                                                |.  C2 0400        retn 4
00C95A30                                                |>  3C A0          cmp al,0A0
00C95A32                                                |.  73 09          jae short 00C95A3D
00C95A34                                                |.  B8 01000000    mov eax,1
00C95A39                                                |.  5D             pop ebp
00C95A3A                                                |.  C2 0400        retn 4
00C95A3D                                                |>  3C E0          cmp al,0E0
00C95A3F                                                |.  1BC0           sbb eax,eax
00C95A41                                                |.  40             inc eax
00C95A42                                                |.  5D             pop ebp
00C95A43                                                \.  C2 0400        retn 4

//sub_42FA80
Stub2
00C1FA80                                                /$  3C 80          cmp al,80                                       ; Tayutama2_trial_WEB.00C1FA80(guessed void)
00C1FA82                                                |.  73 03          jae short 00C1FA87
00C1FA84                                                |.  33C0           xor eax,eax
00C1FA86                                                |.  C3             retn
00C1FA87                                                |>  3C A0          cmp al,0A0
00C1FA89                                                |.  73 06          jae short 00C1FA91
00C1FA8B                                                |.  B8 01000000    mov eax,1
00C1FA90                                                |.  C3             retn
00C1FA91                                                |>  3C E0          cmp al,0E0
00C1FA93                                                |.  1BC0           sbb eax,eax
00C1FA95                                                |.  40             inc eax
00C1FA96                                                \.  C3             retn

//00495970
Stub3
00C85970                                                /$  3C 80          cmp al,80                                       ; Tayutama2_trial_WEB.00C85970(guessed void)
00C85972                                                |.  72 04          jb short 00C85978
00C85974                                                |.  3C A0          cmp al,0A0
00C85976                                                |.  72 07          jb short 00C8597F
00C85978                                                |>  3C E0          cmp al,0E0
00C8597A                                                |.  73 03          jae short 00C8597F
00C8597C                                                |.  33C0           xor eax,eax
00C8597E                                                |.  C3             retn
00C8597F                                                |>  B8 01000000    mov eax,1
00C85984                                                \.  C3             retn

*/

ASM int CheckFontCode1(BYTE a1)
{
	__asm
	{
		push ebp
		mov ebp, esp
		mov al, byte ptr[ebp + 8]
		cmp al, 80h
		jae loc_00C95A30
		xor eax, eax
		pop ebp
		retn 4
		
		loc_00C95A30:
		cmp al, 0FEh
		jae loc_00C95A3D
		mov eax, 1
		pop ebp
		retn 4

		loc_00C95A3D:
		cmp al, 0E0h
		sbb eax, eax
		inc eax
		pop ebp
		retn 4
	}
}

//sub_476100
ASM BOOL CheckFontCode2(BYTE a1)
{
	__asm
	{
		cmp al, 80h
		jae loc_00D0FA87
		xor eax, eax
		retn

		loc_00D0FA87:
		cmp al, 0FEh
		jae loc_00D0FA91
		mov eax, 1
		retn
		
		loc_00D0FA91:
		cmp al, 0E0h
		sbb eax, eax
		inc eax
		retn
	}
}


ASM BOOL CheckFontCode3(BYTE a1)
{
	__asm
	{
		cmp al, 80h
		jb loc_00D75978
		cmp al, 0FEh
		jb loc_00D7597F

		loc_00D75978:
		cmp al, 0E0h
		jae loc_00D7597F
		xor eax, eax
		retn

		loc_00D7597F:
		mov eax, 1
		retn
	}
}

UINT WINAPI HookGetOEMCP(void)
{
	return 936;
}

UINT WINAPI HookGetACP(void)
{
	return 936;
}

//==========================
//C++

Tayutama2Hook* Tayutama2Hook::Handle = nullptr;

PVOID Tayutama2Hook::OldDecompressFile       = (PVOID)0x00465320;
PVOID Tayutama2Hook::OldCheckOSDefaultLangID = (PVOID)0x0046F6F0;
PVOID Tayutama2Hook::OldLoadFileImm          = (PVOID)0x00465580;
PVOID Tayutama2Hook::OldCheckFont1           = (PVOID)0x004A5A20;
PVOID Tayutama2Hook::OldCheckFont2           = (PVOID)0x0042FA80;
PVOID Tayutama2Hook::OldCheckFont3           = (PVOID)0x00495970;

Tayutama2Hook::Tayutama2Hook() :
	hSelfModule(nullptr),
	IndexBuffer(nullptr),
	InitFileSystem(FALSE),
	MainWin(nullptr),
	DefFont(nullptr),
	pfTextOutA(nullptr),
	pfTextOutW(nullptr),
	pfCreateFontA(nullptr),
	pfGetOEMCP(nullptr),
	pfGetACP(nullptr),
	pfCreateWindowExA(nullptr),
	pfSetWindowTextA(nullptr),
	pfCreateFileA(nullptr),
	pfMessageBoxA(nullptr),
	pfMultiByteToWideChar(nullptr)
{

}

Tayutama2Hook::~Tayutama2Hook()
{
	if (IndexBuffer)
	{
		HeapFree(GetProcessHeap(), 0, IndexBuffer);
	}
}

Tayutama2Hook* Tayutama2Hook::GetGlobalData()
{
	if (!Handle)
	{
		Handle = new Tayutama2Hook;
		if (!Handle)
		{
			MessageBoxW(NULL, L"初始化错误", BaseAppName, MB_OK);
			return nullptr;
		}
	}
	return Handle;
}

BOOL IATPatch(LPCSTR szDllName, PROC pfnOrg, PROC pfnNew)
{
	HMODULE hmod;
	LPCSTR szLibName;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc;
	PIMAGE_THUNK_DATA pThunk;
	DWORD dwOldProtect, dwRVA;
	PBYTE pAddr;

	hmod = GetModuleHandleW(NULL);
	pAddr = (PBYTE)hmod;
	pAddr += *((DWORD*)&pAddr[0x3C]);
	dwRVA = *((DWORD*)&pAddr[0x80]);

	pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)((DWORD)hmod + dwRVA);

	for (; pImportDesc->Name; pImportDesc++)
	{
		szLibName = (LPCSTR)((DWORD)hmod + pImportDesc->Name);
		if (!_stricmp(szLibName, szDllName))
		{
			pThunk = (PIMAGE_THUNK_DATA)((DWORD)hmod + pImportDesc->FirstThunk);
			for (; pThunk->u1.Function; pThunk++)
			{
				if (pThunk->u1.Function == (DWORD)pfnOrg)
				{
					VirtualProtect((LPVOID)&pThunk->u1.Function, 4, PAGE_EXECUTE_READWRITE, &dwOldProtect);
					pThunk->u1.Function = (DWORD)pfnNew;
					VirtualProtect((LPVOID)&pThunk->u1.Function, 4, dwOldProtect, &dwOldProtect);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}


void WriteMemory(PBYTE Code, ULONG Addr, ULONG Len)
{
	DWORD OldCode = 0;
	VirtualProtect((LPVOID)Addr, Len, PAGE_EXECUTE_READWRITE, &OldCode);
	RtlCopyMemory((PVOID)Addr, Code, Len);
}


HRESULT WINAPI Tayutama2Hook::Init(HMODULE hSelf)
{
	if (!hSelfModule)
	{
		hSelfModule = hSelf;
	}

	LoadFileSystem();
	SetAppName();

	//DWORD nFont;
	//DefFont = (HFONT)AddFontMemResourceEx(TTFDataStatic, TTFDataStaticSize, NULL, &nFont);

	//PBYTE FxxkCompiler = TTFDataStatic;
	
	//DefFont = (HFONT)AddFontMemResourceEx(TTFDataStaticRelease, TTFDataStaticReleaseSize, NULL, &nFont);
	//DefFont = (HFONT)AddFontResourceExW(L"ca.ttf", FR_PRIVATE, NULL);
	//if (DefFont)
	//{
		//
	//}

	BOOL Result = True;
#define IF_FAILED(x, y) if(!x) goto y

	pfTextOutW            = GetProcAddress(GetModuleHandleW(L"Gdi32.dll"),    "TextOutW");
	pfTextOutA            = GetProcAddress(GetModuleHandleW(L"Gdi32.dll"),    "TextOutA");
	pfCreateFontA         = GetProcAddress(GetModuleHandleW(L"Gdi32.dll"),    "CreateFontA");
	pfGetOEMCP            = GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "GetOEMCP");
	pfGetACP              = GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "GetACP");
	pfCreateWindowExA     = GetProcAddress(GetModuleHandleW(L"User32.dll"),   "CreateWindowExA");
	pfSetWindowTextA      = GetProcAddress(GetModuleHandleW(L"User32.dll"),   "SetWindowTextA");
	pfCreateFileA         = GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "CreateFileA");
	pfMessageBoxA         = GetProcAddress(GetModuleHandleW(L"User32.dll"),   "MessageBoxA");
	pfMultiByteToWideChar = GetProcAddress(GetModuleHandleW(L"Kernel32.dll"), "MultiByteToWideChar");

	Result = IATPatch("Gdi32.dll",   pfTextOutW,           (PROC)HookTextOutW);           IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("Gdi32.dll",   pfTextOutA,           (PROC)HookTextOutA);           IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("Gdi32.dll",   pfCreateFontA,        (PROC)HookCreateFontA);        IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("User32.dll",  pfCreateWindowExA,    (PROC)HookCreateWindowExA);    IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("User32.dll",  pfSetWindowTextA,     (PROC)HookSetWindowTextA);     IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("Kernel32.dll",pfCreateFileA,        (PROC)HookCreateFileA);        IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("User32.dll",  pfMessageBoxA,        (PROC)HookMessageBoxA);        IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("Kernel32.dll",pfMultiByteToWideChar,(PROC)HookMultiByteToWideChar);IF_FAILED(Result, ErrorEnd);

#if 1
	Result = IATPatch("Kernel32.dll", pfGetOEMCP,    (PROC)HookGetOEMCP);    IF_FAILED(Result, ErrorEnd);
	Result = IATPatch("Kernel32.dll", pfGetACP,      (PROC)HookGetACP);      IF_FAILED(Result, ErrorEnd);
#endif

	LONG RelocateValue = (SIZE_T)GetModuleHandleW(nullptr) - 0x00400000;

	PVOID DynamicOldCheckOSDefaultLangID = (PVOID)((SIZE_T)OldCheckOSDefaultLangID + RelocateValue);
	OldCheckOSDefaultLangID = DynamicOldCheckOSDefaultLangID;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldCheckOSDefaultLangID, CheckOSDefaultLangID);
	DetourTransactionCommit();

	PVOID DynamicOldDecompressFile = (PVOID)((SIZE_T)OldDecompressFile + RelocateValue);
	OldDecompressFile = DynamicOldDecompressFile;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldDecompressFile, HookDecompressFile_FASTCALL);
	DetourTransactionCommit();

	PVOID DynamicOldLoadFileImm = (PVOID)((SIZE_T)OldLoadFileImm + RelocateValue);
	OldLoadFileImm = DynamicOldLoadFileImm;

#if 1
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldLoadFileImm, ::LoadFileBuffer);
	DetourTransactionCommit();
#endif

	PVOID DynamicOldCheckFont1 = (PVOID)((SIZE_T)OldCheckFont1 + RelocateValue);
	OldCheckFont1 = DynamicOldCheckFont1;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldCheckFont1, CheckFontCode1);
	DetourTransactionCommit();


	PVOID DynamicOldCheckFont2 = (PVOID)((SIZE_T)OldCheckFont2 + RelocateValue);
	OldCheckFont2 = DynamicOldCheckFont2;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldCheckFont2, CheckFontCode2);
	DetourTransactionCommit();

	PVOID DynamicOldCheckFont3 = (PVOID)((SIZE_T)OldCheckFont3 + RelocateValue);
	OldCheckFont3 = DynamicOldCheckFont3;

	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&OldCheckFont3, CheckFontCode3);
	DetourTransactionCommit();

	/*
	.text:0042E498                 cmp     ebx, 0EF40h
	.text:0042E4E8                 cmp     ebx, 0EF40h
	.text:0042F896                 cmp     ebx, 0EF40h
	.text:0042F8ED                 cmp     ebx, 0EF40h

	Code:
	00F1F896                      |.  81FB 40EF0000  cmp ebx,0EF40
	*/

	PVOID DynamicHookFontBounary1 = (PVOID)((SIZE_T)0x0042E3B8 + RelocateValue);
	PVOID DynamicHookFontBounary2 = (PVOID)((SIZE_T)0x0042E408 + RelocateValue);
	PVOID DynamicHookFontBounary3 = (PVOID)((SIZE_T)0x0042F7B6 + RelocateValue);
	PVOID DynamicHookFontBounary4 = (PVOID)((SIZE_T)0x0042F80D + RelocateValue);

	static BYTE HookCodeEBX[] = { 0x81, 0xFB, 0x40, 0xFE };

	WriteMemory(HookCodeEBX, (ULONG_PTR)DynamicHookFontBounary1, sizeof(HookCodeEBX));
	WriteMemory(HookCodeEBX, (ULONG_PTR)DynamicHookFontBounary2, sizeof(HookCodeEBX));
	WriteMemory(HookCodeEBX, (ULONG_PTR)DynamicHookFontBounary3, sizeof(HookCodeEBX));
	WriteMemory(HookCodeEBX, (ULONG_PTR)DynamicHookFontBounary4, sizeof(HookCodeEBX));

	//WriteMemory(HookCodeEBX, 0x00425124, sizeof(HookCodeEBX));
	//WriteMemory(HookCodeEBX, 0x00425194, sizeof(HookCodeEBX));
	//WriteMemory(HookCodeEAX, 0x00426425, sizeof(HookCodeEAX));
	//WriteMemory(HookCodeEAX, 0x00426469, sizeof(HookCodeEAX));

	/*
	.text:0042E542                 cmp     ebx, 8140h
	.text:0042E889                 cmp     eax, 8140h
	*/

	PVOID DynamicHookFontSpace1 = (PVOID)((SIZE_T)0x0042E462 + RelocateValue);
	PVOID DynamicHookFontSpace2 = (PVOID)((SIZE_T)0x0042E7A9 + RelocateValue);
	static BYTE HookSpaceEBX[] = { 0x81, 0xFB, 0xA1, 0xA1 };
	static BYTE HookSpaceEAX[] = { 0x3D, 0x40, 0xA1, 0xA1 };

	WriteMemory(HookSpaceEBX, (ULONG_PTR)DynamicHookFontSpace1, sizeof(HookSpaceEBX));
	WriteMemory(HookSpaceEAX, (ULONG_PTR)DynamicHookFontSpace2, sizeof(HookSpaceEAX));

	//MessageBoxW(NULL, L"attach", 0, 0);

	return S_OK;

ErrorEnd:
	ExitMessage(L"运行错误=。=");
	return S_FALSE;
}

HRESULT WINAPI Tayutama2Hook::UnInit(HMODULE hSelf)
{
	UNREFERENCED_PARAMETER(hSelf);
	
	//MessageBox(0, L"UnLoad", 0, 0);

	if (DefFont)
		RemoveFontMemResourceEx(DefFont);
	
	SystemFile.Release();
	return S_OK;
}

HFONT WINAPI Tayutama2Hook::DuplicateFontW(HDC hdc, UINT LangId)
{
	LOGFONTW* lplf = (LOGFONTW*)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(LOGFONTW));

	HFONT hOldFont = (HFONT)GetCurrentObject(hdc, OBJ_FONT);
	GetObject(hOldFont, sizeof(LOGFONTW), lplf);
	LOGFONTW Info = { 0 };

	Info.lfHeight = lplf->lfHeight;
	Info.lfWidth = lplf->lfWidth;
	Info.lfEscapement = lplf->lfEscapement;
	Info.lfOrientation = lplf->lfOrientation;
	Info.lfWeight = lplf->lfWeight;
	Info.lfItalic = lplf->lfItalic;
	Info.lfUnderline = lplf->lfUnderline;
	Info.lfStrikeOut = lplf->lfStrikeOut;
	Info.lfOutPrecision = lplf->lfOutPrecision;
	Info.lfClipPrecision = lplf->lfClipPrecision;
	Info.lfQuality = lplf->lfQuality;
	Info.lfPitchAndFamily = lplf->lfPitchAndFamily;
	lstrcpyW(Info.lfFaceName, LangId == CP_SHIFTJIS ? L"MS Gothic" : L"黑体");
	lplf->lfCharSet = LangId;

	HFONT hFont = CreateFontIndirectW(&Info);

	HeapFree(GetProcessHeap(), 0, lplf);
	return hFont;
}


ULONG WINAPI Tayutama2Hook::AnsiToUnicode(
	LPCSTR lpAnsi,
	ULONG  Length,
	LPWSTR lpUnicodeBuffer,
	ULONG  BufferCount,
	ULONG  CodePage)
{
	return MultiByteToWideChar(CodePage, 0, lpAnsi, Length, lpUnicodeBuffer, BufferCount);
}

UINT WINAPI Tayutama2Hook::ExitMessage(const WCHAR* Info, const WCHAR* Title)
{
	UINT Code = MessageBoxW(nullptr, Info, Title, MB_OK);
	::ExitProcess(-1);
	return Code;
}


BOOL WINAPI Tayutama2Hook::WriteInfo(const WCHAR* lpInfo, BOOL ForRelease)
{
	if (!ForRelease)
		return FALSE;

	DWORD ByteRead = 0;
	return WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), lpInfo, lstrlenW(lpInfo), &ByteRead, nullptr) &&
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &ByteRead, nullptr);
}

BOOL WINAPI Tayutama2Hook::WriteInfo(const CHAR* lpInfo, BOOL ForRelease)
{
	if (!ForRelease)
		return FALSE;

	DWORD ByteRead = 0;
	return WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), lpInfo, lstrlenA(lpInfo), &ByteRead, nullptr) &&
		WriteConsoleW(GetStdHandle(STD_OUTPUT_HANDLE), L"\n", 1, &ByteRead, nullptr);
}

#define ProjectDir L"ProjectDir\\"


wstring ToLowerString(wstring& Name)
{
	wstring Result;
	for (auto it : Name)
	{
		if ((it >= L'A') && (it <= L'Z'))
		{
			Result += tolower(it);
		}
		else
		{
			Result += it;
		}
	}
	return Result;
}

HRESULT WINAPI Tayutama2Hook::LoadFileBuffer(LPCSTR lpFileName, PBYTE& Buffer, ULONG& OutSize)
{
	HRESULT Result = S_OK;
	WCHAR   szFileName[MAX_PATH] = { 0 };

	AnsiToUnicode(lpFileName, lstrlenA(lpFileName), szFileName, MAX_PATH);
	wstring FileName = GetPackageName(wstring(szFileName));

	if (!InitFileSystem)
	{
		wstring Path = ProjectDir;
		Path += FileName;
		WinFile File;

		if (File.Open(Path.c_str(), WinFile::FileRead) == S_FALSE)
		{
			WriteInfo(Path.c_str());
			return S_FALSE;
		}

		OutSize = File.GetSize32();
		Buffer = (PBYTE)HostAlloc(OutSize);
		if (Buffer == nullptr)
		{
			Result = S_FALSE;
			goto ProjectReadEnd;
		}
		File.Read(Buffer, OutSize);

		ProjectReadEnd:
		File.Release();
	}
	else
	{
		auto it = ChunkList.find(Hash64(ToLowerString(FileName).c_str(), FileName.length() * 2));
		if (it != ChunkList.end())
		{
			OutSize = it->second.Size;
			Buffer = (PBYTE)HostAlloc(OutSize);
			if (Buffer == nullptr)
			{
				return S_FALSE;
			}
			SystemFile.Seek(it->second.Offset, FILE_BEGIN);
			SystemFile.Read(Buffer, OutSize);

		}
		else
		{
			return S_FALSE;
		}
	}
	return Result;
}
