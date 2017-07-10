#include "DLLGen.h"

//Name:	Tracker
//Data:	2017-07-04
//Function: Track obj in frame or get (obj,oBox,pos,deg).
//		frame: 3-channel source image, in which to track obj.
//		obj: 1-channel template image, which to track in frame.
//			If NULL: get/update (obj, oBox, pos=pp[0-1], deg=pp[2]).
//		oBox: restore the position of obj in frame, same size as obj.
//		pp[0] = pos.x, pp[1] = pos.y, pp[2] = degree, pp[3] = what to do.
//		pp[2]: init = degree range to search, return = degree to prick.
//		pp[3]: init = threshold to determine whether lose obj or not.
//			return = minus(position variation > pp[3]), unaltered (else).
//Return: the probability that obj best matches frame.
double Tracker(const Mat& frame, Mat& obj, Rect& oBox, int(&pp)[4])
{
	Mat gray;	Point pos;	double mx;
	if (frame.channels()<3)	gray = frame.clone();
	else	cvtColor(frame, gray, CV_RGB2GRAY);

	if (obj.empty()) //update (obj,oBox,pos,deg)
	{
		obj = imread("Vein=.png", 0);
		mx = Locate(gray, obj, pos, pp[2]);
		pp[0] = pos.x;	pp[1] = pos.y;
		pos -= 0.5*Point(obj.size()); //top-left
		oBox = Rect(pos, obj.size()); //update oBox
		//oBox.y -= oBox.height/2;	oBox.height *= 2;
		obj = gray(oBox); //update obj
		return mx; //match probability
	}//end if

	pp[3] = abs(pp[3]);	Rect pre = oBox; //initialize
	mx = Track(frame, obj, oBox, Point2f(1,2)); //update oBox
	pos = (oBox.tl() + oBox.br())*0.5; //oBox center
	pp[0] = pos.x;	pp[1] = pos.y; //update pos
	if (abs(pre.x-oBox.x) > pp[3] || abs(pre.y-oBox.y) > pp[3])
		pp[3] *= -1; //pp[3]<0: lose target, else: keep tracking
	return mx; //tracking probability
}//end Tracker

//Name:	Track
//Data:	2017-07-03
//Function: Find obj in part of frame, oBox=position.
//		frame: 3-channel source image, in which to find obj.
//		obj: 1-channel template image, which to find in frame.
//		oBox: restore the position of obj in frame, same size as obj.
//		dc: coefficient, search = (width*dc.x, height*dc.y) + oBox.
//Return: the probability that obj best matches frame.
double Track(const Mat& frame, const Mat& obj, Rect& oBox, Point2f dc)
{
	const Size delta(oBox.width*dc.x, oBox.height*dc.y);
	Rect search(oBox + delta); //augment size(width,height)
	search -= 0.5*Point(delta); //calibrate top-left(x,y)
	search &= Rect(Point(0, 0), frame.size()); //intersect

	Mat src, similar;	Point loc;	double mx = 0;
	if (frame.channels()<3)	src = frame.clone();
	else	cvtColor(frame, src, CV_RGB2GRAY); //essential
	matchTemplate(src(search), obj, similar, TM_CCOEFF_NORMED);
	minMaxLoc(similar, NULL, &mx, NULL, &loc);

	oBox.x = loc.x + search.x; //update oBox.x
	oBox.y = loc.y + search.y; //update oBox.y
	return mx; //highest probability
}//end Track

//Name:	Locate
//Data:	2017-07-04
//Function: create ROI, locate obj(right center) in src.
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the right center of obj in src,
//			origin = top left corner of src, x-axis = rightwards, y-axis = downwards.
//		deg: the search range of angle in [-deg, deg] degree. deg prefers in [0, 90].
//			return = degrees obj rotates right center, anti-clockwise as positive.
//Return: the probability that obj best matches src.
double Locate(const Mat& src, const Mat& obj, Point& pos, int& deg)
{
	//crop image: retain in ROI
	const int x = src.cols, y = src.rows;
	const double dx = (x > y) ? 0.05 : 0;
	const double dy = (x > y) ? 0.10 : 0.15;
	const double rate = (x > y) ? 0.75 : 0.9;
	Point Org(x*dx, y*dy), End(x*(1-dx)*rate, y*(1-dy));
	Mat dst = src(Rect(Org, End)); //ROI

	//deg = degree that obj should rotate
	double mx = Match(dst, obj, pos, deg);
	pos += Org; //coordinates: from ROI back to src
	return mx; //MarkIm(src, obj, pos, deg);
}//end Locate

