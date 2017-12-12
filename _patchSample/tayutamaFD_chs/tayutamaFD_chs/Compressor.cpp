#include <windows.h>
#include "Compressor.h"


#define NRV2E

#include "ucl/ucl_conf.h"
#include <ucl/ucl.h>
#include "ucl/n2_99.c"
#include "ucl/alloc.c"

Bool
FASTCALL
UCL_NRV2E_Compress(
PVoid   Input,
ULong   InputSize,
PVoid   Output,
PULong  OutputSize,
Long    Level
)
{
	Bool                    Result;
	ucl_compress_config_t   UclConfig;

	FillMemory(&UclConfig, sizeof(UclConfig), (Byte)-1);
	UclConfig.bb_endian = 0;
	UclConfig.bb_size = 32;
	UclConfig.max_offset = 0x3FFFFF;

	Result = ucl_nrv2e_99_compress(
		(PByte)Input,
		InputSize,
		(PByte)Output,
		(ucl_uint *)OutputSize,
		NULL,
		Level,
		&UclConfig,
		NULL
		);

	return Result == UCL_E_OK;
}

Bool
FASTCALL
UCL_NRV2E_Decompress(
PVoid   Input,
ULong   InputSize,
PVoid   Output,
PULong  OutputSize
)
{
	ULong Size;
	UNREFERENCED_PARAMETER(InputSize);
	Size = (ULONG)UCL_NRV2E_DecompressASMFast32(Input, Output);
	if (OutputSize != NULL)
		*OutputSize = Size;

	return True;
}

ASM ULONG_PTR FASTCALL UCL_NRV2E_DecompressASMFast32(PVOID /* pvInput */, PVOID /* pvOutput */)
{
	INLINE_ASM
	{
		add     esp, -0x18;
		mov[esp + 0x00], ebx;
		mov[esp + 0x04], ebp;
		mov[esp + 0x08], esi;
		mov[esp + 0x0C], edi;
		mov[esp + 0x10], edx;
		cld;
		mov     esi, ecx;
		mov     edi, edx;
		or      ebp, 0xFFFFFFFF;
		xor     ecx, ecx;
		jmp L029;

		INLINE_ASM __emit 0x8D INLINE_ASM __emit 0xB4 INLINE_ASM __emit 0x26 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00;   // lea esi, [esi]
		INLINE_ASM __emit 0x8D INLINE_ASM __emit 0xB4 INLINE_ASM __emit 0x26 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00 INLINE_ASM __emit 0x00;   // lea esi, [esi]
	L022:
		mov     al, byte ptr[esi];
		inc     esi;
		mov     byte ptr[edi], al;
		inc     edi;
	L026:
		add     ebx, ebx;
		jnb L033;
		jnz L022;
	L029:
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
		jb L022;
	L033:
		mov     eax, 0x1;
	L034:
		add     ebx, ebx;
		jnz L039;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L039:
		adc     eax, eax;
		add     ebx, ebx;
		jnb L047;
		jnz L055;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
		jb L055;
	L047:
		dec     eax;
		add     ebx, ebx;
		jnz L053;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L053:
		adc     eax, eax;
		jmp L034;
	L055:
		sub     eax, 0x3;
		jb L072;
		shl     eax, 0x8;
		mov     al, byte ptr[esi];
		inc     esi;
		xor     eax, 0xFFFFFFFF;
		je L120;
		sar     eax, 1;
		mov     ebp, eax;
		jnb L078;
	L065:
		add     ebx, ebx;
		jnz L070;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L070:
		adc     ecx, ecx;
		jmp L099;
	L072:
		add     ebx, ebx;
		jnz L077;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L077:
		jb L065;
	L078:
		inc     ecx;
		add     ebx, ebx;
		jnz L084;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L084:
		jb L065;
	L085:
		add     ebx, ebx;
		jnz L090;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
	L090:
		adc     ecx, ecx;
		add     ebx, ebx;
		jnb L085;
		jnz L098;
		mov     ebx, dword ptr[esi];
		sub     esi, -0x4;
		adc     ebx, ebx;
		jnb L085;
	L098:
		add     ecx, 0x2;
	L099:
		cmp     ebp, -0x500;
		adc     ecx, 0x2;
		lea     edx, dword ptr[edi + ebp];
		cmp     ebp, -0x4;
		jbe L111;
	L104:
		mov     al, byte ptr[edx];
		inc     edx;
		mov     byte ptr[edi], al;
		inc     edi;
		dec     ecx;
		jnz L104;
		jmp L026;
	L111:
		mov[esp + 0x14], ecx;
		and     ecx, ~3;
		jecxz   L111_END;
	L111_:
		mov     eax, dword ptr[edx];
		add     edx, 0x4;
		mov     dword ptr[edi], eax;
		add     edi, 0x4;
		sub     ecx, 0x4;
		ja L111_;

	L111_END:

		mov     ecx, [esp + 0x14];
		and     ecx, 3;
		jecxz   L111_LOOP_2_END;
		mov[esp + 0x14], ecx;

	L111_LOOP_2:
		mov     al, [edx];
		mov[edi], al;
		inc     edx;
		inc     edi;
		loop    L111_LOOP_2;

		mov     ecx, [esp + 0x14];
		sub     edx, ecx;
		add     edx, 4;

	L111_LOOP_2_END:

		//        add     edi, ecx;
		xor     ecx, ecx;
		jmp L026;
	L120:
		mov     eax, edi;
		mov     ebx, [esp + 0x00];
		mov     ebp, [esp + 0x04];
		mov     esi, [esp + 0x08];
		mov     edi, [esp + 0x0C];
		sub     eax, [esp + 0x10];
		add     esp, 0x18;
		ret;
	}
}
