#pragma once
#include "bib.h"

//uchar c0(uchar t)
//{
//	uchar a = -0.5;
//	return (-a*t*t*t + a*t*t);
//}
//uchar c1(uchar t)
//{
//	uchar a = -0.5;
//	return (-(a + 2)*t*t*t + (2 * a + 3)*t*t - a*t);
//}
//uchar c2(uchar t)
//{
//	uchar a = -0.5;
//	return ((a + 2)*t*t*t - (a + 3)*t*t + 1);
//}
//uchar c3(uchar t)
//{
//	uchar a = -0.5;
//	return (a*t*t*t - 2 * a*t*t + a*t);
//}
//
//uchar Hr(Mat &src, int i, int j, float x, int Xmax, int Ymax)
//{
//	uchar Q;
//	if (i-1<0) { i = 1; }
//	if (i>Xmax-3) { i = Xmax-3; }
//	if (j-1<0) { j = 1; }
//	if (j>Ymax-3) { j = Ymax-3; }
//	Q = src.at<uchar>(j,i - 1) * c3(x - i) + src.at<uchar>(j,i) * c2(x - i) + src.at<uchar>(j,i + 1) * c1(x - i) + src.at<uchar>(j,i + 2) * c0(x - i);
//	return Q;
//}
////=========================================================================================
//
//uchar zoom = 2.0;
//string srcPath = "E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image1\\image1.jpg";
//
//int main()
//{
//	int Xmax, Ymax, XmaxS, YmaxS;
//	Mat src = imread(srcPath, CV_LOAD_IMAGE_GRAYSCALE);
//
//	Xmax = src.cols;
//	Ymax = src.rows;
//	
//	XmaxS = zoom*Xmax;
//	YmaxS = zoom*Ymax;
//
//	float v1;
//	float x, y, xr, yr;
//	Size srcSize(XmaxS, YmaxS);
//	Mat dst(srcSize, CV_8UC1);
//
//	for (int j = 0;j<YmaxS;j++)
//	{
//		for (int i = 0;i<XmaxS;i++)
//		{
//			x = i*float(Xmax) / XmaxS;
//			y = j*float(Ymax) / YmaxS;
//			xr = x;    // coordonnées réelles
//			yr = y;
//
//			// calcul de l'element le plus proche
//			x = floor(x);
//			y = floor(y);
//
//			if (x >= 0 && x <= Xmax - 1 && y >= 0 && y <= Ymax - 1)
//			{
//				v1 = Hr(src, x, y - 1, xr,Xmax,Ymax)*c3(yr - y) + Hr(src, x, y, xr, Xmax, Ymax)*c2(yr - y) +
//					Hr(src, x, y + 1, xr, Xmax, Ymax)*c1(yr - y) + Hr(src, x, y + 2, xr, Xmax, Ymax)*c0(yr - y);
//				dst.at<uchar>(j, i) = v1;
//				//cout << dst.at<uchar>(j, i) << "=" << v1<<"  ";
//
//			}
//			else
//			{
//				v1 = 0;
//				dst.at<uchar>(j, i) = v1;
//			}
//
//		}
//	}
//	imshow("Image src", src);
//	imshow("Image dest", dst);
//	waitKey(0);
//	return 0;
//}

#define M11  0.0    
#define M12  1.0   
#define M13  0.0   
#define M14  0.0   
#define M21 -0.5   
#define M22  0.0   
#define M23  0.5   
#define M24  0.0   
#define M31  1.0   
#define M32 -2.5   
#define M33  2.0   
#define M34 -0.5   
#define M41 -0.5   
#define M42  1.5   
#define M43 -1.5   
#define M44  0.5 

float CalCurveInt(float t, float p0, float p1, float p2, float p3)
{

	float t2 = t * t;
	float t3 = t2 * t;

	float x = 0.5f * ((2.0f * p1) +
		(-p0 + p2) * t +
		(2.0f * p0 - 5.0f * p1 + 4 * p2 - p3) * t2 +
		(-p0 + 3.0f * p1 - 3.0f * p2 + p3) * t3);

	//double c1, c2, c3, c4;

	//c1 = M12*p2;
	//c2 = M21*p1 + M23*p3;
	//c3 = M31*p1 + M32*p2 + M33*p3 + M34*p4;
	//c4 = M41*p1 + M42*p2 + M43*p3 + M44*p4;

	//double x = (((c4*t + c3)*t + c2)*t + c1);
	return x;
}

