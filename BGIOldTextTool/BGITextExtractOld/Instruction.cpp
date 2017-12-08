#include "Instruction.h"


BGIInsRecord RecordInfo[] =
{
	{ 0x00, "PushDword", 1, { GetDword } },
	{ 0x01, "PushOffset", 1, { GetOffset } },
	{ 0x02, "PushPCOffset", 1, { GetDword } },
	{ 0x03, "PushString", 1, { GetString } },
	{ 0x04, NULL, 0 },
	{ 0x05, NULL, 0 },
	{ 0x06, NULL, 0 },
	{ 0x07, NULL, 0 },
	{ 0x08, "ReadMemory", 1, { GetDword } },
	{ 0x09, "WriteMemory", 1, { GetDword } },
	{ 0x0A, "WriteMemoryArgs", 1, { GetDword } },
	{ 0x0B, NULL, 0 },
	{ 0x0C, NULL, 0 },
	{ 0x0D, NULL, 0 },
	{ 0x0E, NULL, 0 },
	{ 0x0F, NULL, 0 },
	{ 0x10, "PushPC", 0 },
	{ 0x11, "PopPC", 0 },
	{ 0x12, NULL, 0 },
	{ 0x13, NULL, 0 },
	{ 0x14, NULL, 0 },
	{ 0x15, NULL, 0 },
	{ 0x16, NULL, 0 },
	{ 0x17, NULL, 0 },
	{ 0x18, "Jmp", 0 },
	{ 0x19, "JC", 1, { GetDword } },
	{ 0x1A, "Call", 0 },
	{ 0x1B, "Return", 0 },
	{ 0x1C, NULL, 0 },
	{ 0x1D, NULL, 0 },
	{ 0x1E, "ExceptionHandler", 0 },
	{ 0x1F, "UnregExceptionHandler", 0 },
	{ 0x20, "Add", 0 },
	{ 0x21, "Sub", 0 },
	{ 0x22, "Imul", 0 },
	{ 0x23, "Idlv", 0 },
	{ 0x24, "Mod", 0 },
	{ 0x25, "And", 0 },
	{ 0x26, "Or", 0 },
	{ 0x27, "Xor", 0 },
	{ 0x28, "Not", 0 },
	{ 0x29, "Shl", 0 },
	{ 0x2A, "Shr", 0 },
	{ 0x2B, "Sar", 0 },
	{ 0x2C, NULL, 0 },
	{ 0x2D, NULL, 0 },
	{ 0x2E, NULL, 0 },
	{ 0x2F, NULL, 0 },
	{ 0x30, "SetE", 0 },
	{ 0x31, "SetNE", 0 },
	{ 0x32, "SetLE", 0 },
	{ 0x33, "SetGE", 0 },
	{ 0x34, "SetL", 0 },
	{ 0x35, "SetG", 0 },
	{ 0x36, NULL, 0 },
	{ 0x37, NULL, 0 },
	{ 0x38, "AndBool", 0 },
	{ 0x39, "OrBool", 0 },
	{ 0x3A, "ZeroBool", 0 },
	{ 0x3B, NULL, 0 },
	{ 0x3C, NULL, 0 },
	{ 0x3D, NULL, 0 },
	{ 0x3E, NULL, 0 },
	{ 0x3F, "GetArgs", 1, { GetDword } },
	{ 0x40, NULL, 0 },
	{ 0x41, NULL, 0 },
	{ 0x42, NULL, 0 },
	{ 0x43, NULL, 0 },
	{ 0x44, NULL, 0 },
	{ 0x45, NULL, 0 },
	{ 0x46, NULL, 0 },
	{ 0x47, NULL, 0 },
	{ 0x48, NULL, 0 },
	{ 0x49, NULL, 0 },
	{ 0x4A, NULL, 0 },
	{ 0x4B, NULL, 0 },
	{ 0x4C, NULL, 0 },
	{ 0x4D, NULL, 0 },
	{ 0x4E, NULL, 0 },
	{ 0x4F, NULL, 0 },
	{ 0x50, NULL, 0 },
	{ 0x51, NULL, 0 },
	{ 0x52, NULL, 0 },
	{ 0x53, NULL, 0 },
	{ 0x54, NULL, 0 },
	{ 0x55, NULL, 0 },
	{ 0x56, NULL, 0 },
	{ 0x57, NULL, 0 },
	{ 0x58, NULL, 0 },
	{ 0x59, NULL, 0 },
	{ 0x5A, NULL, 0 },
	{ 0x5B, NULL, 0 },
	{ 0x5C, NULL, 0 },
	{ 0x5D, NULL, 0 },
	{ 0x5E, NULL, 0 },
	{ 0x5F, NULL, 0 },
	{ 0x60, "CopyMemory", 0 },
	{ 0x61, "ZeroMemory", 0 },
	{ 0x62, "FillMemory", 0 },
	{ 0x63, "CmpMemory", 0 },
	{ 0x64, NULL, 0 },
	{ 0x65, NULL, 0 },
	{ 0x66, "CmpString", 0 },
	{ 0x67, NULL, 0 },
	{ 0x68, "Strlen", 0 },
	{ 0x69, "CmpString2", 0 },
	{ 0x6A, "Strcpy", 0 },
	{ 0x6B, "ErrorMsg", 0 },
	{ 0x6C, "IsPunctuation", 0 },
	{ 0x6D, "StringUpper", 0 },
	{ 0x6E, NULL, 0 },
	{ 0x6F, "StringFormat", 0 },
	{ 0x70, NULL, 0 },
	{ 0x71, NULL, 0 },
	{ 0x72, NULL, 0 },
	{ 0x73, NULL, 0 },
	{ 0x74, "Change", 0 },
	{ 0x75, NULL, 0 },
	{ 0x76, NULL, 0 },
	{ 0x77, NULL, 0 },
	{ 0x78, "YesNoBox", 0 },
	{ 0x79, "FatalBox", 0 },
	{ 0x7A, NULL, 0 },
	{ 0x7B, NULL, 0 },
	{ 0x7C, "ModelBox", 0 },
	{ 0x7D, NULL, 0 },
	{ 0x7E, "CopyToClipBoard", 0 },
	{ 0x7F, "Debug", 2, { GetString, GetDword } },

	//SysCall ?
	//长度似乎不超过WORD
	{ 0x80, "CallFunction1", 0 },
	{ 0x81, "CallFunction2", 0 },
	{ 0x83, NULL, 0 },
	{ 0x84, NULL, 0 },
	{ 0x85, NULL, 0 },
	{ 0x86, NULL, 0 },
	{ 0x87, NULL, 0 },
	{ 0x88, NULL, 0 },
	{ 0x89, NULL, 0 },
	{ 0x8a, NULL, 0 },
	{ 0x8b, NULL, 0 },
	{ 0x8c, NULL, 0 },
	{ 0x8d, NULL, 0 },
	{ 0x8e, NULL, 0 },
	{ 0x8f, NULL, 0 },
	{ 0x90, "CallFunction3", 0 },
	{ 0x91, "CallFunction4", 0 },
	{ 0x92, "CallFunction5", 0 },
	{ 0x93, NULL, 0 },
	{ 0x94, NULL, 0 },
	{ 0x95, NULL, 0 },
	{ 0x96, NULL, 0 },
	{ 0x97, NULL, 0 },
	{ 0x98, NULL, 0 },
	{ 0x99, NULL, 0 },
	{ 0x9a, NULL, 0 },
	{ 0x9b, NULL, 0 },
	{ 0x9c, NULL, 0 },
	{ 0x9d, NULL, 0 },
	{ 0x9e, NULL, 0 },
	{ 0x9f, NULL, 0 },
	{ 0xa0, "CallFunction6", 0 },
	{ 0xa1, NULL, 0 },
	{ 0xa2, NULL, 0 },
	{ 0xa3, NULL, 0 },
	{ 0xa4, NULL, 0 },
	{ 0xa5, NULL, 0 },
	{ 0xa6, NULL, 0 },
	{ 0xa7, NULL, 0 },
	{ 0xa8, NULL, 0 },
	{ 0xa9, NULL, 0 },
	{ 0xaa, NULL, 0 },
	{ 0xab, NULL, 0 },
	{ 0xac, NULL, 0 },
	{ 0xad, NULL, 0 },
	{ 0xae, NULL, 0 },
	{ 0xaf, NULL, 0 },
	{ 0xb0, "CallFunction7", 0 },
	{ 0xb1, NULL, 0 },
	{ 0xb2, NULL, 0 },
	{ 0xb3, NULL, 0 },
	{ 0xb4, NULL, 0 },
	{ 0xb5, NULL, 0 },
	{ 0xb6, NULL, 0 },
	{ 0xb7, NULL, 0 },
	{ 0xb8, NULL, 0 },
	{ 0xb9, NULL, 0 },
	{ 0xba, NULL, 0 },
	{ 0xbb, NULL, 0 },
	{ 0xbc, NULL, 0 },
	{ 0xbd, NULL, 0 },
	{ 0xbe, NULL, 0 },
	{ 0xbf, NULL, 0 },
	{ 0xc0, "CallFunction8", 0 },
	{ 0xc1, NULL, 0 },
	{ 0xc2, NULL, 0 },
	{ 0xc3, NULL, 0 },
	{ 0xc4, NULL, 0 },
	{ 0xc5, NULL, 0 },
	{ 0xc6, NULL, 0 },
	{ 0xc7, NULL, 0 },
	{ 0xc8, NULL, 0 },
	{ 0xc9, NULL, 0 },
	{ 0xca, NULL, 0 },
	{ 0xcb, NULL, 0 },
	{ 0xcc, NULL, 0 },
	{ 0xcd, NULL, 0 },
	{ 0xce, NULL, 0 },
	{ 0xcf, NULL, 0 },
	{ 0xd0, NULL, 0 },
	{ 0xd1, NULL, 0 },
	{ 0xd2, NULL, 0 },
	{ 0xd3, NULL, 0 },
	{ 0xd4, NULL, 0 },
	{ 0xd5, NULL, 0 },
	{ 0xd6, NULL, 0 },
	{ 0xd7, NULL, 0 },
	{ 0xd8, NULL, 0 },
	{ 0xd9, NULL, 0 },
	{ 0xda, NULL, 0 },
	{ 0xdb, NULL, 0 },
	{ 0xdc, NULL, 0 },
	{ 0xdd, NULL, 0 },
	{ 0xde, NULL, 0 },
	{ 0xdf, NULL, 0 },
	{ 0xe0, NULL, 0 },
	{ 0xe1, NULL, 0 },
	{ 0xe2, NULL, 0 },
	{ 0xe3, NULL, 0 },
	{ 0xe4, NULL, 0 },
	{ 0xe5, NULL, 0 },
	{ 0xe6, NULL, 0 },
	{ 0xe7, NULL, 0 },
	{ 0xe8, NULL, 0 },
	{ 0xe9, NULL, 0 },
	{ 0xea, NULL, 0 },
	{ 0xeb, NULL, 0 },
	{ 0xec, NULL, 0 },
	{ 0xed, NULL, 0 },
	{ 0xee, NULL, 0 },
	{ 0xef, NULL, 0 },
	{ 0xf0, NULL, 0 },
	{ 0xf1, NULL, 0 },
	{ 0xf2, NULL, 0 },
	{ 0xf3, NULL, 0 },
	{ 0xf4, NULL, 0 },
	{ 0xf5, NULL, 0 },
	{ 0xf6, NULL, 0 },
	{ 0xf7, NULL, 0 },
	{ 0xf8, NULL, 0 },
	{ 0xf9, NULL, 0 },
	{ 0xfa, NULL, 0 },
	{ 0xfb, NULL, 0 },
	{ 0xfc, NULL, 0 },
	{ 0xfd, NULL, 0 },
	{ 0xfe, NULL, 0 },
	{ 0xff, NULL, 0 }
};


