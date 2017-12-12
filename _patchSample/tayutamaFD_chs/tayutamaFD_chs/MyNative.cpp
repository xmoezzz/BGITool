#include "MyNative.h"

Bool IsShiftJISChar(SizeT uChar)
{
	Byte LowByte, HighByte;

	LowByte = LOBYTE(uChar);
	HighByte = HIBYTE(uChar);
	if ((HighByte >= 0x80 && HighByte <= 0x9F) ||
		(HighByte >= 0xE0 && HighByte <= 0xFC))
	{
		if ((LowByte >= 0x40 && LowByte <= 0x7E) ||
			(LowByte >= 0x80 && LowByte <= 0xFC))
		{
			return True;
		}
	}
	/*
	else if (HighByte >= 0xA1 && HighByte <= 0xDF)
	return True;
	*/
	return False;
}

Bool IsShiftJISString(PCChar pString, SizeT Length)
{
	Bool   bMBChar;
	PCChar p;

	if (pString == NULL)
		return False;

	bMBChar = False;
	p = pString;
	for (; Length; ++p, --Length)
	{
		Char  c;
		Int32 ch;

		c = *p;
		CONTINUE_IF(c > 0);
		BREAK_IF(c == 0 || --Length == 0);

		ch = *(PUInt16)p++;
		if (!IsShiftJISChar(SWAPCHAR(ch)))
			return False;
		else
			bMBChar = True;
	}

	return bMBChar;
}