Mat CalcCatmull(Mat &src, int zoom)
{
	int v1, v2, v3, v4, Ptr, Xmax, Ymax;
	float Result, t, c1, c2, c3, c4;
	//------------------------------------------------------------
	Xmax = src.cols;
	Ymax = src.rows;
	Size srcSize(zoom*Xmax, Ymax);
	Mat dst(srcSize, CV_32FC1);

	for (int j = 0; j < Ymax; j++)
	{
		Ptr = 0;
		for (int i = 0; i < Xmax; i++)
		{
			//float rr=src.at<uchar>(j, i);
			//cout << rr << " ";
			v1 = i - 1;	v2 = i; v3 = i + 1;	v4 = i + 2;

			if (i - 1 < 0)		v1 = 0;
			if (Xmax <= i + 1)	v3 = Xmax - 1;
			if (Xmax <= i + 2)	v4 = Xmax - 1;

			for (double J = 1; J <= zoom; J++)
			{
				t = J / zoom;
				Result = 0.0;

				c1 = src.at<float>(j, v1);
				c2 = src.at<float>(j, v2);
				c3 = src.at<float>(j, v3);
				c4 = src.at<float>(j, v4);

				Result = CalCurveInt(t, c1, c2, c3, c4);

				//if (Result < 0) Result = abs(Result);
				//if (Result > 255) Result = 255;

				//cout << Result << " ";
				dst.at<float>(j, Ptr) = abs(Result);
				Ptr++;
			}
		}
		//cout << endl;
	}
	//cout << "-----------------"<<endl;
	imshow("Image dest1", dst);
	imwrite("Image dest1.png", dst);
	//------------------------------------------------
	Xmax = dst.cols;
	Ymax = dst.rows;

	Size srcSize1(Xmax, zoom*Ymax);
	Mat dest(srcSize1, CV_32FC1);

	for (int i = 0; i < Xmax; i++)
	{
		Ptr = 0;
		for (int j = 0; j < Ymax; j++)
		{

			v1 = j - 1;	v2 = j;	v3 = j + 1; v4 = j + 2;

			if (j - 1 < 0)		v1 = 0;
			if (Ymax <= j + 1)	v3 = Ymax - 1;
			if (Ymax <= j + 2)	v4 = Ymax - 1;

			for (double J = 1; J <= zoom; J++)
			{
				t = J / zoom;
				Result = 0.0;

				c1 = dst.at<float>(v1, i);
				c2 = dst.at<float>(v2, i);
				c3 = dst.at<float>(v3, i);
				c4 = dst.at<float>(v4, i);

				Result = CalCurveInt(t, c1, c2, c3, c4);

				//if (Result < 0) Result = abs(Result);
				//if (Result > 255) Result = 255;

				dest.at<float>(Ptr, i) = Result;
				//cout << Result <<" " ;
				Ptr++;
			}
		}
		//cout << endl;
	}

	return dest;
}

float zoom = 2.0;
string srcPath = "F:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image1\\image1-2.png";

int main()
{
	Mat src = imread(srcPath, 0);
	int width = src.cols;
	int hight = src.rows;

	/*Image Decimation*/
	Size srcdSize(int(width / zoom), int(hight / zoom));
	Mat srcd;
	pyrDown(src, srcd, srcdSize);
	imshow("decimation", srcd);

	Mat _src = srcd;
	_src.convertTo(_src, CV_32FC1, 1.0 / 255.0);
	Mat dst = CalcCatmull(_src, zoom);

	imshow("Image Source", src);
	imshow("Image dest", dst);
	imwrite("Image dest.png", dst);

	cout << src.cols << "  " << src.rows << endl;
	cout << dst.cols << "  " << dst.rows << endl;

	/*------------------------------- Quality metrics -------------------------------*/
	double PSNR = psnr(src, dst); 
	//cout << "PSNR= "<<PSNR << endl;
	
	//Scalar SSIM = getMSSIM(src, dst);
	//cout << "SSIM=" << SSIM << "\n";

	//double mySSIM = myssim(src, dst);
	//cout << "mySSIM=" << mySSIM << "\n";

	waitKey(0);
	return 0;
}




