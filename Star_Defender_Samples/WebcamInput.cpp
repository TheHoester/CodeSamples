#include "opencv2/highgui.hpp"
#include "opencv2/objdetect.hpp"
#include "opencv2/video.hpp"
#include "opencv2/videoio.hpp"
#include <iostream>
#include <stdio.h>

#pragma comment (lib, "opencv_core310d.lib")
#pragma comment (lib, "opencv_imgproc310d.lib")
#pragma comment (lib, "opencv_objdetect310d.lib")
#pragma comment (lib, "opencv_video310d.lib")
#pragma comment (lib, "opencv_videoio310d.lib")

using namespace std;
using namespace cv;

// Declare structure to be used to pass data from C++ to Mono.
struct Circle
{
	Circle(int x, int y, int radius) : X(x), Y(y), Radius(radius) {}
	int X, Y, Radius;
};

String windowName = "Unity OpenCV";
Mat frame;
Mat back;
Mat fore;
vector<pair<Point, double> > palmCentres;
VideoCapture capture;
Ptr<BackgroundSubtractorMOG2> pMOG2;
int backgroundFrame = 500;
CascadeClassifier faceCascade;
int _scale = 1;


extern "C" int __declspec(dllexport) __stdcall  Init(int& outCameraWidth, int& outCameraHeight)
{
	pMOG2 = createBackgroundSubtractorMOG2();
	pMOG2->setDetectShadows(false);
	pMOG2->setNMixtures(3);

	// Load LBP face cascade.
	if (!faceCascade.load("lbpcascade_frontalface.xml"))
		return -1;

	// Open the stream.
	capture.open(0);
	if (!capture.isOpened())
		return -2;

	outCameraWidth = capture.get(CAP_PROP_FRAME_WIDTH);
	outCameraHeight = capture.get(CAP_PROP_FRAME_HEIGHT);

	return 0;
}

extern "C" void __declspec(dllexport) __stdcall  Close()
{
	capture.release();
}

extern "C" void __declspec(dllexport) __stdcall SetScale(int scale)
{
	_scale = scale;
}

extern "C" void __declspec(dllexport) __stdcall DetectFace(Circle* outFaces, int maxOutFacesCount, int& outDetectedFacesCount)
{
	Mat frame;
	capture >> frame;
	if (frame.empty())
		return;

	std::vector<Rect> faces;
	// Convert the frame to grayscale for cascade detection.
	Mat grayscaleFrame;
	cvtColor(frame, grayscaleFrame, COLOR_BGR2GRAY);
	Mat resizedGray;
	// Scale down for better performance.
	resize(grayscaleFrame, resizedGray, Size(frame.cols / _scale, frame.rows / _scale));
	equalizeHist(resizedGray, resizedGray);

	// Detect faces.
	faceCascade.detectMultiScale(resizedGray, faces);

	// Draw faces.
	for (size_t i = 0; i < faces.size(); i++)
	{
		Point center(_scale * (faces[i].x + faces[i].width / 2), _scale * (faces[i].y + faces[i].height / 2));
		ellipse(frame, center, Size(_scale * faces[i].width / 2, _scale * faces[i].height / 2), 0, 0, 360, Scalar(0, 0, 255), 4, 8, 0);

		// Send to application.
		outFaces[i] = Circle(faces[i].x, faces[i].y, faces[i].width / 2);
		outDetectedFacesCount++;

		if (outDetectedFacesCount == maxOutFacesCount)
			break;
	}

	// Display debug output.
	imshow(windowName, frame);
}

