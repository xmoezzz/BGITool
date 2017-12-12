#ifndef _NtDefine_
#define _NtDefine_

#include <Windows.h>

#define True  TRUE
#define False FALSE

typedef float               Float, *PFloat, *LPFloat;
typedef double              Double, *PDouble, *LPDouble;

typedef char                Char, s8, Int8, *ps8, *PInt8, *PChar;
typedef const char         *PCChar;
typedef wchar_t             WChar, *PWChar;
typedef const wchar_t      *PCWChar;
typedef unsigned char       u8, UChar, UInt8, UInt24, Byte, *pu8, *PByte, *PUInt8, *PUChar;
typedef short               Short, s16, Int16, *ps16, *PInt16, *PShort, *LPShort;
typedef unsigned short      UShort, Word, u16, UInt16, *pu16, *PUInt16, *PWord, *LPWord, *PUShort, *LPUShort;
typedef long                Long, *PLong, *LPLong;
typedef long long           Long64, LongLong, *PLong64, *PLongLong;
typedef unsigned long       Dword, ULong, *PULong, *LPULong, *PDword, *LPDword;
typedef unsigned long long  ULong64, ULongLong, *PULong64, *PULongLong;
typedef void                Void, *PVoid, *LPVoid;
typedef const void         *LPCVoid, *PCVoid;

typedef int                 Bool, Int, s32, Int32, *PInt, *ps32, *PInt32;
typedef unsigned int        UInt, u32, UInt32, *PUInt, *pu32, *PUInt32;

typedef __int64             s64, Int64, *ps64, *PInt64;
typedef unsigned __int64    u64, UInt64, *pu64, *PUInt64;

typedef int(__cdecl *QSORT_COMPARE_ROUTINE)(const void *, const void *);

#if defined(MY_X64)
typedef __int64 Int_Ptr, *PInt_Ptr;
typedef unsigned __int64 UInt_Ptr, *PUInt_Ptr;

typedef __int64 Long_Ptr, *PLong_Ptr, LongPtr, *PLongPtr;
typedef unsigned __int64 ULong_Ptr, *PULong_Ptr, ULongPtr, *PULongPtr;
#else
typedef int __w64 Int_Ptr, *PInt_Ptr;
typedef unsigned int __w64 UInt_Ptr, *PUInt_Ptr;

typedef long __w64 Long_Ptr, *PLong_Ptr, LongPtr, *PLongPtr;
typedef unsigned long __w64 ULong_Ptr, *PULong_Ptr, ULongPtr, *PULongPtr;
#endif

typedef ULong_Ptr SizeT, *PSizeT;
typedef Long_Ptr  SSizeT, *PSSizeT;

typedef ULong_Ptr SizeT, *PSizeT;
typedef Long_Ptr  SSizeT, *PSSizeT;

#define MAX_SHORT  (Short) (0x7FFF)
#define MAX_USHORT (UShort)(0xFFFF)
#define MAX_INT    (Int)   (0x7FFFFFFF)
#define MAX_UINT   (UInt)  (0xFFFFFFFF)
#define MAX_INT64  (Int64) (0x7FFFFFFFFFFFFFFF)
#define MAX_UINT64 (UInt64)(0xFFFFFFFFFFFFFFFF)
#define MAX_NTPATH  0x220


#define CHAR_UPPER(ch) (IN_RANGE('a', (ch), 'z') ? ((ch) & (Byte)0xDF) : ch)
#define CHAR_LOWER(ch) (IN_RANGE('A', (ch), 'Z') ? ((ch) | (Byte)~0xDF) : ch)

#define _CHAR_UPPER4W(ch) (UInt64)((ch) & 0xFFDFFFDFFFDFFFDF)
#define CHAR_UPPER4W(ch) _CHAR_UPPER4W((UInt64)(ch))
#define CHAR_UPPER3W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x0000FFFFFFFFFFFF)
#define CHAR_UPPER2W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x00000000FFFFFFFF)
#define CHAR_UPPER1W(ch) (UInt64)(CHAR_UPPER4W(ch) & 0x000000000000FFFF)

#define _CHAR_UPPER4(ch) (UInt32)((ch) & 0xDFDFDFDF)
#define CHAR_UPPER4(ch) (UInt32)_CHAR_UPPER4((UInt32)(ch))
#define CHAR_UPPER3(ch) (UInt32)(CHAR_UPPER4(ch) & 0x00FFFFFF)
#define CHAR_UPPER2(ch) (UInt32)(CHAR_UPPER4(ch) & 0x0000FFFF)
#define CHAR_UPPER1(ch) (UInt32)(CHAR_UPPER4(ch) & 0x000000FF)
#define CHAR_UPPER8(ch) ((UInt64)(ch) & 0xDFDFDFDFDFDFDFDF)

#define _TAG2(s) ((((s) << 8) | ((s) >> 8)) & 0xFFFF)
#define TAG2(s) _TAG2((u16)(s))

