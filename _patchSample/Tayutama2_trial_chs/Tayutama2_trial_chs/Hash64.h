#ifndef _Hash64_
#define _Hash64_

#include <Windows.h>

ULONG64 WINAPI Hash64(const void * key, LONG len, ULONG seed = 0xEE6B27EB);

#endif