extern "C" void __declspec(dllexport) __stdcall DetectHand(double& outPalmCentreX, double& outPalmCentreY, int& outNumOfFingers)
{
	vector<vector<Point>> contours;

	capture >> frame;

	//Update the current background model and get the foreground
	if (backgroundFrame > 0)
	{
		pMOG2->apply(frame, fore);
		backgroundFrame--;
	}
	else
		pMOG2->apply(frame, fore, 0);

	//Get background image to display it
	pMOG2->getBackgroundImage(back);

	//Enhance edges in the foreground by applying erosion and dilation
	erode(fore, fore, Mat());
	dilate(fore, fore, Mat());

	//Find the contours in the foreground
	findContours(fore, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	for (int i = 0; i < contours.size(); i++)
	{
		//Ignore all small insignificant areas
		if (contourArea(contours[i]) >= 5000)
		{
			//Draw contour
			vector<vector<Point> > tcontours;
			tcontours.push_back(contours[i]);
			drawContours(frame, tcontours, -1, cv::Scalar(0, 0, 255), 2);

			//Detect Hull in current contour
			vector<vector<Point> > hulls(1);
			vector<vector<int> > hullsI(1);
			convexHull(Mat(tcontours[0]), hulls[0], false);
			convexHull(Mat(tcontours[0]), hullsI[0], false);
			drawContours(frame, hulls, -1, cv::Scalar(0, 255, 0), 2);

			//Find minimum area rectangle to enclose hand
			RotatedRect rect = minAreaRect(Mat(tcontours[0]));

			//Find Convex Defects
			vector<Vec4i> defects;
			if (hullsI[0].size() > 0)
			{
				Point2f rect_points[4]; rect.points(rect_points);
				for (int j = 0; j < 4; j++)
					line(frame, rect_points[j], rect_points[(j + 1) % 4], Scalar(255, 0, 0), 1, 8);
				Point roughCentre;
				convexityDefects(tcontours[0], hullsI[0], defects);
				if (defects.size() >= 3)
				{
					vector<Point> palmPoints;
					for (int j = 0; j < defects.size(); j++)
					{
						int startidx = defects[j][0];
						Point ptStart(tcontours[0][startidx]);
						int endidx = defects[j][1];
						Point ptEnd(tcontours[0][endidx]);
						int faridx = defects[j][2];
						Point ptFar(tcontours[0][faridx]);
						//Sum up all the hull and defect points to compute average
						roughCentre += ptFar + ptStart + ptEnd;
						palmPoints.push_back(ptFar);
						palmPoints.push_back(ptStart);
						palmPoints.push_back(ptEnd);
					}

					//Get palm center by 1st getting the average of all defect points, this is the rough palm center,
					//Then U chose the closest 3 points ang get the circle radius and center formed from them which is the palm center.
					roughCentre.x /= defects.size() * 3;
					roughCentre.y /= defects.size() * 3;
					Point closest_pt = palmPoints[0];
					vector<pair<double, int> > distvec;
					for (int i = 0;i < palmPoints.size();i++)
					{
						double value = (roughCentre.x - palmPoints[i].x) * (roughCentre.x - palmPoints[i].x) +
							(roughCentre.y - palmPoints[i].y) * (roughCentre.y - palmPoints[i].y);
						distvec.push_back(make_pair(value, i));
					}
					sort(distvec.begin(), distvec.end());

					//Keep choosing 3 points till you find a circle with a valid radius
					//As there is a high chance that the closes points might be in a linear line or too close that it forms a very large circle
					pair<Point, double> soln_circle;
					for (int i = 0;i + 2 < distvec.size();i++)
					{
						Point p1 = palmPoints[distvec[i + 0].second];
						Point p2 = palmPoints[distvec[i + 1].second];
						Point p3 = palmPoints[distvec[i + 2].second];

						double offset = pow(p2.x, 2) + pow(p2.y, 2);
						double bc = (pow(p1.x, 2) + pow(p1.y, 2) - offset) / 2.0;
						double cd = (offset - pow(p3.x, 2) - pow(p3.y, 2)) / 2.0;
						double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x)* (p1.y - p2.y);
						double TOL = 0.0000001;
						if (abs(det) >= TOL)
						{
							double idet = 1 / det;
							double centerx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * idet;
							double centery = (cd * (p1.x - p2.x) - bc * (p2.x - p3.x)) * idet;
							double radius = sqrt(pow(p2.x - centerx, 2) + pow(p2.y - centery, 2));

							soln_circle = make_pair(Point(centerx, centery), radius);
						}

						if (soln_circle.second != 0)
							break;
					}

					//Find avg palm centers for the last few frames to stabilize its centers, also find the avg radius
					palmCentres.push_back(soln_circle);
					if (palmCentres.size() > 10)
						palmCentres.erase(palmCentres.begin());

					Point palmCentre = Point();
					double radius = 0;
					for (int i = 0;i < palmCentres.size();i++)
					{
						palmCentre += palmCentres[i].first;
						radius += palmCentres[i].second;
					}
					palmCentre.x /= palmCentres.size();
					palmCentre.y /= palmCentres.size();
					radius /= palmCentres.size();

					//Draw the palm center and the palm circle
					//The size of the palm gives the depth of the hand
					circle(frame, palmCentre, 5, Scalar(144, 144, 255), 3);
					circle(frame, palmCentre, radius, Scalar(144, 144, 255), 2);

					//Detect fingers by finding points that form an almost isosceles triangle with certain thesholds
					outNumOfFingers = 0;
					for (int j = 0;j < defects.size();j++)
					{
						int startidx = defects[j][0]; Point ptStart(tcontours[0][startidx]);
						int endidx = defects[j][1]; Point ptEnd(tcontours[0][endidx]);
						int faridx = defects[j][2]; Point ptFar(tcontours[0][faridx]);
						//X o--------------------------o Y
						double Xdist = sqrt((palmCentre.x - ptFar.x) * (palmCentre.x - ptFar.x) + (palmCentre.y - ptFar.y) * (palmCentre.y - ptFar.y));
						double Ydist = sqrt((palmCentre.x - ptStart.x) * (palmCentre.x - ptStart.x) + (palmCentre.y - ptStart.y) * (palmCentre.y - ptStart.y));
						double length = sqrt((ptFar.x - ptStart.x) * (ptFar.x - ptStart.x) + (ptFar.y - ptStart.y) * (ptFar.y - ptStart.y));
						double retLength = sqrt((ptEnd.x - ptFar.x) * (ptEnd.x - ptFar.x) + (ptEnd.y - ptFar.y) * (ptEnd.y - ptFar.y));

						//Play with these thresholds to improve performance
						if (length <= 3 * radius&&Ydist >= 0.4*radius&&length >= 10 && retLength >= 10 && max(length, retLength) / min(length, retLength) >= 0.8)
							if (min(Xdist, Ydist) / max(Xdist, Ydist) <= 0.8)
								if ((Xdist >= 0.1*radius&&Xdist <= 1.3*radius&&Xdist < Ydist) || (Ydist >= 0.1*radius&&Ydist <= 1.3*radius&&Xdist > Ydist))
									line(frame, ptEnd, ptFar, Scalar(0, 255, 0), 1), outNumOfFingers++;
					}

					outPalmCentreX = palmCentre.x;
					outPalmCentreY = palmCentre.y;
					outNumOfFingers = min(5, outNumOfFingers);
				}
			}

		}
	}
	imshow(windowName + "Frame", frame);
}