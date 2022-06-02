#pragma once
//typedef unsigned long DWORD;
//typedef unsigned short WORD;


#pragma pack(push,1)
struct BITMAPINFOHEADER
{
	DWORD Size;
	DWORD Width;
	DWORD Height;
	WORD Planes;
	WORD BitCount;
	DWORD Compression;
	DWORD SizeImage;
	DWORD XPelsPerMeter;
	DWORD YPelsPerMeter;
	DWORD ColorUsed;
	DWORD ColorImportant;

};
#pragma pack(pop)
