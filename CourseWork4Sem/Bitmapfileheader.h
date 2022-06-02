#pragma once

typedef unsigned long       DWORD;
typedef unsigned short      WORD;


#pragma pack(push,1)
struct  BITMAPFILEHEADER
{
	WORD Type;
	DWORD Size;
	WORD Reserved1;
	WORD Reserved2;
	DWORD OffsetBits;

};
#pragma pack(pop)
