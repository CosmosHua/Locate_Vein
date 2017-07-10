#include<iostream>
#include<opencv.hpp>

using namespace cv;
using namespace std;

Mat ImRot(const Mat& im, int degree, double scale = 1.0);
Point PtRot(const Mat& im, const Point sP, int degree, double scale = 1.0);
Point PtBaRot(const Mat& im, const Point dP, int degree, double scale = 1.0);

void main()
{
	Mat src = imread("F:/Test/Girl.png", 0);

	Point pos(210, 300), dp, sp;
	namedWindow("Source");	namedWindow("Result");

	Mat dst = src.clone();
	circle(dst, pos, 4, Scalar(), -1);
	imshow("Source", ImRot(dst, 45, 0.6));

	dp = PtRot(src, pos, 45, 0.6);
	dst = ImRot(src, 45, 0.6);
	circle(dst, dp, 4, Scalar(), -1);
	imshow("Result", dst);

	sp = PtBaRot(src, dp, 45, 0.6);
	cout << "Rotated Point: " << dp << endl;
	cout << "Original Point: " << sp << endl;
	waitKey();
}//end main

Mat ImRot(const Mat& im, int degree, double scale)
{
	int width = im.cols, height = im.rows;
	Point2d center(width / 2.0, height / 2.0);
	//get rotation matrix around center by degree with scale:
	Mat rot = getRotationMatrix2D(center, degree, scale), dst;

	double angle = degree * CV_PI / 180;
	double a = abs(sin(angle))*scale, b = abs(cos(angle))*scale;
	double width2 = height*a + width*b, height2 = height*b + width*a;
	//translate by the center difference:
	rot.col(2).row(0) += (width2 - width) / 2.0;
	rot.col(2).row(1) += (height2 - height) / 2.0;

	//Affine to new Size with filled color:
	Size sz(cvRound(width2), cvRound(height2));
	warpAffine(im, dst, rot, sz, 1, 0, Scalar(255, 255, 255));
	return dst;
}//end ImRot

Point PtRot(const Mat& im, const Point sP, int degree, double scale)
{
	int width = im.cols, height = im.rows;
	Point2d center(width / 2.0, height / 2.0);
	double angle = degree * CV_PI / 180;	Point dP(sP);

	//rotate sP around the center by angle with scale:
	double x = sP.x - center.x, y = sP.y - center.y;
	double sina = sin(angle)*scale, cosa = cos(angle)*scale;
	double dPx = x*cosa + y*sina + center.x;
	double dPy = -x*sina + y*cosa + center.y;

	//translate the rotated sP by center difference:
	double a = abs(sin(angle))*scale, b = abs(cos(angle))*scale;
	double width2 = height*a + width*b, height2 = height*b + width*a;
	dP.x = cvRound(dPx + (width2 - width) / 2.0);
	dP.y = cvRound(dPy + (height2 - height) / 2.0);
	return dP;
}//end PtRot

Point PtBaRot(const Mat& im, const Point dP, int degree, double scale)
{
	int width = im.cols, height = im.rows;
	Point2d center(width / 2.0, height / 2.0);
	double angle = degree * CV_PI / 180;	Point sP(dP);

	//translate dP back by center difference:
	double a = abs(sin(angle))*scale, b = abs(cos(angle))*scale;
	double width2 = height*a + width*b, height2 = height*b + width*a;
	double dPx = dP.x - (width2 - width) / 2.0;
	double dPy = dP.y - (height2 - height) / 2.0;

	//rotate dP around the center by -angle with 1/scale:
	double x = dPx - center.x, y = dPy - center.y;
	double sina = sin(-angle) / scale, cosa = cos(-angle) / scale;
	sP.x = cvRound(x*cosa + y*sina + center.x);
	sP.y = cvRound(-x*sina + y*cosa + center.y);
	return sP;
}//end PtBaRot
