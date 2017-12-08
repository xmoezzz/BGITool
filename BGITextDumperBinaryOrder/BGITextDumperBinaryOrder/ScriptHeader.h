#ifndef _ScriptHeader_
#define _ScriptHeader_

#include <Windows.h>

//0x1C
typedef struct ScriptHeader
{
	CHAR  Magic[0x1C];
	ULONG HeaderSize;
} BGI_BURIKO_SCRIPT_HEADER;
//====================================


#endif
