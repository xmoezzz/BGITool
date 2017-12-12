#ifndef _ImageEx_
#define _ImageEx_

#include <Windows.h>
#include "NtDefine.h"

#define RGBA(r, g, b, a) (UInt32)(((u32)(u8)(r)) | (((u32)(u8)(g) << 8)) | (((u32)(u8)(b) << 16)) | (((u32)(u8)(a) << 24)))

#define RGBA_GetRValue(color) ((Byte)((color) >> 0))
#define RGBA_GetGValue(color) ((Byte)((color) >> 8))
#define RGBA_GetBValue(color) ((Byte)((color) >> 16))
#define RGBA_GetAValue(color) ((Byte)((color) >> 24))
#define RGBA(r, g, b, a) (UInt32)(((u32)(u8)(r)) | (((u32)(u8)(g) << 8)) | (((u32)(u8)(b) << 16)) | (((u32)(u8)(a) << 24)))

#pragma pack(push, 1)

typedef struct
{
	UINT16 wTag;                   // 0x00
	UINT32 dwFileSize;             // 0x02
	UINT32 dwReserve;              // 0x06
	UINT32 dwRawOffset;            // 0x0A
	struct
	{
		UINT32 dwInfoHeaderSize;   // 0x0E
		UINT32 dwWidth;            // 0x12
		UINT32 dwHeight;           // 0x16
		UINT16 wLayer;             // 0x1A
		UINT16 wBit;               // 0x1C
		UINT32 dwCompressed;       // 0x1E
		UINT32 dwSizeImage;        // 0x22
		INT32  iXPelsPerMeter;     // 0x26
		INT32  iYPelsPerMeter;     // 0x2A
		UINT32 dwClrUsed;          // 0x2E
		UINT32 dwClrImportant;     // 0x32
	} Info;
} IMG_BITMAP_HEADER, SBitMapHeader, *PBitMapHeader, *LPBitMapHeader;

typedef struct
{
	UInt16 Tag;                   // 0x00
	UInt32 FileSize;             // 0x02
	UInt32 Reserve;              // 0x06
	UInt32 RawOffset;            // 0x0A
	struct
	{
		UInt32 InfoHeaderSize;   // 0x0E
		Int32  Width;            // 0x12
		Int32  Height;           // 0x16
		UInt16 Layer;             // 0x1A
		UInt16 Bit;               // 0x1C
		UInt32 Compressed;       // 0x1E
		UInt32 SizeImage;        // 0x22
		Int32  XPelsPerMeter;     // 0x26
		Int32  YPelsPerMeter;     // 0x2A
		UInt32 ClrUsed;          // 0x2E
		UInt32 ClrImportant;     // 0x32
	} Info;
} IMAGE_BITMAP_HEADER, *PIMAGE_BITMAP_HEADER;

typedef struct
{
	UCHAR  ID;
	UCHAR  byPaletteType;
	UCHAR  byImageType;
	UINT16 usPaletteEntry;
	UINT16 usPaletteLength;
	UCHAR  byPaletteBitCount;
	UINT16 usOriginX;
	UINT16 usOriginY;
	UINT16 usWidth;
	UINT16 usHeight;
	UCHAR  byPixelDepth;
	UCHAR  byImageDescription;
} IMG_TGA_HEADER, STGAHeader;

typedef struct
{
	union
	{
		LPVOID lpBuffer;
		PBYTE  pbBuffer;
	};
	INT32  Width;
	INT32  Height;
	INT32  BitsPerPixel;
	INT32  Stride;
	LPVOID lpExtraInfo;
	INT32  ExtraInfoSize;
} UCIInfo, *PUCIInfo, *LPUCIInfo;

typedef struct
{
	union
	{
		PVoid lpBuffer;
		PByte  pbBuffer;
	};
	Int32   Width;
	Int32   Height;
	Int32   BitsPerPixel;
	Int32   Stride;
	PVoid   ExtraInfo;
	UInt32  ExtraInfoSize;

} UCI_INFO;

#define UCI_META_INFO_MAGIC TAG4('META')

typedef struct
{
	UInt32 Magic;   // TAG4('META')
	UInt32 cbSize;
	Int32  Width;
	Int32  Height;
	Int32  BitsPerPixel;
} UCI_META_INFO;

typedef struct
{
	UCHAR   Width;
	UCHAR   Height;
	UCHAR   NumberOfColorInPalette;
	UCHAR   Reserve;

	union
	{
		struct
		{
			USHORT  Planes;
			USHORT  BitsPerPixel;
		} Ico;

		struct
		{
			USHORT  X;
			USHORT  Y;
		} Cur;
	};

	ULONG ImageSize;
	ULONG ImageOffset;

} IMAGE_ICO_IMAGE_ENTRY, *PIMAGE_ICO_IMAGE_ENTRY;

typedef struct
{
	USHORT Reserve;
	USHORT FileType;   // 1
	USHORT NumberOfImage;

	IMAGE_ICO_IMAGE_ENTRY   Entry[1];

} IMAGE_ICO_HEADER, *PIMAGE_ICO_HEADER;

#pragma pack(pop)

BOOL
__fastcall
InitBitmapHeader(
IMG_BITMAP_HEADER  *pHeader,
INT32               Width,
INT32               Height,
INT32               BitsPerPixel,
PINT32              pStride
);


#endif