#define _TAG3(s) ( \
                (((s) >> 16) & 0xFF)       | \
                (((s)        & 0xFF00))    | \
                (((s) << 16) & 0x00FF0000) \
                )
#define TAG3(s) _TAG3((u32)(s))

#define _TAG4(s) ( \
                (((s) >> 24) & 0xFF)       | \
                (((s) >> 8 ) & 0xFF00)     | \
                (((s) << 24) & 0xFF000000) | \
                (((s) << 8 ) & 0x00FF0000) \
                )
#define TAG4(s) _TAG4((u32)(s))

#define TAG8(left, right) (((UInt64)TAG4(right) << 32) | TAG4(left))

#define _TAG2W(x) (((x) & 0xFF) << 16 | ((x) & 0xFF00) >> 8)
#define TAG2W(x) (UInt32)_TAG2W((UInt32)(x))

#define _TAG3W(x) (TAG4W(x) >> 16)
#define TAG3W(x) (UInt64)_TAG3W((UInt64)(x))

#define _TAG4W(x) (((UInt64)TAG2W((x) & 0xFFFF) << 32) | ((UInt64)TAG2W((x) >> 16)))
#define TAG4W(x) (UInt64)_TAG4W((UInt64)(x))

#pragma warning(disable:4310)
#define SWAP2(v) (u16)(((u32)(v) << 8) | ((u16)(v) >> 8))
#define SWAPCHAR(v) ((u32)SWAP2(v))

#define LoByte(v)  (u8) ((v & 0xFF))
#define HiByte(v)  (u8) (((v) >> 8) & 0xFF)
#define LoWord(v)  (u16)((v) & 0xFFFF)
#define HiWord(v)  (u16)(((v) >> 16) & 0xFFFF)
#define LoDword(v) (u32)((v))
#define HiDword(v) (u32)(((v) >> 32))

#define MakeLong(l, h)   (long)((s32)(l) | ((s32)(h) << 16))
#define MakeLong64(l, h) (s64)((s64)(l) | (s64)(h) << 32)

#define MakeDword(l, h) (u32)((u32)(l) | ((u32)(h) << 16))
#define MakeQword(l, h) (u64)((u64)(l) | (u64)(h) << 32)

#define MAKEINTRESA(i) ((PChar)(Word)(i))
#define MAKEINTRESW(i) ((PWChar)(Word)(i))

#define STRTOULONG(x) (ULong_Ptr)(x)

#define ML_IP_ADDRESS(a1, a2, a3, a4) ((a1) | ((a2) << 8) | ((a3) << 16) | ((a4) << 24))
#define ML_PORT(_port) SWAP2(_port)

#define FOR_EACH(_it, _base, _n) for (auto _Count = ( ((_it) = (_base)), (_n)); _Count != 0; ++(_it), --_Count)
#define FOR_EACH_REVERSE(_it, _base, _n) for (auto _Count = ( ((_it) = (_base) + (_n) - 1), (_n)); _Count != 0; --(_it), --_Count)
#define FOR_EACH_ARRAY(_it, _arr) FOR_EACH(_it, _arr, countof(_arr))
#define FOR_EACH_S(_it, _base, _n, _size) for (auto _Count = ( ((_it) = (_base)), (_n)); _Count != 0; ((_it) = PtrAdd(_it, _size)), --_Count)
#define FOR_EACH_X(_it, _base, _n) for (auto _Count = ( ((_it) = (_base)), (_n); _Count != 0; ++(_it), --(_n), --_Count)

#define FOR_EACH_FORWARD(_it, _n) { (_it) += (_n); (_Count) += (_n); }
#define FOR_EACH_BACKWARD(_it, _n) { (_it) -= (_n); (_Count) -= (_n); }

#define LOOP_ALWAYS for (;;)
#define LOOP_FOREVER LOOP_ALWAYS
#define LOOP_ONCE   for (Bool __condition_ = True; __condition_; __condition_ = False)

enum // ECodePage
{
	CP_SHIFTJIS = 932,
	CP_GBK = 936,
	CP_GB2312 = CP_GBK,
	CP_BIG5 = 950,
	CP_UTF16_LE = 1200,
	CP_UTF16_BE = 1201,
};

#define BOM_UTF8        (UInt32)(0xBFBBEF)
#define BOM_UTF16_LE    (UInt16)(0xFEFF)
#define BOM_UTF16_BE    (UInt16)(0xFFFE)

#define __MAKE_WSTRING(str) L##str
#define MAKE_WSTRING(str) __MAKE_WSTRING(str)

#define ASM_UNIQUE() INLINE_ASM mov eax, __LINE__

#define ASM_DUMMY(Bytes) ASM_DUMMY_##Bytes

#define ASM_DUMMY_1 INLINE_ASM nop
#define ASM_DUMMY_2 INLINE_ASM mov eax, eax

// lea eax, [eax+0];
#define ASM_DUMMY_3 INLINE_ASM __emit 0x8D INLINE_ASM __emit 0x40 INLINE_ASM __emit 0x00

