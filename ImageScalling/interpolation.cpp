/********************************* interpolation par spline catmullRome ***************************************
***********************************     suivant X puis Y + PSNR        ****************************************
******************************** cette algo. fonctionne correctement ***************************************
***************************************************************************************************************/
#include "bib.h"

IplImage* image;
IplImage* imagetr;
IplImage* images;

# define N 3400
float Resolution = 2.0;
int _Ne, _Ns, _Nr;
double _Ve[N];
double  Vs[N][N];


//=========================================================================================

int _tmain(int argc, _TCHAR* argv[])
{
	// ---------------Ouvre un fichier image--------------------------
	IplImage* img = cvLoadImage("test.BMP");

	//------------ En cas de problème, fin de la fonction------------
	if (!img)
	{
		cout << "L'image est introuvable ou incorrect\n";

	}

	//-------------- Crée une fenêtre----------------------------------
	cvNamedWindow("Image");

	//-----------------------Affiche l'image--------------------------
	cvShowImage("Image", img);

	//-----------------------conversion de l'image en niveau de gris----------------
	image = cvCreateImage(cvGetSize(img), img->depth, 1);
	cvCvtColor(img, image, CV_RGB2GRAY);
	cvNamedWindow("image en niv de gris");
	cvShowImage("image en niv de gris", image);
	CvSize sizeimage = cvSize(image->width, image->height);

	//-------------------affichage taille de l'image-----------------//
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

	//====================affichage de l'image reduite================================
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
	int I, J, p, PtrS;
	double CourbePt = 0;

	images = cvCreateImage(cvSize(Resolution*Xmax, Resolution*Ymax), image->depth, 1);
	CvScalar pixels;
	//----------------------------INTERPOLATION SUIVANT X----------------------------------//
	//...........................................................................
	clock_t start, end;
	start = clock();
	//...........................................................................
	_Ne = Xmax + 1;
	_Nr = Resolution;
	_Ns = (_Ne - 1)*_Nr;

	for (int m = 0;m <= Ymax - 1;m++)
	{
		for (p = 0;p <= Xmax - 1;p++)
		{
			_Ve[p] = Ve[m][p];
		}
		PtrS = 0;
		for (I = 1;I <= (_Ne - 1);I++)
		{
			for (J = 1.0;J <= Resolution;J++)
			{
				float Temps = J / Resolution;
				int i, K;
				double Result = 0.0;

				for (int i = -2;i <= 1;i++)
				{
					K = I + i;
					if (K < 0) { K = 0; }
					if (K >= _Ne) { K = _Ne - 1; }
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
				PtrS = PtrS + 1;
			}
		}
	}
	//-------------------------INTERPOLATION SUIVANT Y---------------------------------------//
	_Ne = Ymax + 1;
	_Nr = Resolution;
	_Ns = (_Ne - 1)*_Nr;
	PtrS = 0;

	for (int m = 0;m <= Resolution*Xmax - 1;m++)
	{
		for (p = 0;p <= Ymax - 1;p++)
		{
			_Ve[p] = Vs[p][m];
		}
		PtrS = 0;
		for (I = 1;I <= (_Ne - 1);I++)
		{
			for (J = 1.0;J <= Resolution;J++)
			{
				float Temps = J / Resolution;
				int i, K;
				double Result = 0.0;

				for (int i = -2;i <= 1;i++)
				{
					K = I + i;
					if (K < 0) { K = 0; }
					if (K >= _Ne) { K = _Ne - 1; }
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
	//...........................................................................
	end = clock();
	printf("Temps en secondes : %f\n", (end - start) / (double)CLOCKS_PER_SEC);
	//...........................................................................

	//=========================Affichage de l'image interpoler============================//
	cvNamedWindow("image interpolée", CV_WINDOW_AUTOSIZE);
	cvShowImage("image interpolée", images);
	cvSaveImage("image interpolée.BMP", images);

	/*//============transformation de l'image interpoler en matrice=========//
		float **t1;
		t1 = new float* [Ymax];
		for (j=0;j<=Ymax-1;j++)
		{
		   t1[j]= new float [Xmax];
		   for(i=0;i<=Xmax-1 ;i++)
			 {
				pixel = cvGet2D(images,j,i);
				t1[j][i] = pixel.val[0];
			 }
		}

	//=========================PSNR===============================//
		double error, pix, x, psnr;
		error = 0.0;
		pix = (double)Ymax*(double)Xmax;
		for(i=0;i<Xmax;i++)
		{
			for(j=0;j<Ymax;j++)
			{
				x = (double)t1[i][j] - (double)Ve[i][j];
				error += ((x * x) / pix);
			}
		}
		psnr = 10.0 * log10((255.0*255.0)/error);
		cout<<"psnr="<<psnr<<"\n";*/

		//===================================================================================================//
			// Temporise en attendant la pression d'une touche pendant 5 secondes
	cvWaitKey();
	//system("PAUSE");
	return 0;
}