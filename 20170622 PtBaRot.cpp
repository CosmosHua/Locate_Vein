#include<iostream>
#include<opencv.hpp>

using namespace cv;
using namespace std;

Mat RotIm(const Mat& im, int degree, double scale = 1.0);
Point2d PtRot(const Mat& im, const Point2d& sP, int degree, double scale = 1.0);
Point2d PtBaRot(const Mat& im, const Point2d& dP, int degree, double scale = 1.0);
Point2d RotPt(const Point2d& center, const Point2d& sP, int degree, double scale = 1.0);
Point2d NewCt(const Point2d& center, int degree, double scale = 1.0);

void main()
{
	Mat src = imread("F:/Test/Girl.png", 0);

	Point pos(200, 300), dp, sp;
	namedWindow("Source");	namedWindow("Result");

	Mat dst = src.clone();
	circle(dst, pos, 4, Scalar(), -1);
	imshow("Source", RotIm(dst, 45, 0.6));

	dp = PtRot(src, pos, 45, 0.6);
	dst = RotIm(src, 45, 0.6);
	circle(dst, dp, 4, Scalar(), -1);
	imshow("Result", dst);

	sp = PtBaRot(src, dp, 45, 0.6);
	cout << "Rotated Point: " << dp << endl;
	cout << "Original Point: " << sp << endl;
	waitKey();
}//end main


//Rotate Image im by degree with scale, return rotated image:
Mat RotIm(const Mat& im, int degree, double scale)
{
	Point2d center(im.size());	center *= 0.5;
	//get rotation matrix around center by degree with scale:
	Mat rot = getRotationMatrix2D(center, degree, scale), dst;

	//translate by the center difference:
	Point2d center2(NewCt(center, degree, scale));
	rot.col(2).row(0) += (center2 - center).x;
	rot.col(2).row(1) += (center2 - center).y;

	//Affine to new Size with filled color:
	warpAffine(im, dst, rot, Size(2 * center2), 1, 0, Scalar(255, 255, 255));
	return dst;
}//end RotIm

//Rotate Point sP by degree with scale, return rotated point:
Point2d PtRot(const Mat& im, const Point2d& sP, int degree, double scale)
{
	Point2d center(im.size());	center *= 0.5;
	//rotate sP around center by degree with scale:
	Point2d dP = RotPt(center, sP, degree, scale);
	//translate the rotated sP by center difference:
	return dP += (NewCt(center, degree, scale) - center);
}//end PtRot

//Rotate back Point dP from the rotated coordinates:
Point2d PtBaRot(const Mat& im, const Point2d& dP, int degree, double scale)
{
	Point2d center(im.size());	center *= 0.5;
	//translate dP back by center difference:
	Point2d sP = dP - (NewCt(center, degree, scale) - center);
	//rotate dP around center by -degree with 1/scale:
	return RotPt(center, sP, -degree, 1 / scale);
}//end PtBaRot

//Rotate Point sP around center by degree with scale, return rotated point:
Point2d RotPt(const Point2d& center, const Point2d& sP, int degree, double scale)
{
	double angle = degree * CV_PI / 180;
	double sina = sin(angle)*scale, cosa = cos(angle)*scale;
	Point2d P(sP - center), dP(center);
	dP.x += P.x*cosa + P.y*sina;
	dP.y += -P.x*sina + P.y*cosa;
	return dP;
}//end RotPt

//Compute the new center after rotate by degree with scale:
Point2d NewCt(const Point2d& center, int degree, double scale)
{
	double angle = degree * CV_PI / 180;
	double sina = abs(sin(angle)*scale), cosa = abs(cos(angle)*scale);
	double newx = center.x*cosa + center.y*sina;	//x = width/2
	double newy = center.x*sina + center.y*cosa;	//y = height/2
	return Point2d(newx, newy);
}//end NewCt
