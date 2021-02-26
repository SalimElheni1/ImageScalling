#pragma once
#include "bib.h"

float CalCurveInt(float t, float p0, float p1, float p2, float p3)
{

	float t2 = t * t;
	float t3 = t2 * t;

	float x = 0.5f * ((2.0f * p1) +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4 * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);

	return x;
}


void CalcCatmull(Mat& src, Mat& dst,vector<float>t,float zoom)
{
	//float t = 0.5;
	float out, p0, p1, p2, p3;
	dst = Mat(zoom, zoom, CV_32FC1, 1.0 / 255.0);
	
	dst.at<float>(0, 0) = src.at<float>(1, 1);
	//dst.at<float>(2, 0) = src.at<float>(2, 1);

	//dst.at<float>(0, 2) = src.at<float>(1, 2);
	//dst.at<float>(2, 2) = src.at<float>(2, 2);
	//
	p0 = src.at<float>(0, 1);
	p1 = src.at<float>(1, 1);
	p2 = src.at<float>(2, 1);
	p3 = src.at<float>(3, 1);

	out=CalCurveInt( t[0], p0, p1, p2, p3);
	dst.at<float>(1, 0) = out;

	//p0 = src.at<float>(0, 2);
	//p1 = src.at<float>(1, 2);
	//p2 = src.at<float>(2, 2);
	//p3 = src.at<float>(3, 2);

	//out = CalCurveInt(t[0], p0, p1, p2, p3);
	//dst.at<float>(1, 2) = out;

	p0 = src.at<float>(1, 0);
	p1 = src.at<float>(1, 1);
	p2 = src.at<float>(1, 2);
	p3 = src.at<float>(1, 3);

	out=CalCurveInt(t[0], p0, p1, p2, p3);
	dst.at<float>(0, 1) = out;

	//p0 = src.at<float>(2, 0);
	//p1 = src.at<float>(2, 1);
	//p2 = src.at<float>(2, 2);
	//p3 = src.at<float>(2, 3);

	//out =CalCurveInt(t[0], p0, p1, p2, p3);
	//dst.at<float>(2, 1) = out;

	// diagonals
	p0 = src.at<float>(0, 0);
	p1 = src.at<float>(1, 1);
	p2 = src.at<float>(2, 2);
	p3 = src.at<float>(3, 3);
	
	dst.at<float>(1, 1) = CalCurveInt(t[0], p0, p1, p2, p3);
}

float zoom = 2.0;
string srcPath = "F:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image5\\image5-6.png";

int main()
{
	vector<float>temps;
	for (float i = 1.0;i <= zoom;i++)
	{
		temps.push_back(i/zoom); 
	}

	Mat src = imread(srcPath, 0);
	src.convertTo(src, CV_32FC1, 1.0 / 255.0);
	int Xmax = src.cols;
	int Ymax = src.rows;

	///*Image Decimation*/
	Size srcdSize(int(Xmax / zoom), int(Ymax / zoom));
	Mat srcd;
	pyrDown(src, srcd, srcdSize);
	imshow("decimation", srcd);

	Mat _src = srcd;
	int width = _src.cols;
	int hight = _src.rows;

	Mat _rect,dst, dstImg;

	_rect = Mat(4, 4, CV_32FC1, 1.0 / 255.0);
	dstImg = Mat(hight * zoom, width * zoom,  CV_32FC1, 1.0 / 255.0);

	int a, b;
	int r0, r1, r2, r3;
	int c0, c1, c2, c3;
	for (int j = 0;j < hight;j++)
	{
		r0 = j - 1; r1 = j; r2 = j + 1; r3 = j + 2;
		if (r0 < 0) r0 = 0; 
		if (r2 > hight-1) r2 = hight - 1;
		if (r3 > hight-1) r3 = hight - 1;

		for (int i = 0;i < width ;i++)
		{
			c0 = i - 1;c1 = i;c2 = i + 1;c3 = i + 2;
			if (c0 < 0) c0 = 0;
			if (c2 > width-1) c2 = width - 1;
			if (c3 > width-1) c3 = width - 1;

			_rect.at<float>(0, 0) = _src.at<float>(r0, c0); _rect.at<float>(0, 1) = _src.at<float>(r0, c1); _rect.at<float>(0, 2) = _src.at<float>(r0, c2); _rect.at<float>(0, 3) = _src.at<float>(r0, c3);
			_rect.at<float>(1, 0) = _src.at<float>(r1, c0); _rect.at<float>(1, 1) = _src.at<float>(r1, c1); _rect.at<float>(1, 2) = _src.at<float>(r1, c2); _rect.at<float>(1, 3) = _src.at<float>(r1, c3);
			_rect.at<float>(2, 0) = _src.at<float>(r2, c0); _rect.at<float>(2, 1) = _src.at<float>(r2, c1); _rect.at<float>(2, 2) = _src.at<float>(r2, c2); _rect.at<float>(2, 3) = _src.at<float>(r2, c3);
			_rect.at<float>(3, 0) = _src.at<float>(r3, c0); _rect.at<float>(3, 1) = _src.at<float>(r3, c1); _rect.at<float>(3, 2) = _src.at<float>(r3, c2); _rect.at<float>(3, 3) = _src.at<float>(r3, c3);

			CalcCatmull(_rect, dst, temps,zoom);

			a = zoom * i; b = zoom * j;
			if (a > zoom * width - 4) a = zoom * width - 2;
			if (b > zoom * hight - 4) b = zoom * hight - 2;

			dst.copyTo(dstImg(Rect(a, b, zoom, zoom)));
		}
	}
	
	//imshow("Image src", src);
	//imshow("Image dest", dstImg);

	cout << src.cols << " x " << zoom << " = " << dstImg.cols << endl;
	cout << src.rows << " x " << zoom << " = " << dstImg.rows << endl;

	//dstImg.convertTo(dstImg, CV_8UC3, 255);//important to imwrite!
	//imwrite("Image dest.png", dstImg);
	//imshow("Image Source", src);
	//imshow("Image dest", dst);
	//imwrite("Image dest.png", dst);
	

	/*------------------------------- Quality metrics -------------------------------*/
	double PSNR = psnr(src, dstImg);
	cout << "PSNR= " << PSNR << endl;

	Scalar SSIM = getMSSIM(src, dstImg);
	cout << "SSIM=" << SSIM << "\n";

	double mySSIM = myssim(src, dstImg);
	cout << "mySSIM=" << mySSIM << "\n";

	srcd.convertTo(srcd, CV_8UC3, 255);
	dstImg.convertTo(dstImg, CV_8UC3, 255);
	imwrite("decime.png", srcd);
	imwrite("dest.png", dstImg);

	waitKey(0);
	return 0;
}