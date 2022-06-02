#pragma once
#include "Bitmapfileheader.h"
#include "Bitmapinfoheader.h"
#include "Rgbquad.h"
#include "Rgbtripple.h"
#define _CRT_SECURE_NO_WARNINGS

class Image
{
	BITMAPINFOHEADER bmInfoHeader;
	RGBTRIPLE* rgbTriple;
	BITMAPFILEHEADER bmFileHeader;
	RGBQUAD* palette;
	BYTE* raw;

	bool isPalette(DWORD bitCount)
	{
		if (bitCount == 1 || bitCount == 4 || bitCount == 8)
		{
			return true;
		}
		return false;
	}

	uint32_t width_size()
	{
		auto width = bmInfoHeader.Width;
		while (width % 4 != 0)
		{
			width++;
		}
		return width;
	}

	int leftBits(uint16_t width, uint8_t bit) {

		uint16_t left = getImageRowSize(width, bit) % 4;

		return  left ? 4 - left : 0;

	}

	/*void copyImage(const Image& image)
	{

		bmFileHeader = image.bmFileHeader;
		bmInfoHeader = image.bmInfoHeader;




	}*/


	uint16_t getImageRowSize(uint16_t width, uint8_t bit)
	{
		return (width * bit + 7) / 8;
	}

	uint16_t getImageSize(uint16_t width, uint16_t  height, uint8_t bit)
	{
		return (getImageRowSize(bmInfoHeader.Width, bmInfoHeader.BitCount) + leftBits(width, bit)) * height;
	}


	uint16_t color_transf(RGBTRIPLE cur_tripple)
	{
		uint16_t color = 0.299 * cur_tripple.Red + 0.597 * cur_tripple.Green + 0.114 * cur_tripple.Blue;
		if (color > 255)
			color = 255;
		return color;
	}

	uint16_t find_index(uint16_t color, uint16_t palette_size)
	{
		for (int i = 0; i < palette_size; ++i)
		{
			if (palette[i].Red >= color)
			{
				return i;
			}
		}
		return 0;
	}

	void without_palette_to_palette(uint16_t depth)
	{
		delete palette;

		bmInfoHeader.BitCount = depth;
		bmInfoHeader.SizeImage = bmInfoHeader.Width * bmInfoHeader.Height * depth;
		const auto palette_size = pow(2, depth);
		palette = new RGBQUAD[palette_size];

		uint8_t stepInColors = 256 / (palette_size - 1);

		for (uint16_t i = 0; i < palette_size; ++i)
		{
			palette[i].Red = i * stepInColors;
			palette[i].Green = i * stepInColors;
			palette[i].Blue = i * stepInColors;
			palette[i].Reserved = 0;
		}

		const uint32_t raw_size = (bmInfoHeader.Height * width_size()) / (8 / bmInfoHeader.BitCount);
		delete raw;
		raw = new BYTE[raw_size];

		int img_size = bmInfoHeader.Width * bmInfoHeader.Height;


		const uint8_t indexOffset = 8 - bmInfoHeader.BitCount;//4
		uint32_t rawCount = 0;

		for (int i = 0; i < bmInfoHeader.Height; i++)
		{

			uint8_t curIndexOffset = 0;//4
			uint8_t leftBits = 8;
			uint8_t indexByte = 0;

			for (int j = 0; j < bmInfoHeader.Width; ++j)
			{
				uint16_t color = color_transf(rgbTriple[i * bmInfoHeader.Width + j]);
				uint16_t curColorIndex = find_index(color, palette_size) << curIndexOffset;
				indexByte |= curColorIndex;

				curIndexOffset += bmInfoHeader.BitCount;//0

				leftBits -= bmInfoHeader.BitCount;//4

				if (leftBits == 0)
				{
					raw[rawCount] = indexByte;
					rawCount++;
					curIndexOffset = 0;
					leftBits = 8;
					indexByte = 0;
				}


			}
		}

		delete rgbTriple;
	}






public:

