#include <Windows.h>
#include "TayutamaHook.h"

HRESULT WINAPI Init(HMODULE hSelf)
{
	if (!TayutamaHook::GetGlobalData())
	{
		TayutamaHook::ExitMessage(L"³õÊ¼»¯Ê§°Ü");
		return S_FALSE;
	}
	DisableThreadLibraryCalls(hSelf);
	return TayutamaHook::GetGlobalData()->Init(hSelf);
}

HRESULT WINAPI UnInit(HMODULE hSelf)
{
	if (!TayutamaHook::GetGlobalData())
	{
		//ignored some errors
		return S_FALSE;
	}
	return TayutamaHook::GetGlobalData()->UnInit(hSelf);
}
