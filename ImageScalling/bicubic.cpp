#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdint.h>
#include <array>
#include <vector>
#include <windows.h>  // for bitmap headers.  Sorry non windows people!

#define CLAMP(v, min, max) if (v < min) { v = min; } else if (v > max) { v = max; } 

typedef uint8_t uint8;

struct SImageData
{
	SImageData()
		: m_width(0)
		, m_height(0)
	{ }

	long m_width;
	long m_height;
	long m_pitch;
	std::vector<uint8> m_pixels;
};

void WaitForEnter()
{
	printf("Press Enter to quit");
	fflush(stdin);
	getchar();
}

bool LoadImage(const char *fileName, SImageData& imageData)
{
	// open the file if we can
	FILE *file;
	file = fopen(fileName, "rb");
	if (!file)
		return false;

	// read the headers if we can
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER infoHeader;
	if (fread(&header, sizeof(header), 1, file) != 1 ||
		fread(&infoHeader, sizeof(infoHeader), 1, file) != 1 ||
		header.bfType != 0x4D42 || infoHeader.biBitCount != 24)
	{
		fclose(file);
		return false;
	}

	// read in our pixel data if we can. Note that it's in BGR order, and width is padded to the next power of 4
	imageData.m_pixels.resize(infoHeader.biSizeImage);
	fseek(file, header.bfOffBits, SEEK_SET);
	if (fread(&imageData.m_pixels[0], imageData.m_pixels.size(), 1, file) != 1)
	{
		fclose(file);
		return false;
	}

	imageData.m_width = infoHeader.biWidth;
	imageData.m_height = infoHeader.biHeight;

	imageData.m_pitch = imageData.m_width * 3;
	if (imageData.m_pitch & 3)
	{
		imageData.m_pitch &= ~3;
		imageData.m_pitch += 4;
	}

	fclose(file);
	return true;
}

bool SaveImage(const char *fileName, const SImageData &image)
{
	// open the file if we can
	FILE *file;
	file = fopen(fileName, "wb");
	if (!file)
		return false;

	// make the header info
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER infoHeader;

	header.bfType = 0x4D42;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = 54;

	infoHeader.biSize = 40;
	infoHeader.biWidth = image.m_width;
	infoHeader.biHeight = image.m_height;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 24;
	infoHeader.biCompression = 0;
	infoHeader.biSizeImage = image.m_pixels.size();
	infoHeader.biXPelsPerMeter = 0;
	infoHeader.biYPelsPerMeter = 0;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	header.bfSize = infoHeader.biSizeImage + header.bfOffBits;

	// write the data and close the file
	fwrite(&header, sizeof(header), 1, file);
	fwrite(&infoHeader, sizeof(infoHeader), 1, file);
	fwrite(&image.m_pixels[0], infoHeader.biSizeImage, 1, file);
	fclose(file);
	return true;
}

// t is a value that goes from 0 to 1 to interpolate in a C1 continuous way across uniformly sampled data points.
// when t is 0, this will return B.  When t is 1, this will return C.  Inbetween values will return an interpolation
// between B and C.  A and B are used to calculate slopes at the edges.
float CubicHermite(float A, float B, float C, float D, float t)
{
	float a = -A / 2.0f + (3.0f*B) / 2.0f - (3.0f*C) / 2.0f + D / 2.0f;
	float b = A - (5.0f*B) / 2.0f + 2.0f*C - D / 2.0f;
	float c = -A / 2.0f + C / 2.0f;
	float d = B;

	return a*t*t*t + b*t*t + c*t + d;
}

float Lerp(float A, float B, float t)
{
	return A * (1.0f - t) + B * t;
}

const uint8* GetPixelClamped(const SImageData& image, int x, int y)
{
	CLAMP(x, 0, image.m_width - 1);
	CLAMP(y, 0, image.m_height - 1);
	return &image.m_pixels[(y * image.m_pitch) + x * 3];
}

std::array<uint8, 3> SampleNearest(const SImageData& image, float u, float v)
{
	// calculate coordinates
	int xint = int(u * image.m_width);
	int yint = int(v * image.m_height);

	// return pixel
	auto pixel = GetPixelClamped(image, xint, yint);
	std::array<uint8, 3> ret;
	ret[0] = pixel[0];
	ret[1] = pixel[1];
	ret[2] = pixel[2];
	return ret;
}

