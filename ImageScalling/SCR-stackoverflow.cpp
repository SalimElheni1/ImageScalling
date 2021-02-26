#include "bib.h"

//-----------------------------------------------------------------------------------------------------
// Take 2 points, compute values between p1 and p2, p0 and p3 need for tangents computation
// on the bouunds. Parameter t - changes in range 0 to 1 (0 - we are in p1, 1 - we are in p2)
//-----------------------------------------------------------------------------------------------------
void PointOnCurve(Point2f &out, float t, Point2f p0, Point2f p1, Point2f p2, Point2f p3)
{
	float t2 = t * t;
	float t3 = t2 * t;
	out.x = 0.5f * ((2.0f * p1.x) + (-p0.x + p2.x) * t +
		(2.0f * p0.x - 5.0f * p1.x + 4 * p2.x - p3.x) * t2 +
		(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
	out.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t +
		(2.0f * p0.y - 5.0f * p1.y + 4 * p2.y - p3.y) * t2 +
		(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
}
//-----------------------------------------------------------------------------------------------------
// interpolation of 4х4 patch
// 
//          S * S * S * S 
//          * * * * * * * 
//          S * S * S * S 
//          * * * * * * * 
//          S * S * S * S 
//          * * * * * * * 
//          S * S * S * S 
//  
//          S- pixels of source imgage
//
//          sequentially take 2 middle columns and computte D.
//
//          S * 1 * 2 * S
//          * * * * * * *
//          S * 1 * 2 * S
//          * * D * D * *
//          S * 1 * 2 * S
//          * * * * * * *
//          S * 1 * 2 * S
//
//          same for rows and we will have F
//
//          S * S * S * S
//          * * * * * * *
//          3 * 3 F 3 * 3
//          * * D * D * *
//          4 * 4 F 4 * 4
//          * * * * * * *
//          S * S * S * S
//
//          then compute diagonals and after averafing with neihbours will find С
//
//          1 * S * S * 2
//          * * * * * * *
//          S * 1 F 2 * S
//          * * D C D * *
//          S * 2 F 1 * S
//          * * * * * * *
//          2 * S * S * 1
//-----------------------------------------------------------------------------------------------------
void PointOnSurface(Mat& src, Mat& dst)
{
	float t = 0.5;
	Point2f out;
	dst = Mat(3, 3, CV_32FC1);
	// Угловые точки результата совпадают с точками центральной ячейки исходного патча
	dst.at<float>(0, 0) = src.at<float>(1, 1);
	dst.at<float>(2, 0) = src.at<float>(2, 1);

	dst.at<float>(0, 2) = src.at<float>(1, 2);
	dst.at<float>(2, 2) = src.at<float>(2, 2);

	Point2f p0;
	Point2f p1;
	Point2f p2;
	Point2f p3;

	p0.x = 0;p0.y = src.at<float>(0, 1);
	p1.x = 1;p1.y = src.at<float>(1, 1);
	p2.x = 2;p2.y = src.at<float>(2, 1);
	p3.x = 3;p3.y = src.at<float>(3, 1);

	PointOnCurve(out, t, p0, p1, p2, p3);
	dst.at<float>(1, 0) = out.y;

	p0.x = 0;p0.y = src.at<float>(0, 2);
	p1.x = 1;p1.y = src.at<float>(1, 2);
	p2.x = 2;p2.y = src.at<float>(2, 2);
	p3.x = 3;p3.y = src.at<float>(3, 2);

	PointOnCurve(out, t, p0, p1, p2, p3);
	dst.at<float>(1, 2) = out.y;

	p0.x = 0;p0.y = src.at<float>(1, 0);
	p1.x = 1;p1.y = src.at<float>(1, 1);
	p2.x = 2;p2.y = src.at<float>(1, 2);
	p3.x = 3;p3.y = src.at<float>(1, 3);


	PointOnCurve(out, t, p0, p1, p2, p3);
	dst.at<float>(0, 1) = out.y;

	p0.x = 0;p0.y = src.at<float>(2, 0);
	p1.x = 1;p1.y = src.at<float>(2, 1);
	p2.x = 2;p2.y = src.at<float>(2, 2);
	p3.x = 3;p3.y = src.at<float>(2, 3);

	PointOnCurve(out, t, p0, p1, p2, p3);
	dst.at<float>(2, 1) = out.y;

	// diagonals

	// 1
	p0.x = 0;p0.y = src.at<float>(0, 0);
	p1.x = 1;p1.y = src.at<float>(1, 1);
	p2.x = 2;p2.y = src.at<float>(2, 2);
	p3.x = 3;p3.y = src.at<float>(3, 3);


	PointOnCurve(out, t, p0, p1, p2, p3);
	float d1 = out.y;

	// 2
	p0.x = 0;p0.y = src.at<float>(3, 0);
	p1.x = 1;p1.y = src.at<float>(2, 1);
	p2.x = 2;p2.y = src.at<float>(1, 2);
	p3.x = 3;p3.y = src.at<float>(0, 3);

	PointOnCurve(out, t, p0, p1, p2, p3);
	float d2 = out.y;

	// averaging
	dst.at<float>(1, 1) = 1.0 / 6.0*(d1 + d2 + dst.at<float>(0, 1) + dst.at<float>(1, 0) + dst.at<float>(1, 2) + dst.at<float>(2, 1));
}
//-----------------------------------------------------------------------------------------------------
void Scale2Times(Mat& src_img, Mat &dstImg)
{
	Mat imgf/*, img*/;
	Mat dst;
	Mat src;
	int a, b;
	src_img.convertTo(imgf, CV_32FC1, 1.0 / 255.0);

	//copyMakeBorder(imgf, img, 1, 1, 1, 1, BORDER_REFLECT);

	dstImg = Mat(src_img.rows * 2, src_img.cols * 2, CV_32FC1);

	for (int i = 0;i < imgf.rows;i++)
	{
		if (i < imgf.rows - 4) a = i;
		else a = imgf.rows - 5;
		for (int j = 0;j < imgf.cols;j++)
		{
			if (j < imgf.cols - 4) b = j;
			else b = imgf.cols - 5;
			imgf(Rect(b, a, 4, 4)).copyTo(src);
			PointOnSurface(src, dst);
			dst.copyTo(dstImg(Rect(2 * j, 2 * i, 3, 3)));
		}
	}
	//dstImg = dstImg(Rect(0, 0, dstImg.cols - 2, dstImg.rows - 2)).clone();
}
//-----------------------------------------------------------------------------------------------------
string srcPath = "E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image0\\lena.png";
//-----------------------------------------------------------------------------------------------------
int main(int argc, char** argv)
{
	/*Read Image*/
	Mat Img = imread(srcPath, 0);
	namedWindow("Src");
	imshow("Src", Img);
	int width = Img.cols;
	int hight = Img.rows;

	/*Image Decimation*/
	Size srcdSize(int(width / 2), int(hight / 2));
	Mat srcd;
	pyrDown(Img, srcd, srcdSize);
	imshow("decimation", srcd);

	/*Interpolation*/
	Mat dstImg;
	Scale2Times(srcd, dstImg);
	namedWindow("Catmul-Rom");
	imshow("Catmul-Rom", dstImg);
	cout << Img.cols<<"  "<< Img.rows<<endl;

	cout << dstImg.cols << "  " << dstImg.rows << endl;

	/*------------------------------- Quality metrics -------------------------------*/
	//double PSNR = psnr(Img, dstImg);
	//cout << "PSNR= " << PSNR << endl;

	//Scalar SSIM = getMSSIM(Img, dstImg);
	//cout << "SSIM=" << SSIM << "\n";

	//double mySSIM = myssim(Img, dstImg);
	//cout << "mySSIM=" << mySSIM << "\n";

	/*Interpolation cvResize OpenCV*/
	//Mat ImgLin(srcd.rows * 2, srcd.cols * 2, CV_8UC1);
	//resize(srcd, ImgLin, Size(srcd.cols * 2, srcd.rows * 2), INTER_CUBIC);
	//namedWindow("cvResize");
	//imshow("cvResize", ImgLin);

	waitKey(0);

	return 0;
}