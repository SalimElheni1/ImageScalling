#pragma once

//Ref: https://www.lighthouse3d.com/tutorials/maths/catmull-rom-spline/

/* Coefficients for Matrix M */
#define M11	 0.0
#define M12	 1.0
#define M13	 0.0
#define M14	 0.0
#define M21	-0.5
#define M22	 0.0
#define M23	 0.5
#define M24	 0.0
#define M31	 1.0
#define M32	-2.5
#define M33	 2.0
#define M34	-0.5
#define M41	-0.5
#define M42	 1.5
#define M43	-1.5
#define M44	 0.5

double catmullRomSpline(float x, float v0, float v1, float v2, float v3) 
{
	double c1, c2, c3, c4;

	c1 = M12*v1;
	c2 = M21*v0 + M23*v2;
	c3 = M31*v0 + M32*v1 + M33*v2 + M34*v3;
	c4 = M41*v0 + M42*v1 + M43*v2 + M44*v3;

	return(((c4*x + c3)*x + c2)*x + c1);
}

double catmullRomSplineV(float x, vector<uchar>v)
{
	double c1, c2, c3, c4;

	c1 = M12*v[1];
	c2 = M21*v[0] + M23*v[2];
	c3 = M31*v[0] + M32*v[1] + M33*v[2] + M34*v[3];
	c4 = M41*v[0] + M42*v[1] + M43*v[2] + M44*v[3];

	return(((c4*x + c3)*x + c2)*x + c1);
}