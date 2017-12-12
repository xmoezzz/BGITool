#ifndef _Compressor_
#define _Compressor_

#include "NtDefine.h"

Bool
FASTCALL
UCL_NRV2E_Compress(
PVoid   Input,
ULong   InputSize,
PVoid   Output,
PULong  OutputSize,
Long    Level
);


Bool
FASTCALL
UCL_NRV2E_Decompress(
PVoid   Input,
ULong   InputSize,
PVoid   Output,
PULong  OutputSize
);

ULONG_PTR FASTCALL UCL_NRV2E_DecompressASMFast32(PVOID /* pvInput */, PVOID /* pvOutput */);



#endif
