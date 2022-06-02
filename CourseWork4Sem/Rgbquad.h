#pragma once
typedef unsigned char BYTE;

#pragma pack(push,1)
struct RGBQUAD
{
	BYTE Blue;
	BYTE Green;
	BYTE Red;
	BYTE Reserved;
};
#pragma pack(pop)