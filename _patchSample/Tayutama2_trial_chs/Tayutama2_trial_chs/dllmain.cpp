#include "Common.h"

EXTERN_C
{
#include "UCIGraph.h" 
}

//Image Compressor
#pragma comment(lib, "UCIStatic.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "detours.lib")
#pragma comment(lib, "lz4.lib")
#pragma comment(lib, "libavcodec.a")

BOOL APIENTRY DllMain(HMODULE hModule, DWORD Reason, LPVOID lpReserved)
{
	UNREFERENCED_PARAMETER(lpReserved);

	switch (Reason)
	{
	case DLL_PROCESS_ATTACH:
	{
		//AllocConsole();
		UCIInitOrUninit(DLL_PROCESS_ATTACH);
		Init(hModule);
	}
	break;
	case DLL_PROCESS_DETACH:
	{
		UCIInitOrUninit(DLL_PROCESS_DETACH);
		UnInit(hModule);
	}
	break;
	}
	return TRUE;
}

