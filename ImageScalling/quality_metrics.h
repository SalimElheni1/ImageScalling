
	//-----------------------OK------------------------------------------------
	// Mean squared error
	double eqm(Mat & img1, Mat & img2)
	{
		int i, j;
		double eqm = 0;
		int height = img1.rows;
		int width = img1.cols;

		for (i = 0; i < height; i++)
			for (j = 0; j < width; j++)
				eqm += (img1.at<float>(i, j) - img2.at<float>(i, j)) * (img1.at<float>(i, j) - img2.at<float>(i, j));

		eqm /= height * width;

		return eqm;
	}
	/*	Compute the PSNR between 2 images  */
	double psnr(Mat & img_src, Mat & img_compressed)
	{
		int D = 255;
		return (10 * log10((D*D) / eqm(img_src, img_compressed)));
	}


	Scalar getMSSIM(const Mat& i1, const Mat& i2)
	{
		const double C1 = 6.5025, C2 = 58.5225;
		/***************************** INITS **********************************/
		int d = CV_32F;
		Mat I1, I2;
		i1.convertTo(I1, d);            // cannot calculate on one byte large values
		i2.convertTo(I2, d);
		Mat I2_2 = I2.mul(I2);        // I2^2
		Mat I1_2 = I1.mul(I1);        // I1^2
		Mat I1_I2 = I1.mul(I2);        // I1 * I2
									   /*************************** END INITS **********************************/
		Mat mu1, mu2;                   // PRELIMINARY COMPUTING
		GaussianBlur(I1, mu1, Size(11, 11), 1.5);
		GaussianBlur(I2, mu2, Size(11, 11), 1.5);
		Mat mu1_2 = mu1.mul(mu1);
		Mat mu2_2 = mu2.mul(mu2);
		Mat mu1_mu2 = mu1.mul(mu2);
		Mat sigma1_2, sigma2_2, sigma12;
		GaussianBlur(I1_2, sigma1_2, Size(11, 11), 1.5);
		sigma1_2 -= mu1_2;
		GaussianBlur(I2_2, sigma2_2, Size(11, 11), 1.5);
		sigma2_2 -= mu2_2;
		GaussianBlur(I1_I2, sigma12, Size(11, 11), 1.5);
		sigma12 -= mu1_mu2;
		Mat t1, t2, t3;
		t1 = 2 * mu1_mu2 + C1;
		t2 = 2 * sigma12 + C2;
		t3 = t1.mul(t2);                 // t3 = ((2*mu1_mu2 + C1).*(2*sigma12 + C2))
		t1 = mu1_2 + mu2_2 + C1;
		t2 = sigma1_2 + sigma2_2 + C2;
		t1 = t1.mul(t2);                 // t1 =((mu1_2 + mu2_2 + C1).*(sigma1_2 + sigma2_2 + C2))
		Mat ssim_map;
		divide(t3, t1, ssim_map);        // ssim_map =  t3./t1;
		Scalar mssim = mean(ssim_map);   // mssim = average of ssim map
		return mssim;
	}

	double myssim(Mat & ref_image, Mat & obj_image)
	{
		double C1 = 6.5025, C2 = 58.5225;

		int width = ref_image.cols;
		int height = ref_image.rows;
		double mean_x = 0;
		double mean_y = 0;
		double sigma_x = 0;
		double sigma_y = 0;
		double sigma_xy = 0;
		for (int v = 0; v < height; v++)
		{
			for (int u = 0; u < width; u++)
			{
				mean_x += ref_image.at<float>(v, u);
				mean_y += ref_image.at<float>(v, u);

			}
		}
		mean_x = mean_x / width / height;
		mean_y = mean_y / width / height;
		for (int v = 0; v < height; v++)
		{
			for (int u = 0; u < width; u++)
			{
				sigma_x += (ref_image.at<float>(v, u) - mean_x)* (ref_image.at<float>(v, u) - mean_x);
				sigma_y += (obj_image.at<float>(v, u) - mean_y)* (obj_image.at<float>(v, u) - mean_y);
				sigma_xy += abs((ref_image.at<float>(v, u) - mean_x)* (obj_image.at<float>(v, u) - mean_y));
			}
		}
		sigma_x = sigma_x / (width*height - 1);
		sigma_y = sigma_y / (width*height - 1);
		sigma_xy = sigma_xy / (width*height - 1);
		double fenzi = (2 * mean_x*mean_y + C1) * (2 * sigma_xy + C2);
		double fenmu = (mean_x*mean_x + mean_y * mean_y + C1) * (sigma_x + sigma_y + C2);
		double ssim = fenzi / fenmu;
		return ssim;

	}


	/*My PSNR Code*/
	/*double error, pix, x, psnr;
	error = 0.0;
	pix = width*hight;
	for (int i = 0;i<hight;i++)
	{
		for (int j = 0;j<width;j++)
		{
			x = dest.at<float>(i, j) - src.at<float>(i, j);
			error += ((x * x) / pix);
		}
	}
	psnr = 10.0 * log10((255.0*255.0) / error);*/ 



