#include <Windows.h>
#include "Tayutama2Hook.h"

HRESULT WINAPI Init(HMODULE hSelf)
{
	if (!Tayutama2Hook::GetGlobalData())
	{
		Tayutama2Hook::ExitMessage(L"³õÊ¼»¯Ê§°Ü");
		return S_FALSE;
	}
	DisableThreadLibraryCalls(hSelf);
	return Tayutama2Hook::GetGlobalData()->Init(hSelf);
}

HRESULT WINAPI UnInit(HMODULE hSelf)
{
	if (!Tayutama2Hook::GetGlobalData())
	{
		//ignored some errors
		return S_FALSE;
	}
	return Tayutama2Hook::GetGlobalData()->UnInit(hSelf);
}