std::array<uint8, 3> SampleLinear(const SImageData& image, float u, float v)
{
	// calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
	float x = (u * image.m_width) - 0.5f;
	int xint = int(x);
	float xfract = x - floor(x);

	float y = (v * image.m_height) - 0.5f;
	int yint = int(y);
	float yfract = y - floor(y);

	// get pixels
	auto p00 = GetPixelClamped(image, xint + 0, yint + 0);
	auto p10 = GetPixelClamped(image, xint + 1, yint + 0);
	auto p01 = GetPixelClamped(image, xint + 0, yint + 1);
	auto p11 = GetPixelClamped(image, xint + 1, yint + 1);

	// interpolate bi-linearly!
	std::array<uint8, 3> ret;
	for (int i = 0; i < 3; ++i)
	{
		float col0 = Lerp(p00[i], p10[i], xfract);
		float col1 = Lerp(p01[i], p11[i], xfract);
		float value = Lerp(col0, col1, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = uint8(value);
	}
	return ret;
}

std::array<uint8, 3> SampleBicubic(const SImageData& image, float u, float v)
{
	// calculate coordinates -> also need to offset by half a pixel to keep image from shifting down and left half a pixel
	float x = (u * image.m_width) - 0.5;
	int xint = int(x);
	float xfract = x - floor(x);

	float y = (v * image.m_height) - 0.5;
	int yint = int(y);
	float yfract = y - floor(y);

	// 1st row
	auto p00 = GetPixelClamped(image, xint - 1, yint - 1);
	auto p10 = GetPixelClamped(image, xint + 0, yint - 1);
	auto p20 = GetPixelClamped(image, xint + 1, yint - 1);
	auto p30 = GetPixelClamped(image, xint + 2, yint - 1);

	// 2nd row
	auto p01 = GetPixelClamped(image, xint - 1, yint + 0);
	auto p11 = GetPixelClamped(image, xint + 0, yint + 0);
	auto p21 = GetPixelClamped(image, xint + 1, yint + 0);
	auto p31 = GetPixelClamped(image, xint + 2, yint + 0);

	// 3rd row
	auto p02 = GetPixelClamped(image, xint - 1, yint + 1);
	auto p12 = GetPixelClamped(image, xint + 0, yint + 1);
	auto p22 = GetPixelClamped(image, xint + 1, yint + 1);
	auto p32 = GetPixelClamped(image, xint + 2, yint + 1);

	// 4th row
	auto p03 = GetPixelClamped(image, xint - 1, yint + 2);
	auto p13 = GetPixelClamped(image, xint + 0, yint + 2);
	auto p23 = GetPixelClamped(image, xint + 1, yint + 2);
	auto p33 = GetPixelClamped(image, xint + 2, yint + 2);

	// interpolate bi-cubically!
	// Clamp the values since the curve can put the value below 0 or above 255
	std::array<uint8, 3> ret;
	for (int i = 0; i < 3; ++i)
	{
		float col0 = CubicHermite(p00[i], p10[i], p20[i], p30[i], xfract);
		float col1 = CubicHermite(p01[i], p11[i], p21[i], p31[i], xfract);
		float col2 = CubicHermite(p02[i], p12[i], p22[i], p32[i], xfract);
		float col3 = CubicHermite(p03[i], p13[i], p23[i], p33[i], xfract);
		float value = CubicHermite(col0, col1, col2, col3, yfract);
		CLAMP(value, 0.0f, 255.0f);
		ret[i] = uint8(value);
	}
	return ret;
}

void ResizeImage(const SImageData &srcImage, SImageData &destImage, float scale, int degree)
{
	destImage.m_width = long(float(srcImage.m_width)*scale);
	destImage.m_height = long(float(srcImage.m_height)*scale);
	destImage.m_pitch = destImage.m_width * 3;
	if (destImage.m_pitch & 3)
	{
		destImage.m_pitch &= ~3;
		destImage.m_pitch += 4;
	}
	destImage.m_pixels.resize(destImage.m_pitch*destImage.m_height);

	uint8 *row = &destImage.m_pixels[0];
	for (int y = 0; y < destImage.m_height; ++y)
	{
		uint8 *destPixel = row;
		float v = float(y) / float(destImage.m_height - 1);
		for (int x = 0; x < destImage.m_width; ++x)
		{
			float u = float(x) / float(destImage.m_width - 1);
			std::array<uint8, 3> sample;

			if (degree == 0)
				sample = SampleNearest(srcImage, u, v);
			else if (degree == 1)
				sample = SampleLinear(srcImage, u, v);
			else if (degree == 2)
				sample = SampleBicubic(srcImage, u, v);

			destPixel[0] = sample[0];
			destPixel[1] = sample[1];
			destPixel[2] = sample[2];
			destPixel += 3;
		}
		row += destImage.m_pitch;
	}
}

int main(int argc, char **argv)
{
	float scale = 1.0f;
	int degree = 0;

	/*bool showUsage = argc < 5 ||
		(sscanf(argv[3], "%f", &scale) != 1) ||
		(sscanf(argv[4], "%i", &degree) != 1);

	char *srcFileName = argv[1];
	char *destFileName = argv[2];

	if (showUsage)
	{
		printf("Usage: <source> <dest> <scale> <degree>ndegree 0 = nearest, 1 = bilinear, 2 = bicubic.nn");
		WaitForEnter();
		return 1;
	}
*/
	/*printf("Attempting to resize a 24 bit image.n");
	printf("  Source = %sn  Dest = %sn  Scale = %0.2fnn", srcFileName, destFileName, scale);*/

	SImageData srcImage;SImageData destImage;
	LoadImage("img1.bmp", srcImage);
	ResizeImage(srcImage, destImage, 2, 1);
	SaveImage("img2.bmp", destImage);
	/*if (LoadImage(srcFileName, srcImage))
	{
		printf("%s loadedn", srcFileName);
		SImageData destImage;
		ResizeImage(srcImage, destImage, scale, degree);
		if (SaveImage(destFileName, destImage))
			printf("Resized image saved as %sn", destFileName);
		else
			printf("Could not save resized image as %sn", destFileName);
	}
	else
		printf("could not read 24 bit bmp file %snn", srcFileName);*/
	//system('pause');
	return 0;
}