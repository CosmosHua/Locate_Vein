#pragma once
#ifndef _Hua_Locate_
#define _Hua_Locate_

#define DLL_API __declspec(dllexport)

#include<iostream>
#include<opencv.hpp>

using namespace cv;
using namespace std;

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
DLL_API double Tracker(const Mat& frame, Mat& obj, Rect& oBox, int(&pp)[4]);

//Name:	Track
//Data:	2017-07-03
//Function: Find obj in frame, oBox=position, mul=augment factor
//		frame: 3-channel source image, in which to find obj.
//		obj: 1-channel template image, which to find in frame.
//		oBox: restore the position of obj in frame, same size as obj.
//		dc: coefficient, search = (width*dc.x, height*dc.y) + oBox.
//Return: the probability that obj best matches frame.
DLL_API double Track(const Mat& frame, const Mat& obj, Rect& oBox, Point2f dc);

//Name:	Locate
//Data:	2017-07-04
//Function: Create ROI, Locate obj, [Mark arrow line, change coordinates].
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the right center of obj in src,
//			[origin = the center of src, x-coordinate = upwards, y-coordinate = leftwards].
//		Mid: the search range of angle in [-Mid, Mid] degree. Mid prefers in [0, 90].
//			return = degree obj rotates left center from rightwards, anti-clockwise as positive.
//Return: the probability that obj best matches src.
DLL_API double Locate(const Mat& src, const Mat& obj, Point& pos, int& Mid);

//Name:	Match
//Data:	2017-07-04
//Function:	find obj in src, pos = position in src's coordinates.
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the right center of obj in src,
//			origin = top left corner of src, x-coordinate = rightwards, y-coordinate = downwards.
//		Mid: the search range of angle in [-Mid, Mid] degree. Mid prefers in [0, 90].
//			return = degree obj rotates left center from rightwards, anti-clockwise as positive.
//Return: the probability that obj best matches src.
DLL_API double Match(const Mat& src, const Mat& obj, Point& pos, int& Mid);

//Name:	Match1
//Data:	2017-07-04
//Function:	find obj in src, pos = paracentral position in src's coordinates.
//		src: source image, in which to find obj.
//		obj: template image, which to find in src.
//		pos: restore the position of the paracentral center of obj in src,
//			origin = top left corner of src, x-coordinate = rightwards, y-coordinate = downwards.
//		Mid: the search range of angle in [-Mid, Mid] degree. Mid prefers in [0, 90].
//			return = degree obj rotates left center from rightwards, anti-clockwise as positive.
//Return: the probability that obj best matches src.
DLL_API double Match1(const Mat& src, const Mat& obj, Point& pos, int& Mid);

//Rotate Image im by degree with scale, return rotated image:
DLL_API Mat RotIm(const Mat& im, int degree, double scale = 1.0);

//Rotate Point sP by degree with scale, return rotated point:
DLL_API Point2d PtRot(const Mat& im, const Point2d& sP, int degree, double scale = 1.0);

//Rotate back Point dP from the rotated coordinates:
DLL_API Point2d PtBaRot(const Mat& im, const Point2d& dP, int degree, double scale = 1.0);

//Rotate Point sP around center by degree with scale, return rotated point:
DLL_API Point2d RotPt(const Point2d& center, const Point2d& sP, int degree, double scale = 1.0);

//Compute the new center after rotate by degree with scale:
DLL_API Point2d NewCt(const Point2d& center, int degree, double scale = 1.0);

//convert pos to a new coordinates:
DLL_API void NewCor(const Point& center, Point& pos, int flag = 0);

//Mark pos(dot), deg(arrow line) on src, OR rectangle on rotated src:
DLL_API Mat MarkIm(const Mat& src, const Mat& obj, const Point& pos, int deg, bool k = false);

#endif
