#ifndef _UCIGraph_
#define _UCIGraph_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


typedef unsigned char	U8;
typedef unsigned short	U16;
typedef unsigned int	U32;

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct
	{
		U8 red[256];
		U8 green[256];
		U8 blue[256];
	}SColorMap;

#pragma pack(push)
#pragma pack(1)
	typedef struct PictureInfo
	{
		int left, top;
		int width, height;
		unsigned short x_density, y_density;
		short bit;
		void* hInfo;
	}PictureInfo;
#pragma pack(pop)

	int __stdcall UCIDecode(
		const void* src,	// 输入UCI数据指针(不能传入null,其它指针参数可以传入null表示不需要输出)
		int 		srclen, // 输入UCI数据长度
		void**		dst,	// 输出RAW数据的指针(BGR或BGRA格式)
		int*		stride, // 输出RAW数据的行间字节跨度(dst不为null时,stride不能传入null)
		int*		width,	// 输出图像的宽度值
		int*		height, // 输出图像的高度值
		int*		bit);	// 输出图像的bpp值(每像素位数)

	void __stdcall UCIFree(void* p);

	void __stdcall UCIDebug(int level);

	BOOL __stdcall UCIInitOrUninit(DWORD reason);

	BOOL __stdcall gfpGetPluginInfo(DWORD version, char* label, int label_max_size, char* extension, int extension_max_size, int* support);

	void* __stdcall gfpLoadPictureInit(const WCHAR* filename);

	BOOL __stdcall gfpLoadPictureGetInfo(void* p, int* pictype, int* width, int* height, int* dpi, int* bits_per_pixel, int* bytes_per_line, BOOL* has_colormap, char* label, int label_max_size);

	BOOL __stdcall gfpLoadPictureGetLine(void* p, int line, U8* buf);

	BOOL __stdcall gfpLoadPictureGetColormap(void* p, SColorMap* cmap);

	void __stdcall gfpLoadPictureExit(void* p);

	int __stdcall IsSupported(const WCHAR* filename, int dw);

	int __stdcall GetPictureInfo(const WCHAR* filename, int len, unsigned flag, PictureInfo* pi);

	int __stdcall GetPicture(const WCHAR* filename, int len, unsigned flag, HLOCAL* hbi, HLOCAL* hbm, void* lpPrgressCallback, int lData);

	int __stdcall GetPreview(const WCHAR* filename, int len, unsigned flag, HLOCAL* hbi, HLOCAL* hbm, void* lpPrgressCallback, int lData);

#ifdef __cplusplus
}
#endif

#endif
