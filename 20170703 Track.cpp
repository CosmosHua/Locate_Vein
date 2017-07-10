#include "DLLGen.h"
#pragma comment(lib, "DLLx64")

void main(int argc, char* argv[])
{
	VideoCapture capture(0);
	capture.open("D:/Hua/uvs170418-003.avi");

	Mat frame, obj;	namedWindow("Result");
	capture >> frame;
	if (frame.empty())
	{	cout << "Frame Empty!" << endl;	return;	}

	Rect oBox;	int pp[4] = { 0, 0, 25, 20 };
	//pp[2]: init=search degree range, return=prick degree.
	//pp[3]: init=threshold, return negative if lose obj.
	for (int i = 0; ; ++i)
	{
		capture >> frame;
		double mx = Tracker(frame, obj, oBox, pp);

		rectangle(frame, oBox, Scalar(0, 255, 0), 2);
		frame = MarkIm(frame, obj, Point(pp[0],pp[1]), pp[2]);
		imshow("Result", frame);

		cout << "Prob = " << mx << "\t:";
		cout << " Pos = " << Point(pp[0], pp[1]);
		cout << " Degree = " << pp[2] << endl;
		//if (pp[3] < 0)	waitKey();
		switch (waitKey(10))
		{
		case 'q':	return;
		case 'n':	pp[2] = 25;	obj = Mat();	break;
		}//end switch
	}//end while
}//end main
