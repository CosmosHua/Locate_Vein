#include "DLLGen.h"
#pragma comment(lib, "DLLx64")

void main()
{
	Mat src = imread("F:/Test/Pic/2017-06-12-14-21-25.jpg", 0);
	Mat obj = imread("F:/Test/Vein=.png", 0); //load template

	Point pos;	int deg = 25; //position & degree
	cout << "Prob = " << Locate(src, obj, pos, deg) << endl;
	namedWindow("Result"); //find pos & deg:
	imshow("Result", MarkIm(src, obj, pos, deg));
	cout << "Position to prick (center as origin):\t" << pos << endl;
	cout << "Direction to ratate (anti-clockwise):\t" << deg << " degrees" << endl;
	waitKey();
}//end main