	Image(char mode, unsigned short bCount, int width, int height)
	{

		bmInfoHeader.Size = 40;
		bmInfoHeader.Width = width;
		bmInfoHeader.Height = height;
		bmInfoHeader.Planes = 1;
		bmInfoHeader.BitCount = bCount;
		bmInfoHeader.Compression = 0;
		bmInfoHeader.SizeImage = bCount * width * height;
		bmInfoHeader.XPelsPerMeter = 1920;
		bmInfoHeader.YPelsPerMeter = 1080;
		bmInfoHeader.ColorUsed = 0;
		bmInfoHeader.ColorImportant = 0;

		if (isPalette(bmInfoHeader.BitCount))
		{
			bmInfoHeader.ColorUsed = pow(2, bmInfoHeader.BitCount);
			palette = new RGBQUAD[bmInfoHeader.ColorUsed];
			for (int i = 0; i < bmInfoHeader.ColorUsed; i++)
			{
				palette[i].Red = i;
				palette[i].Green = i;
				palette[i].Blue = i;
				palette[i].Reserved = 0;
			}

		}

		rgbTriple = new RGBTRIPLE[bmInfoHeader.Width * bmInfoHeader.Height];

		for (int i = 0; i < bmInfoHeader.Height; i++)
		{
			for (int j = 0; j < bmInfoHeader.Width; ++j)
			{
				rgbTriple[i * width + j].Red = mode;
				rgbTriple[i * width + j].Green = mode;
				rgbTriple[i * width + j].Blue = mode;
			}
		}

	}