extern BOOL JPMode;


string FixString(string& Line)
{
	string Name;
	for (auto it : Line)
	{
		if (it == 0xA)
		{
			Name += "\\n";
		}
		else
		{
			Name += it;
		}
	}
	return Name;
}

//BGI 使用0xA表示换行
string WINAPI GetString(PBYTE BufferStart, ULONG& Offset)
{
	ULONG Value = *(PULONG)(BufferStart + Offset);
	Offset += 4;
	WCHAR WideString[1500] = { 0 };
	CHAR UTF8Name[2000] = { 0 };

	MultiByteToWideChar(JPMode ? 932 : 936, 0, (CHAR*)(BufferStart + Value), lstrlenA((CHAR*)(BufferStart + Value)), WideString, 1500);
	WideCharToMultiByte(CP_UTF8, 0, WideString, lstrlenW(WideString), UTF8Name, 2000, nullptr, nullptr);

#ifndef DUMP_TEXT_ONLY
	return  FixString("\"" + string(UTF8Name) + "\"");
#else
	return  FixString(string(UTF8Name));
#endif
}

string WINAPI GetOffset(PBYTE BufferStart, ULONG& Offset)
{
	ULONG Value = *(PULONG)(BufferStart + Offset);
	Offset += 4;
	CHAR Name[260] = { 0 };
	wsprintfA(Name, "BGI_DEF(0x%08x)", Value);
	return string(Name);
}


