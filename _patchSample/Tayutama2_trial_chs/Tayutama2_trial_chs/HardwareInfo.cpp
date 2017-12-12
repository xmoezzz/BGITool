#include "Tayutama2Hook.h"
#include "GetSysInfo.h"

#define AppCPUInfoIntel L"Intel"
#define AppCPUInfoAMD   L"AMD"

#define AppGPUInfoIntel0  L"Intel:无法开启"
#define AppGPUInfoIntel1  L"Intel:硬件加速"
#define AppGPUInfoNvidia0 L"NVIDIA:无法开启"
#define AppGPUInfoNvidia1 L"NVIDIA:硬件加速"
#define AppGPUInfoAMD0    L"AMD-ATI:不是很懂你们AMD"
#define AppGPUInfoAMD1    L"AMD-ATI:不是很懂你们AMD"
#define AppGPUInfoATI0    L"AMD-ATI:不是很懂你们AMD"
#define AppGPUInfoATI1    L"AMD-ATI:不是很懂你们AMD"
#define AppGPUInfoUnk     L"未知显卡:无法开启"
#define AppDynamic0       L"动态汉化:关闭"
#define AppDynamic1       L"动态汉化:开启"

BOOL WINAPI GetIntelInternalSupport()
{
	HMODULE hMod = GetModuleHandleW(L"IntelQuickDecode.dll");
	if (!hMod)
		return FALSE;

	return TRUE;
}

BOOL WINAPI GetNvidiaInternalSupport()
{
	return TRUE;
}

BOOL WINAPI GetAMDInternalSupport()
{
	return FALSE;
}

HRESULT WINAPI Tayutama2Hook::SetAppName()
{
	int Argc = 0;
	BOOL ReadyForRelease = TRUE;
	PWCHAR* Args = CommandLineToArgvW(GetCommandLineW(), &Argc);
	
	for (int i = 0; i < Argc; i++)
	{
		if (!lstrcmpiW(Args[i], L"--xmoe_debug"))
		{
			ReadyForRelease = FALSE;
			break;
		}
	}

	if(ReadyForRelease)
	{
		AppName = BaseAppName;
		AppName += L"(通用计算Ver1.2：强制关闭)";
		return S_OK;
	}
	
	wstring CpuName, CpuType;
	DWORD CpuCount, CpuSpeed;
	GetSysInfo SystemInfo;
	SystemInfo.GetCpuInfo(CpuName, CpuType, CpuCount, CpuSpeed);

	AppName = BaseAppName;
	AppName += L"(Info : ";
	if (wcsstr(CpuName.c_str(), AppCPUInfoIntel))
	{
		AppName += L"Intel指令加速:开启, ";
	}
	else
	{
		AppName += L"Intel指令加速:关闭, ";
	}

	DISPLAY_DEVICEW dd = {0};
	dd.cb = sizeof(dd);
	vector<wstring> GraphCardList;
	int i = 0;
	HRESULT InfoResult = S_FALSE;

	AppName += L"解码模式:";
	
	while (EnumDisplayDevices(NULL, i, &dd, 0))
	{
		//wprintf(TEXT("Device Name: %s Device String: %s\n"), dd.DeviceName, dd.DeviceString);
		if (wcsstr(dd.DeviceString, L"Intel"))
		{
			InfoResult = S_OK;
			AppName += AppGPUInfoIntel0;
			break;
		}
		else if (wcsstr(dd.DeviceString, L"AMD") || wcsstr(dd.DeviceString, L"ATI"))
		{
			InfoResult = S_OK;
			if (GetAMDInternalSupport())
				AppName += AppGPUInfoAMD1;
			else
				AppName += AppGPUInfoAMD0;
			break;
		}
		else if (wcsstr(dd.DeviceString, L"NVIDIA"))
		{
			InfoResult = S_OK;
			if (GetNvidiaInternalSupport())
				AppName += AppGPUInfoNvidia1;
			else
				AppName += AppGPUInfoNvidia0;
			break;
		}

		if (EnumDisplayDevices(dd.DeviceName, 0, &dd, 0))
		{
			//wprintf(TEXT("Monitor Name: %s Monitor String: %s\n"), dd.DeviceName, dd.DeviceString);
		}
		i++;
	}

	if(FAILED(InfoResult))
		AppName += AppGPUInfoUnk;

	AppName += L", ";
	AppName += AppDynamic1;

	AppName += L")";
	return S_OK;
}

HRESULT WINAPI Tayutama2Hook::GetAppName(wstring& Name)
{
	Name = this->AppName;
	return S_OK;
}
