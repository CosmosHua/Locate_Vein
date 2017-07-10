#include <iostream>
#include <opencv.hpp>

using namespace cv;
using namespace std;

Mat onAffine(Mat* im, Point2f dif);
Mat onRot(const Mat& im, int pos, int rat);
void onMouse(int Event, int x, int y, int drag, Mat* im);

void main(int argc, char* argv[])
{
	Mat Cam = imread("F:/Test/Girl.png"), dst;
	namedWindow("Video");	int pos = 90, rat = 10;
	createTrackbar("Angle", "Video", &pos, 180); //rotate the image
	createTrackbar("Scale", "Video", &rat, 20); //scale the image
	setMouseCallback("Video", (MouseCallback)onMouse, &Cam); //drag image

	while ((waitKey(50) & 255) != 13)
	{//press the Enter key to exit the programe
		dst = onRot(Cam, pos, rat);
		imshow("Video", dst);
	}//end while
}//end main


void onMouse(int Event, int x, int y, int drag, Mat* im)
{
	static Mat res = im->clone();   static const Point ini(0, 0);
	static Point seed(ini), tdf(ini);   Point now(x, y);

	switch (Event)
	{
	case CV_EVENT_LBUTTONDOWN:  seed = now; break;
	case CV_EVENT_LBUTTONUP:    tdf += (now - seed);  break;
	case CV_EVENT_RBUTTONUP:    res.copyTo(*im);    tdf = ini;  break;
	}//end switch

	if (drag == CV_EVENT_FLAG_LBUTTON) //drag the mouse to move the image
		*im = onAffine(&res, tdf + (now - seed)); //use Affine transformation
}//end onMouse

Mat onAffine(Mat* im, Point2f dif)
{
	Point2f org[3], tri[3];  org[1] = Point2f(im->cols / 2, 0);
	org[0] = Point2f(0, 0);  org[2] = Point2f(0, im->rows / 2);
	for (int i = 0; i<3; ++i)  tri[i] = org[i] + dif;

	Mat warp = getAffineTransform(org, tri), dst;
	warpAffine(*im, dst, warp, im->size(), 1, 0, Scalar(255, 255, 255));
	return dst;
}//end onAffine

Mat onRot(const Mat& im, int pos, int rat)
{
	int width = im.cols, height = im.rows;
	Point2d center(width / 2.0, height / 2.0);
	double degree = 2*pos - 180, scale = 0.1*rat;
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
}//end onRot
