#ifndef _BGIRawBuilder_
#define _BGIRawBuilder_

#include <Windows.h>

#pragma pack(push, 1)

typedef struct BGIBitmap
{
	WORD  Height;
	WORD  Width;
	DWORD Depth;
	DWORD Unk[2]; //Set Zero
}BGIBitmap, *pBGIBitMap;

typedef struct XmoeImage
{
	ULONG Maigc;
	ULONG RawSize;
}XmoeImage;

#pragma pack(pop)

#endif
