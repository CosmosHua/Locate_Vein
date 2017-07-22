#include "DLLGen.h"
#pragma comment(lib, "DLLx64")

void main(int argc, char* argv[])
{
	Mat src = imread("F:/Test/Girl.png");
	//horizontal=0, vertical=1, both=-1
	flip(src, src, -1);
	namedWindow("test");	imshow("test", src);
	waitKey();
}//end main