//
////double coefCTMR(int i, float Temps )
////{
////	double Result1;
////	switch (i)
////	{
////	case -2:
////		Result1 = ((-Temps + 2.0) * Temps - 1.0) * Temps / 2.0;
////		break;
////
////	case -1:
////		Result1 = (((3.0 * Temps - 5.0) * Temps) * Temps + 2.0) / 2.0;
////		break;
////
////	case  0:
////		Result1 = ((-3.0 * Temps + 4.0) * Temps + 1.0) * Temps / 2.0;
////		break;
////
////	case  1:
////		Result1 = ((Temps - 1.0) * Temps * Temps) / 2.0;
////		break;
////
////	default:
////		Result1 = 0.0;
////		break;
////	}
////	return Result1;
////}
//
////double CatmullRomSpline(Mat & I, int J, int c, int r)
////{
////	int Xmax = I.cols;
////	int Ymax = I.rows;
////
////	float Temps = J / zoom;
////	int i, K;
////	double Result = 0.0;
////
////	for (i = -2; i <= 1; i++)
////	{
////		K = r + i;
////		if (K<0) { K = 0; }
////		if (K >= Ymax) { K = Ymax - 1; }
////		float Result1 = coefCTMR(i, Temps);
////		Result = Result + Result1*I.at< uchar>(K, c);
////	}
////}
//
//
//
//float zoom = 2.0;
//
//int main()
//{
//	/* image path: E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\pattern3\\pattern3.jpg */
//
//	Mat src = imread("E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image1\\image1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
//	//Mat src0  = imread("E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image1\\image1-2.png", CV_LOAD_IMAGE_GRAYSCALE);
//	
//	//imshow("Image Source", src);
//	
//	int width = src.cols;
//	int hight = src.rows;
//	
//	/* ========================= Image Decimation ========================= */
//	Size srcdSize(int (width/zoom) , int (hight/zoom));
//	Mat srcd;
//	pyrDown(src, srcd, srcdSize);
//	imshow("decimation", srcd);
//	
//	int Xmax = srcd.cols;
//	int Ymax = srcd.rows;
//
//	/* ========================= Image interpolation ========================= */
//
//	/* ------------------------- Columns interpolation ----------------------- */
//	Size imageSizeX(zoom*Xmax,Ymax ); //Size(cols, rows) Rmq: mat.at<T>(row,col)
//	Mat Vx(imageSizeX, CV_8UC1);
//	
//	for (int r = 0; r < Ymax; r++)  //r:rows
//	{		
//		int PtrS = 0;
//		for (int c = 0; c < Xmax; c++) //c:columns
//		{
//			for (int J = 1; J <= zoom; J++)
//			{
//				
//				double Temps = J / zoom;
//				int i, K;
//				double Result = 0.0;
//
//				for (i = -2; i <= 1; i++)
//				{
//					K = c + i;
//					if (K<0) { K = 0; }
//					if (K >= Xmax) { K = Xmax-1; }
//					double Result1;
//					switch (i)
//					{
//					case -2:
//						Result1 = ((-Temps + 2.0) * Temps - 1.0) * Temps / 2.0;
//						break;
//
//					case -1:
//						Result1 = (((3.0 * Temps - 5.0) * Temps) * Temps + 2.0) / 2.0;
//						break;
//
//					case  0:
//						Result1 = ((-3.0 * Temps + 4.0) * Temps + 1.0) * Temps / 2.0;
//						break;
//
//					case  1:
//						Result1 = ((Temps - 1.0) * Temps * Temps) / 2.0;
//						break;
//
//					default:
//						Result1 = 0.0;
//						break;
//					}
//					//float Result1 = coefCTMR(i, Temps);
//					//cout << r << " " << K << "///";
//					Result = Result + Result1*srcd.at<uchar>(r, K);					
//				}
//				Vx.at<uchar>(r, PtrS) = Result;
//
//				PtrS++;
//			}
//		}		
//	}
//	//imshow("Columns interpolation", Vx);
//	//imwrite("Columns interpolation.png", Vx);
//
//	/* ------------------------- Rows interpolation ----------------------- */
//	Size imageSizeY(zoom*Xmax, zoom*Ymax); //Size(cols, rows) Rmq: mat.at<T>(row,col)
//	Mat dest(imageSizeY, CV_8UC1);
//
//	for (int c = 0; c < zoom*Xmax; c++)  //r:rows
//	{
//		int PtrS = 0;
//		for (int r = 0; r < Ymax; r++) //c:columns
//		{
//			for (int J = 1; J <= zoom; J++)
//			{
//
//				float Temps = J / zoom;
//				int i, K;
//				double Result = 0.0;
//
//				for (i = -2; i <= 1; i++)
//				{
//					K = r + i;
//					if (K<0) { K = 0; }
//					if (K >= Ymax) { K = Ymax - 1; }
//					double Result1;
//					switch (i)
//					{
//					case -2:
//						Result1 = ((-Temps + 2.0) * Temps - 1.0) * Temps / 2.0;
//						break;
//
//					case -1:
//						Result1 = (((3.0 * Temps - 5.0) * Temps) * Temps + 2.0) / 2.0;
//						break;
//
//					case  0:
//						Result1 = ((-3.0 * Temps + 4.0) * Temps + 1.0) * Temps / 2.0;
//						break;
//
//					case  1:
//						Result1 = ((Temps - 1.0) * Temps * Temps) / 2.0;
//						break;
//
//					default:
//						Result1 = 0.0;
//						break;
//					}
//					//float Result1 = coefCTMR(i, Temps);
//					Result = Result + Result1*Vx.at< uchar>(K, c);
//				}
//				dest.at<uchar>(PtrS, c) = Result;
//				PtrS++;
//			}
//		}
//	}
//	imshow("XY interpolation", dest);
//
//	/*------------------------------- Quality metrics -------------------------------*/
//	//double PSNR = psnr(src, dest); 
//	//cout << "PSNR= "<<PSNR << endl;
//	//
//	//Scalar SSIM = getMSSIM(src, dest);
//	//cout << "SSIM=" << SSIM << "\n";
//
//	//double mySSIM = myssim(src, dest);
//	//cout << "mySSIM=" << mySSIM << "\n";
//
//	waitKey(0);
//	return 0;
//}