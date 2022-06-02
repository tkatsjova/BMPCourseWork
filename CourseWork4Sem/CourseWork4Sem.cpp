
#include <iostream>
#include <vector>

#include "Image.h"

using namespace std;
void readWrite();

int main()
{
	//void readWrite();

	std::string str2 = "C:\\Test\\2x8_24.bmp";
	char* cstr2 = new char[str2.length() + 1];
	strcpy(cstr2, str2.c_str());

	Image* img1 = new Image(cstr2);
	delete[] cstr2;

	Image *img2 = new Image(0, 24, 6, 8);
	//Image img2;
	//(img1->depthChange (1) );
	img2->changeScale(*img1);

	std::string str = "C:\\Test\\2x8_24_scale.bmp";
	char* cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	img2->writeImage(cstr);
	// do stuff
	delete[] cstr;
	
	
}

void readWrite()
{
	std::string str2 = "C:\\Test\\5x8_256.bmp";
	char* cstr2 = new char[str2.length() + 1];
	strcpy(cstr2, str2.c_str());

	Image* img1 = new Image(cstr2);
	delete[] cstr2;






	std::string str = "C:\\Test\\5x8_256test.bmp";
	char* cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	img1->writeImage(cstr);
	// do stuff
	delete[] cstr;
}