// // lea esi, [esi]
#define ASM_DUMMY_4 INLINE_ASM __emit 0x8D \
                    INLINE_ASM __emit 0x74 \
                    INLINE_ASM __emit 0x26 \
                    INLINE_ASM __emit 0x00

#define ASM_DUMMY_5 INLINE_ASM mov eax, 1
#define ASM_DUMMY_6 INLINE_ASM __emit 0x8D INLINE_ASM __emit 0x80 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00

#define ASM_DUMMY_7 INLINE_ASM __emit 0x8D \
                    INLINE_ASM __emit 0xB4 \
                    INLINE_ASM __emit 0x26 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00 \
                    INLINE_ASM __emit 0x00

#define ASM_DUMMY_AUTO() INLINE_ASM mov eax, 1 INLINE_ASM mov ecx, 1 INLINE_ASM mov edx, 1 ASM_UNIQUE() INLINE_ASM ret

#if !defined(_M_IA64)
#define MEMORY_PAGE_SIZE (4 * 1024)
#else
#define MEMORY_PAGE_SIZE (8 * 1024)
#endif

#define NO_BREAK

#define NT6_LIB(lib) "D:/Dev/Windows Kits/8.0/Lib/win8/um/x86/" #lib

#define LOOP_ALWAYS for (;;)
#define LOOP_FOREVER LOOP_ALWAYS
#define LOOP_ONCE   for (Bool __condition_ = True; __condition_; __condition_ = False)

#if !defined(BREAK_IF)
#define BREAK_IF(c) if (c) break;
#endif /* BREAK_IF */

#if !defined(CONTINUE_IF)
#define CONTINUE_IF(c) if (c) continue;
#endif /* CONTINUE_IF */

#if !defined(RETURN_IF)
#define RETURN_IF(c, r) if (c) return r
#endif /* RETURN_IF */

#if !defined(countof)
#define countof(x) (sizeof((x)) / sizeof(*(x)))
#endif /* countof */

#if !defined(CONST_STRLEN)
#define CONST_STRLEN(str) (countof(str) - 1)
#define CONST_STRSIZE(str) (CONST_STRLEN(str) * sizeof(str[0]))
#endif // CONST_STRLEN

#if !defined(bitsof)
#define bitsof(x) (sizeof(x) * 8)
#endif /* bitsof */

#define FIELD_BASE(address, type, field) (type *)((ULONG_PTR)address - (ULONG_PTR)&((type *)0)->field)
#define FIELD_TYPE(_Type, _Field)  TYPE_OF(((_Type*)0)->_Field)

#ifndef FIELD_SIZE
#define FIELD_SIZE(type, field) (sizeof(((type *)0)->field))
#endif // FIELD_SIZE

#define SET_FLAG(_V, _F)    ((_V) |= (_F))
#define CLEAR_FLAG(_V, _F)  ((_V) &= ~(_F))
#define FLAG_ON(_V, _F)     (!!((_V) & (_F)))
#define FLAG_OFF(_V, _F)     (!FLAG_ON(_V, _F))

#if !defined(TEST_BIT)
#define TEST_BIT(value, bit) ((value) & (1 << bit))
#endif /* TEST_BIT */

#if !defined(TEST_BITS)
#define TEST_BITS(value, bits) (Bool)(!!((value) & (bits)))
#endif /* TEST_BITS */

#if !defined(ROUND_DOWN)
#define ROUND_DOWN(Value, Multiple) ((Value) / (Multiple) * (Multiple))
#endif /* ROUND_DOWN */

#if !defined(ROUND_UP)
#define ROUND_UP(Value, Multiple) (ROUND_DOWN((Value) + (Multiple) - 1, (Multiple)))
#endif /* ROUND_UP */

#if !defined(IN_RANGE)
#define IN_RANGE(low, value, high) (((low) <= (value)) && (value) <= (high))
#define IN_RANGEEX(low, value, high) (((low) <= (value)) && (value) < (high))
#endif

#if !defined(MEMORY_PAGE_ADDRESS)
#define MEMORY_PAGE_ADDRESS(Address) (ROUND_DOWN((ULongPtr)(Address), MEMORY_PAGE_SIZE))
#endif /* MEMORY_PAGE_ADDRESS */

#if defined(DEFAULT_VALUE)
#undef DEFAULT_VALUE
#endif // DEFAULT_VALUE

#ifdef __cplusplus
#define DEFAULT_VALUE(type, var, value) type var = value
#define DEF_VAL(var, value)             var = value
#else
#define DEFAULT_VALUE(type, var, value) type var
#define DEF_VAL(var, value)             var
#endif //CPP_DEFINED

#ifndef FASTCALL
#define FASTCALL __fastcall
#endif

#ifndef ASM
#define ASM __declspec(naked)
#endif /* ASM */

#ifndef NAKED
#define NAKED __declspec(naked)
#endif /* ASM */

#ifndef INLINE_ASM
#define INLINE_ASM __asm
#endif

#endif

