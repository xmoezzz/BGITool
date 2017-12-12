#ifndef _MyNative_
#define _MyNative_

#include "NtDefine.h"
#include <Windows.h>
#include "ImageEx.h"
#include "UCIGraph.h"


Bool IsShiftJISChar(SizeT uChar);
Bool IsShiftJISString(PCChar pString, SizeT Length);

inline PVoid SkipUCIStream(PVoid Data, Int32 DataSize, PUInt32 ExtraInfoSize)
{
	PByte Buffer = (PByte)Data;
	UInt32 StreamCount;

	switch (*(PUInt32)Buffer)
	{
	case TAG4('UCI3'):
	case TAG4('UCI '):
	case TAG4('UCI@'): StreamCount = 1; break;
	case TAG4('UCI4'):
	case TAG4('UCI!'):
	case TAG4('UCIA'): StreamCount = 2; break;
	case TAG4('UCIT'): StreamCount = 3; break;
	case TAG4('UCIQ'): StreamCount = 4; break;
	default: return NULL;
	}

	Buffer += 0xC;
	DataSize -= 0xC;
	while (StreamCount--)
	{
		UInt32 Size = *(PUInt32)Buffer + 4;
		Buffer += Size;
		DataSize -= Size;
	}

	if (DataSize <= 0)
	{
		if (ExtraInfoSize)
			*ExtraInfoSize = 0;
		return NULL;
	}

	if (ExtraInfoSize)
		*ExtraInfoSize = DataSize;

	return Buffer;
}

inline Int32 UCIDecodeEx(PVoid Data, Int32 DataSize, UCI_INFO *UCIInfo, Bool GetExtraInfo)
{
	Int32 ret;

	if (UCIInfo == NULL)
		return -1;

	ret = UCIDecode(Data, DataSize, &UCIInfo->lpBuffer, &UCIInfo->Stride, &UCIInfo->Width, &UCIInfo->Height, &UCIInfo->BitsPerPixel);
	if (ret < 0)
		return ret;

	if (GetExtraInfo)
	{
		UCIInfo->ExtraInfo = SkipUCIStream(Data, DataSize, &UCIInfo->ExtraInfoSize);
	}
	else
	{
		UCIInfo->ExtraInfo = NULL;
		UCIInfo->ExtraInfoSize = 0;
	}

	return ret;
}

inline Void UCIFreeEx(UCI_INFO *UCIInfo)
{
	if (UCIInfo == NULL)
		return;

	UCIFree(UCIInfo->lpBuffer);
}

inline
UInt32
ConvertRawTo8Bit(
UCI_INFO   *UciInfo,
Int32       Width,
Int32       Height,
Int32       BitsPerPixel,
PVoid       pvBuffer,
UInt32      BufferSize
)
{
	PByte              pbSrc, pbDest;
	INT32              BytesPerPixel, Stride;
	IMG_BITMAP_HEADER *pBmp;

	UNREFERENCED_PARAMETER(Width);
	UNREFERENCED_PARAMETER(Height);
	UNREFERENCED_PARAMETER(BitsPerPixel);

	if (BufferSize < sizeof(*pBmp))
		return 0;

	pBmp = (IMG_BITMAP_HEADER *)pvBuffer;
	InitBitmapHeader(pBmp, Width, Height, BitsPerPixel, &Stride);
	if (pBmp->dwFileSize > BufferSize)
		return 0;

	pbDest = (PByte)(pBmp + 1);
	for (ULong Color = 0, Count = 256; Count; Color += 0x00010101, --Count)
	{
		*(PULong)pbDest = Color;
		pbDest += 4;
	}

	BytesPerPixel = UciInfo->BitsPerPixel / 8;
	pbSrc = UciInfo->pbBuffer + (UciInfo->Height - 1) * UciInfo->Stride;
	pbDest = (PByte)pBmp + pBmp->dwRawOffset;
	for (ULong Height = UciInfo->Height;;)
	{
		PByte p1, p2;

		p1 = pbSrc;
		p2 = pbDest;
		for (ULong Width = UciInfo->Width;;)
		{
			ULong R, G, B, Color = *(PULong)p1;

			// Y = 0.299R + 0.587G + 0.114B

			/************************************************************************
			full range打算用这个常用的矩阵参数, JPEG应也用的是这个吧

			Y = 0.11448 * B + 0.58661 * G + 0.29891 * R
			U = 0.50000 * B - 0.33126 * G - 0.16874 * R + 128
			V =-0.08131 * B - 0.41869 * G + 0.50000 * R + 128

			优化后的公式如下:

			Y = (120041*B + 615105*G + 313430*R + 0x007FFFF) >> 20
			U = (524288*B - 347351*G - 176937*R + 0x807FFFF) >> 20
			V = (-85260*B - 439028*G + 524288*R + 0x807FFFF) >> 20
			************************************************************************/

			R = RGBA_GetRValue(Color);
			G = RGBA_GetGValue(Color);
			B = RGBA_GetBValue(Color);
			*p2 = (BYTE)((R * 313430 + G * 615105 + B * 120041 + 0x007FFFF) >> 20);
			++p2;
			p1 += BytesPerPixel;

			if (--Width == 0)
				break;
		}

		pbSrc -= UciInfo->Stride;
		pbDest += Stride;

		if (--Height == 0)
			break;
	}

	return pBmp->dwFileSize;
}

inline
UInt32
ConvertRawTo24Or32(
UCI_INFO   *pUciInfo,
Int32       Width,
Int32       Height,
Int32       BitsPerPixel,
PVoid       pvBuffer,
UInt32      BufferSize
)
{
	PByte              pbSrc, pbDest;
	INT32              Stride;
	IMG_BITMAP_HEADER *pBmp;

	if (BufferSize < sizeof(*pBmp))
		return 0;

	pBmp = (IMG_BITMAP_HEADER *)pvBuffer;
	InitBitmapHeader(pBmp, Width, Height, BitsPerPixel, &Stride);
	if (pBmp->dwFileSize > BufferSize)
		return 0;

	pbSrc = pUciInfo->pbBuffer;
	pbDest = (PByte)pBmp;
	pbDest += (Height - 1) * Stride + sizeof(*pBmp);

	for (ULong Height = pUciInfo->Height; Height; --Height)
	{
		RtlCopyMemory(pbDest, pbSrc, Stride);
		pbDest -= Stride;
		pbSrc += pUciInfo->Stride;
	}

	return pBmp->dwFileSize;
}

inline
UInt32
ConvertRawToBitMap(
UCI_INFO   *pUciInfo,
UInt32      Width,
UInt32      Height,
UInt32      BitsPerPixel,
PVoid       pvBuffer,
UInt32      BufferSize
)
{
	if (BitsPerPixel == 8)
	{
		return ConvertRawTo8Bit(pUciInfo, Width, Height, BitsPerPixel, pvBuffer, BufferSize);
	}
	else
	{
		switch (BitsPerPixel)
		{
		case 24:
		case 32:
			return ConvertRawTo24Or32(pUciInfo, Width, Height, BitsPerPixel, pvBuffer, BufferSize);
		}
	}

	return 0;
}

#endif