	Image(char* fileName)
	{
		FILE* f;
		f = fopen(fileName, "r");
		fread(&bmFileHeader, sizeof(BITMAPFILEHEADER), 1, f);

		if (f == NULL) return; //отписать комментарии

		if (bmFileHeader.Type != 0x4D42) //отписать комментарии
		{
			fclose(f);
			return;
		}

		fread(&bmInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);

		uint16_t width_4 = width_size();//16
		uint16_t diff = width_4 - bmInfoHeader.Width;//3


		if (isPalette(bmInfoHeader.BitCount))
		{
			uint16_t palette_size = pow(2, bmInfoHeader.BitCount);

			palette = new RGBQUAD[palette_size];
			for (int i = 0; i < palette_size; ++i)
			{
				fread(&(palette[i].Red), 1, 1, f);
				fread(&(palette[i].Green), 1, 1, f);
				fread(&(palette[i].Blue), 1, 1, f);
				fread(&(palette[i].Reserved), 1, 1, f);
			}

			fseek(f, bmFileHeader.OffsetBits, SEEK_SET);

			uint64_t raw_size = bmInfoHeader.Height * bmInfoHeader.Width;

			raw = new BYTE[raw_size];

			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{
					fread(&(raw[i * bmInfoHeader.Width + j]), 1, 1, f);
				}
				fseek(f, diff, SEEK_CUR);
			}


		}
		else {

			fseek(f, bmFileHeader.OffsetBits, SEEK_SET);


			rgbTriple = new RGBTRIPLE[bmInfoHeader.Width * bmInfoHeader.Height];

			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{


					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Red), 1, 1, f);
					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Green), 1, 1, f);
					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Blue), 1, 1, f);
				}
				fseek(f, diff, SEEK_CUR);
			}
		}


		fclose(f);

	}

	Image()
	{
		rgbTriple = NULL;

		bmInfoHeader.Size = NULL;
		bmInfoHeader.Width = NULL;
		bmInfoHeader.Height = NULL;
		bmInfoHeader.Planes = NULL;
		bmInfoHeader.BitCount = NULL;
		bmInfoHeader.Compression = NULL;
		bmInfoHeader.SizeImage = NULL;
		bmInfoHeader.XPelsPerMeter = NULL;
		bmInfoHeader.YPelsPerMeter = NULL;
		bmInfoHeader.ColorUsed = NULL;
		bmInfoHeader.ColorImportant = NULL;

		bmFileHeader.Type = NULL;
		bmFileHeader.Size = NULL;
		bmFileHeader.Reserved1 = NULL;
		bmFileHeader.Reserved2 = NULL;
		bmFileHeader.OffsetBits = NULL;


	}

	/*Image(const Image& i)
	{
		copyImage(i);
	}*/



	int loadImage(char* fileName)
	{
		FILE* f;
		f = fopen(fileName, "r");
		fread(&bmFileHeader, sizeof(BITMAPFILEHEADER), 1, f);

		if (f == NULL) return 0; //отписать комментарии

		if (bmFileHeader.Type != 0x4D42) //отписать комментарии
		{
			fclose(f);
			return 0;
		}

		fread(&bmInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);

		fseek(f, bmFileHeader.OffsetBits, SEEK_SET);

		if (isPalette(bmInfoHeader.BitCount))
		{
			uint16_t palette_size = pow(2, bmInfoHeader.BitCount);

			palette = new RGBQUAD[palette_size];
			fread(palette, sizeof(RGBQUAD), palette_size, f);
		}
		else {

			rgbTriple = new RGBTRIPLE[bmInfoHeader.Width * bmInfoHeader.Height];

			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{
					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Red), 1, 1, f);
					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Green), 1, 1, f);
					fread(&(rgbTriple[i * bmInfoHeader.Width + j].Blue), 1, 1, f);
				}
			}
		}

		fclose(f);
		return 0;
	}
	void writeImage(char* fileName)
	{
		FILE* f;
		f = fopen(fileName, "w+b");
		if (f == NULL)
		{
			return;
		}

		bmFileHeader.OffsetBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bmFileHeader.Size = bmFileHeader.OffsetBits + getImageSize(bmInfoHeader.Width, bmInfoHeader.Height, bmInfoHeader.BitCount);
		bmInfoHeader.ColorUsed = pow(2, bmInfoHeader.BitCount);
		bmFileHeader.Type = 0x4D42;



		uint64_t width_4 = width_size();
		uint16_t diff = width_4 - bmInfoHeader.Width;
		uint16_t* buff = new uint16_t[1];
		buff[0] = 0;


		if (isPalette(bmInfoHeader.BitCount))
		{

			const int p_size = sizeof(RGBQUAD) * bmInfoHeader.ColorUsed;

			bmFileHeader.Size += p_size; // добавляем его к общему размеру файла

			bmFileHeader.OffsetBits += p_size; // и к смещению до данных изображения
			fwrite(&bmFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
			fwrite(&bmInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);


			uint16_t palette_size = pow(2, bmInfoHeader.BitCount);


			for (int i = 0; i < palette_size; ++i)
			{
				fwrite(&(palette[i].Red), 1, 1, f);
				fwrite(&(palette[i].Green), 1, 1, f);
				fwrite(&(palette[i].Blue), 1, 1, f);
				fwrite(&(palette[i].Reserved), 1, 1, f);
			}

			uint8_t threshold = (8 / bmInfoHeader.BitCount) - 1;
			uint8_t step = 0;
			uint32_t rawCount = 0;

			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{

					if (step >= threshold) {
						fwrite(&(raw[rawCount]), 1, 1, f);
						rawCount++;
						//std::cout << rawCount << std::endl;
						step = 0;
					}
					else
					{
						step++;
					}
				}
				for (int k = 0; k < diff; ++k)
				{
					fwrite(buff, 1, 1, f);
				}
			}


		}
		else {
			fwrite(&bmFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
			fwrite(&bmInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);


			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{
					fwrite(&(rgbTriple[i * bmInfoHeader.Width + j].Red), 1, 1, f);
					fwrite(&(rgbTriple[i * bmInfoHeader.Width + j].Green), 1, 1, f);
					fwrite(&(rgbTriple[i * bmInfoHeader.Width + j].Blue), 1, 1, f);
				}
				for (int k = 0; k < diff; ++k)
				{

					fwrite(buff, 1, 1, f);
				}
			}
		}

		fclose(f);

	}

	/*Image operator = (Image image)
	{
		if (bmInfoHeader.Size == 0)
		{
			copyImage(image);
		}
		else if (bmInfoHeader.Width == image.bmInfoHeader.Width && bmInfoHeader.Height == image.bmInfoHeader.Height && bmInfoHeader.BitCount == image.bmInfoHeader.BitCount)
		{
			for (int i = 0; i < bmInfoHeader.Height; i++)
			{
				for (int j = 0; j < bmInfoHeader.Width; ++j)
				{
					rgbTriple[i * bmInfoHeader.Width + j].Red = image.rgbTriple[i * bmInfoHeader.Width + j].Red;
					rgbTriple[i * bmInfoHeader.Width + j].Green = image.rgbTriple[i * bmInfoHeader.Width + j].Green;
					rgbTriple[i * bmInfoHeader.Width + j].Blue = image.rgbTriple[i * bmInfoHeader.Width + j].Blue;
					raw[i * bmInfoHeader.Width + j] = image.raw[i * bmInfoHeader.Width + j];
				}
			}

		}
		return *this;

	}*/

	void changeScale(Image image)
	{
		float xRatio = (float)image.bmInfoHeader.Width / bmInfoHeader.Width;
		float yRatio = (float)image.bmInfoHeader.Height / bmInfoHeader.Height;

		for (int i = 0; i < bmInfoHeader.Height; i++)
		{
			for (int j = 0; j < bmInfoHeader.Width; ++j)
			{
				int x = (int)(j * xRatio);
				int y = (int)(i * yRatio);


				rgbTriple[i * bmInfoHeader.Width + j] = image.rgbTriple[y * bmInfoHeader.Width + x];
			}
		}


	}

	void depthChange(uint16_t depth)
	{


		if (depth != 8 && depth != 4 && depth != 1 && depth != 24 && depth != 32)
		{
			throw std::invalid_argument("wrong depth");
		}


		if (!isPalette(bmInfoHeader.BitCount) && (depth == 8 || depth == 4 || depth == 1))
		{
			without_palette_to_palette(depth);
		}


	}

	/*~Image()
	{
		if (rgbTriple) delete (rgbTriple);

	}*/
};

