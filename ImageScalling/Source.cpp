#include "bib.h"


IplImage* image;
IplImage* imagetr;
IplImage* images;

float Resolution = 2;
//int _Ne;
//int _Ns;
//float _Nr;
double _Ve[3000];
double  Vs[3000][3000];


//=========================================================================================

int _tmain(int argc, _TCHAR* argv[])
{
	// ---------------Ouvre un fichier image--------------------------
	IplImage* img = cvLoadImage("E:\\these_doctorat_2014\\DVD_publication\\Papier-Fakhri-salim\\Images\\image2\\image2.jpg");	
	if (!img)
	{cout << "L'image est introuvable ou incorrect\n";}
	cvNamedWindow("Image");
	cvShowImage("Image", img);

	//-----------------------Image en niveau de gris----------------
	image = cvCreateImage(cvGetSize(img), img->depth, 1);
	cvCvtColor(img, image, CV_RGB2GRAY);
	//cvNamedWindow("image en niv de gris");
	//cvShowImage("image en niv de gris", image);
	CvSize sizeimage = cvSize(image->width, image->height);
	int Xmax = sizeimage.width;
	int Ymax = sizeimage.height;
	cout << "width img =" << Xmax << "\n";
	cout << "height img =" << Ymax << "\n";

	//-----------------------conversion image en matrice-------------------//
	int i, j;
	double **Ve;
	Ve = new double*[Ymax];
	CvScalar pixel;
	for (j = 0;j <= Ymax - 1;j++)
	{
		Ve[j] = new double[Xmax];
		for (i = 0;i <= Xmax - 1;i++)
		{
			pixel = cvGet2D(image, j, i);
			Ve[j][i] = pixel.val[0];
		}
	}

	//------------------------redemensiner l'image---------------------//
	int m, n;
	float **l;
	l = new float*[Ymax / 2];
	for (m = 0;m <= (Ymax / 2) - 1;m++)
	{
		l[m] = new float[Xmax / 2];
	}
	n = 0;
	for (i = 0;i <= Xmax - 2;i = i + 2)
	{
		m = 0;
		for (j = 0;j <= Ymax - 2;j = j + 2)
		{
			l[m][n] = Ve[j][i];
			m = m + 1;
		}
		n = n + 1;
	}

	/*images = cvCreateImage(cvSize(Xmax/2,Ymax/2),image-> depth,1);
	CvScalar pixels;

	for(i=0;i<=(Xmax/2)-1;i++)
	{
	for(j=0;j<=(Ymax/2)-1;j++)
	{
	pixels.val[0]= l[j][i];
	cvSet2D(images,j,i,pixels);
	}
	}
	cvNamedWindow("image transformée",CV_WINDOW_AUTOSIZE);
	cvShowImage("image transformée",images);*/


	//=================================INTERPOLATION==================================//
	//................................................................................
	m = 0;
	int I, J, p, PtrS;
	double CourbePt = 0;

	//_Ne = Xmax + 1;
	//_Nr = Resolution;
	//_Ns = (_Ne - 1)*_Nr;

	images = cvCreateImage(cvSize(Resolution*Ymax / 2, Resolution*Xmax / 2), image->depth, 1);
	CvScalar pixels;

	//----------------------------INTERPOLATION SUIVANT X----------------------------------//
	////............................................................................
	//clock_t start, end;
	//start = clock();
	//............................................................................
	for (m = 0;m <= Ymax / 2 - 1;m++)
	{
		for (p = 0;p <= Xmax / 2 - 1;p++)
		{
			_Ve[p] = l[m][p];
		}
		PtrS = 0;
		for (I = 1;I <= (Xmax / 2 - 1);I++)
		{
			for (J = 1;J <= Resolution;J++)
			{
				float Temps = J / Resolution;
				int i, K;
				double Result = 0.0;

				for (int i = -2;i <= 1;i++)
				{
					K = I + i;
					if (K<0) { K = 0; }
					if (K >= Xmax/2) { K = Xmax/2 - 1; }
					double Result1;
					switch (i)
					{
					case -2:
						Result1 = ((-Temps + 2.0) * Temps - 1.0) * Temps / 2.0;
						break;

					case -1:
						Result1 = (((3.0 * Temps - 5.0) * Temps) * Temps + 2.0) / 2.0;
						break;

					case  0:
						Result1 = ((-3.0 * Temps + 4.0) * Temps + 1.0) * Temps / 2.0;
						break;

					case  1:
						Result1 = ((Temps - 1.0) * Temps * Temps) / 2.0;
						break;

					default:
						Result1 = 0.0;
						break;
					}
					Result = (Result + Result1*_Ve[K]);
				}
				Vs[m][PtrS] = Result;
				//pixel.val[0] = Result;
				//cvSet2D(images, PtrS, m, pixel);
				PtrS = PtrS + 1;
			}
		}
	}
	//---------------------------INTERPOLATION SUIVANT Y---------------------------------------//
	PtrS = 0;
	for (m = 0;m <= Resolution*Xmax / 2 - 1;m++)
	{
		for (p = 0;p <= Ymax / 2 - 1;p++)
		{
			_Ve[p] = Vs[p][m];
		}
		PtrS = 0;
		for (I = 1;I <= (Ymax / 2 - 1);I++)
		{
			for (J = 1;J <= Resolution;J++)
			{
				float Temps = J / Resolution;
				int i, K;
				double Result = 0.0;

				for (int i = -2;i <= 1;i++)
				{
					K = I + i;
					if (K<0) { K = 0; }
					if (K >= Ymax/2) { K = Ymax/2 - 1; }
					double Result1;
					switch (i)
					{
					case -2:
						Result1 = ((-Temps + 2.0) * Temps - 1.0) * Temps / 2.0;
						break;

					case -1:
						Result1 = (((3.0 * Temps - 5.0) * Temps) * Temps + 2.0) / 2.0;
						break;

					case  0:
						Result1 = ((-3.0 * Temps + 4.0) * Temps + 1.0) * Temps / 2.0;
						break;

					case  1:
						Result1 = ((Temps - 1.0) * Temps * Temps) / 2.0;
						break;

					default:
						Result1 = 0.0;
						break;
					}
					Result = (Result + Result1*_Ve[K]);
				}
				pixel.val[0] = Result;
				cvSet2D(images, PtrS, m, pixel);
				PtrS = PtrS + 1;
			}
		}
	}
	//...............................................................................................................................................
	//end = clock();
	//printf("Temps en secondes : %f\n", (end - start) / (double)CLOCKS_PER_SEC);
	////................................................................................................................................................

	//=========================Affichage de l'image interpoler============================//
	cvNamedWindow("image interpolée", CV_WINDOW_AUTOSIZE);
	cvShowImage("image interpolée", images);
//	cvSaveImage("image interpolée.BMP", images);

	////============transformation de l'image interpoler en matrice=========//
	//float **t1;
	//t1 = new float*[Ymax];
	//for (j = 0;j <= Ymax - 1;j++)
	//{
	//	t1[j] = new float[Xmax];
	//	for (i = 0;i <= Xmax - 1;i++)
	//	{
	//		pixel = cvGet2D(images, j, i);
	//		t1[j][i] = pixel.val[0];
	//	}
	//}

	////=========================PSNR===============================//
	//double error, pix, x, psnr;
	//error = 0.0;
	//pix = (double)Ymax*(double)Xmax;
	//for (i = 0;i<Xmax;i++)
	//{
	//	for (j = 0;j<Ymax;j++)
	//	{
	//		x = (double)t1[i][j] - (double)Ve[i][j];
	//		error += ((x * x) / pix);
	//	}
	//}
	//psnr = 10.0 * log10((255.0*255.0) / error);

	//cout << "psnr=" << psnr << "\n";
	//////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Temporise en attendant la pression d'une touche pendant 5 secondes
	cvWaitKey();
	//system("PAUSE");
	return 0;
}