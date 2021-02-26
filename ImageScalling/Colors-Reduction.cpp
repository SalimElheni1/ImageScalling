#include "bib.h"



// quantize the image to numBits 
Mat quantizeImage(const Mat& inImage, int numBits)
{
	Mat retImage = inImage.clone();

	uchar maskBit = 0xFF;

	// keep numBits as 1 and (8 - numBits) would be all 0 towards the right
	maskBit = maskBit << (8 - numBits);

	for (int j = 0; j < retImage.rows; j++)
		for (int i = 0; i < retImage.cols; i++)
		{
			uchar valVec = retImage.at< uchar>(j, i);
			valVec = valVec & maskBit;
			retImage.at< uchar>(j, i) = valVec;

			/*Vec3b valVec = retImage.at< Vec3b>(j, i);
			valVec[0] = valVec[0] & maskBit;
			valVec[1] = valVec[1] & maskBit;
			valVec[2] = valVec[2] & maskBit;
			retImage.at< Vec3b>(j, i) = valVec;*/
		}

	return retImage;
}


int main()
{
	Mat inImage;
	inImage =  imread("E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\pattern3\\pattern3.jpg", IMREAD_GRAYSCALE);
	char buffer[30];
	for (int i = 1; i <= 8; i++)
	{
		 Mat quantizedImage = quantizeImage(inImage, i);
		 sprintf(buffer, "%d Bit Image", i);
		// imshow(buffer, quantizedImage);

		 sprintf(buffer, "%d Bit Image.png", i);
		 imwrite(buffer, quantizedImage);
	}

	 waitKey(0);
	return 0;
}