//Name:	Match
//Data:	2017-07-04
//Function:	find obj in src, pos = right center position.
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the right center of obj in src,
//			origin = top left corner of src, x-axis = rightwards, y-axis = downwards.
//		deg: the search range of angle in [-deg, deg] degree. deg prefers in [0, 90].
//			return = degrees obj rotates right center, anti-clockwise as positive.
//Return: the probability that obj best matches src.
double Match(const Mat& src, const Mat& obj, Point& pos, int& deg)
{
	const double scale = 1.0;
	Point loc;	double mx = 0;	int id = 0;
	deg = abs(deg);	const int MAX = 2 * deg + 1;
	for (int i = 0; i < MAX; ++i)
	{
		Point now;	double x = 0;
		Mat dst = RotIm(src, i - deg, scale);
		matchTemplate(dst, obj, dst, TM_CCOEFF_NORMED);
		minMaxLoc(dst, NULL, &x, NULL, &now);
		if (x > mx)//find (i,x,now) with max prob
		{	id = i;	mx = x;	loc = now;	}
	}//end for
	deg = -(id - deg); //degrees obj rotates
	//pick right center/endpoint, arrow left:
	pos = loc + Point(obj.cols, obj.rows/2);
	pos = PtBaRot(src, pos, -deg, scale);
	return mx; //highest probability
}//end Match

//Name:	Match1
//Data:	2017-07-04
//Function:	find obj in src, pos = paracentral center position.
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the paracentral center of obj in src,
//			origin = top left corner of src, x-axis = rightwards, y-axis = downwards.
//		deg: the search range of angle in [-deg, deg] degree. deg prefers in [0, 90].
//			return = degrees obj rotates paracentral center, anti-clockwise as positive.
//Return: the probability that obj best matches src.
double Match1(const Mat& src, const Mat& obj, Point& pos, int& deg)
{
	const double scale = 1.0;
	Point loc;	double mx = 0;	int id = 0;
	deg = abs(deg);	const int MAX = 2 * deg + 1;
	//Point loc[MAX];	double mx[MAX] = { 0 };
	//vector<Point> loc(MAX);	vector<double> mx(MAX);
	for (int i = 0; i < MAX; ++i)
	{
		Point now;	double x = 0;
		Mat dst = RotIm(src, i - deg, scale);
		matchTemplate(dst, obj, dst, TM_CCOEFF_NORMED);
		minMaxLoc(dst, NULL, &x, NULL, &now);
		if (x > mx)//find (i,x,now) with max prob
		{	id = i;	mx = x;	loc = now;	}
	}//end for
	//rotate left/right center from arrow right/left:
	deg = -(id - deg); //obj rotates, against src

	Point center(src.size());
	center = NewCt(center*0.5, -deg, scale);
	pos = loc + Point(0, obj.rows / 2); //left center
	loc += Point(obj.cols, obj.rows / 2); //right center
	//find the paracentral endpoint/center:
	if (norm(pos - center) < norm(loc - center))
		pos = PtBaRot(src, pos, -deg, scale); //arrow right
	else //swap to paracentral/right endpoint/center:
		pos = PtBaRot(src, loc, -deg, scale); //arrow left
	return mx; //highest probability
}//end Match1

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
	warpAffine(im, dst, rot, Size(2*center2), 1, 0, Scalar(255, 255, 255));
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

//convert pos to a new coordinates:
void NewCor(const Point& center, Point& pos, int flag)
{
	switch (flag)
	{
	case 1:	pos -= center;	break; //center as origin
	case 2:	pos = -Point(pos.y, pos.x);	break; //to new coordinates
	case -1:pos = -Point(pos.y, pos.x);	pos += center; 	break; //recover
	default:pos -= center; 	pos = -Point(pos.y, pos.x);	break; //transform
	}//end switch
}//end NewCor

//Mark pos(dot), deg(arrow line) on src, OR rectangle on rotated src:
Mat MarkIm(const Mat& src, const Mat& obj, const Point& pos, int deg, bool k)
{
	Mat dst;
	if (src.channels()>1)	dst = src.clone();
	else	cvtColor(src, dst, CV_GRAY2RGB);
	circle(dst, pos, 5, Scalar(0, 0, 255), -1); //Mark pos
	if (deg)
	{	//Mark deg with an arrow line on original image:
		Point2d pt(pos.x - obj.cols, pos.y); //arrow left
		//Point2d pt(pos.x + obj.cols, pos.y); //arrow right
		pt = RotPt(pos, pt, deg); //rotate arrow endpoint
		arrowedLine(dst, pos, pt, Scalar(0, 0, 255), 2);
	}//end if
	if (k)
	{	//Mark an rectangle on ratated image:
		dst = RotIm(dst, -deg, 1.0);
		Point2d pt2 = PtRot(src, pos, -deg);
		//pt2 -= Point2d(0, obj.rows*0.5); //arrow right
		pt2 -= Point2d(obj.cols, obj.rows*0.5); //arrow left
		rectangle(dst, Rect(pt2, obj.size()), Scalar(0, 255, 0), 2);
	}//end if
	return dst;
}//end 