string WINAPI GetDword(PBYTE BufferStart, ULONG& Offset)
{
	ULONG Value = *(PULONG)(BufferStart + Offset);
	Offset += 4;
	CHAR Name[260] = { 0 };
	wsprintfA(Name, "0x%08x", Value);
	return string(Name);
}


HRESULT GetInstructionInfo(PBYTE Buffer, ULONG& iPos, string& Name)
{
	HRESULT Result = S_OK;
	ULONG Ins = *(ULONG*)(Buffer + iPos);
	iPos += 4;

#ifndef DUMP_TEXT_ONLY
	//似乎是分段的 Msg, Snd Grp Sys
	if (Ins <= 0xFF)
	{
		if (RecordInfo[LOBYTE(Ins)].Name[0] == 0)
		{
			CHAR Info[100] = { 0 };
			wsprintfA(Info, "function_%08x", Ins);
			Name = Info;
		}
		else
		{
			if (RecordInfo[LOBYTE(Ins)].ArgsCount == 0)
			{
				Name = RecordInfo[LOBYTE(Ins)].Name;
			}
			else
			{
				Name += RecordInfo[LOBYTE(Ins)].Name;
				Name += " (";
				for (ULONG Index = 0; Index < RecordInfo[LOBYTE(Ins)].ArgsCount; Index++)
				{
					Name += RecordInfo[LOBYTE(Ins)].Stub[Index](Buffer, iPos);
					Name += Index == RecordInfo[LOBYTE(Ins)].ArgsCount - 1 ? "" : ", ";
				}
				Name += ")";
			}
		}
	}
	else
	{
		CHAR Info[100] = { 0 };
		wsprintfA(Info, "function_%08x", Ins);
		Name = Info;
	}

#else

	if (Ins <= 0xFF)
	{
		if (RecordInfo[LOBYTE(Ins)].Name[0] == 0)
		{
			//CHAR Info[100] = { 0 };
			//wsprintfA(Info, "function_%08x", Ins);
			//Name = Info;
			Result = S_FALSE;
		}
		else
		{
			if (RecordInfo[LOBYTE(Ins)].ArgsCount == 0)
			{
				//Name = RecordInfo[LOBYTE(Ins)].Name;
				Result = S_FALSE;
			}
			else
			{
				if (Ins == 0x00000003UL)
				{
					//Name += RecordInfo[LOBYTE(Ins)].Name;
					for (ULONG Index = 0; Index < RecordInfo[LOBYTE(Ins)].ArgsCount; Index++)
					{
						Name += RecordInfo[LOBYTE(Ins)].Stub[Index](Buffer, iPos);
						Name += Index == RecordInfo[LOBYTE(Ins)].ArgsCount - 1 ? "" : ", ";
					}
					Result = S_OK;
				}
				else
				{
					Name += RecordInfo[LOBYTE(Ins)].Name;
					Name += " (";
					for (ULONG Index = 0; Index < RecordInfo[LOBYTE(Ins)].ArgsCount; Index++)
					{
						Name += RecordInfo[LOBYTE(Ins)].Stub[Index](Buffer, iPos);
						Name += Index == RecordInfo[LOBYTE(Ins)].ArgsCount - 1 ? "" : ", ";
					}
					Name += ")";
					Result = S_FALSE;
				}
			}
		}
	}
	else
	{
		CHAR Info[100] = { 0 };
		wsprintfA(Info, "function_%08x", Ins);
		Name = Info;
		Result = S_FALSE;
	}

#endif

	return Result;
}
