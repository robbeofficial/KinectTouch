//============================================================================
// Name        : KinectTouch.cpp
// Author      : github.com/robbeofficial
// Version     : 0.something
// Description : recognizes touch points on arbitrary surfaces using kinect
// 				 and maps them to TUIO cursors
// 				 (turns any surface into a touchpad)
//============================================================================

/*
 * 1. point your kinect from a higher place down to your table
 * 2. start the program (keep your hands off the table for the beginning)
 * 3. use your table as a giant touchpad
 */

#include <iostream>
#include <vector>
#include <map>
using namespace std;

// openCV
#include <opencv/highgui.h>
#include <opencv/cv.h>
using namespace cv;

// openNI
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
using namespace xn;
#define CHECK_RC(rc, what)											\
	if (rc != XN_STATUS_OK)											\
	{																\
		printf("%s failed: %s\n", what, xnGetStatusString(rc));		\
		return rc;													\
	}

// TUIO
#include "TuioServer.h"
using namespace TUIO;

// TODO smoothing using kalman filter

//---------------------------------------------------------------------------
// Globals
//---------------------------------------------------------------------------

// OpenNI
xn::Context xnContext;
xn::DepthGenerator xnDepthGenerator;
xn::ImageGenerator xnImgeGenertor;

bool mousePressed = false;

//---------------------------------------------------------------------------
// Functions
//---------------------------------------------------------------------------

int initOpenNI(const XnChar* fname) {
	XnStatus nRetVal = XN_STATUS_OK;

	// initialize context
	nRetVal = xnContext.InitFromXmlFile(fname);
	CHECK_RC(nRetVal, "InitFromXmlFile");

	// initialize depth generator
	nRetVal = xnContext.FindExistingNode(XN_NODE_TYPE_DEPTH, xnDepthGenerator);
	CHECK_RC(nRetVal, "FindExistingNode(XN_NODE_TYPE_DEPTH)");

	// initialize image generator
	nRetVal = xnContext.FindExistingNode(XN_NODE_TYPE_IMAGE, xnImgeGenertor);
	CHECK_RC(nRetVal, "FindExistingNode(XN_NODE_TYPE_IMAGE)");

	return 0;
}

void average(vector<Mat1s>& frames, Mat1s& mean) {
	Mat1d acc(mean.size());
	Mat1d frame(mean.size());

	for (unsigned int i=0; i<frames.size(); i++) {
		frames[i].convertTo(frame, CV_64FC1);
		acc = acc + frame;
	}

	acc = acc / frames.size();

	acc.convertTo(mean, CV_16SC1);
}



int main() {

	const unsigned int nBackgroundTrain = 30;
	const unsigned short touchDepthMin = 10;
	const unsigned short touchDepthMax = 20;
	const unsigned int touchMinArea = 50;

	const bool localClientMode = true; 					// connect to a local client

	const double debugFrameMaxDepth = 4000; // maximal distance (in millimeters) for 8 bit debug depth frame quantization
	const char* windowName = "Debug";
	const Scalar debugColor0(0,0,128);
	const Scalar debugColor1(255,0,0);
	const Scalar debugColor2(255,255,255);

	int xMin = 110;
	int xMax = 560;
	int yMin = 120;
	int yMax = 320;

	Mat1s depth(480, 640); // 16 bit depth (in millimeters)
	Mat1b depth8(480, 640); // 8 bit depth
	Mat3b rgb(480, 640); // 8 bit depth

	Mat3b debug(480, 640); // debug visualization

	Mat1s foreground(640, 480);
	Mat1b foreground8(640, 480);

	Mat1b touch(640, 480); // touch mask

	Mat1s background(480, 640);
	vector<Mat1s> buffer(nBackgroundTrain);

	initOpenNI("../niConfig.xml");

	// TUIO server object
	TuioServer* tuio;
	if (localClientMode) {
		tuio = new TuioServer();
	} else {
		tuio = new TuioServer("192.168.0.2",3333,false);
	}
	TuioTime time;

	// create some sliders
	namedWindow(windowName);
	createTrackbar("xMin", windowName, &xMin, 640);
	createTrackbar("xMax", windowName, &xMax, 640);
	createTrackbar("yMin", windowName, &yMin, 480);
	createTrackbar("yMax", windowName, &yMax, 480);

	// create background model (average depth)
	for (unsigned int i=0; i<nBackgroundTrain; i++) {
		xnContext.WaitAndUpdateAll();
		depth.data = (uchar*) xnDepthGenerator.GetDepthMap();
		buffer[i] = depth;
	}
	average(buffer, background);

	while ( waitKey(1) != 27 ) {
		// read available data
		xnContext.WaitAndUpdateAll();

		// update 16 bit depth matrix
		depth.data = (uchar*) xnDepthGenerator.GetDepthMap();
		//xnImgeGenertor.GetGrayscale8ImageMap()



		// update rgb image
		//rgb.data = (uchar*) xnImgeGenertor.GetRGB24ImageMap(); // segmentation fault here
		//cvtColor(rgb, rgb, CV_RGB2BGR);

		// extract foreground by simple subtraction of very basic background model
		foreground = background - depth;

		// find touch mask by thresholding (points that are close to background = touch points)
		touch = (foreground > touchDepthMin) & (foreground < touchDepthMax);

		// extract ROI
		Rect roi(xMin, yMin, xMax - xMin, yMax - yMin);
		Mat touchRoi = touch(roi);

		// find touch points
		vector< vector<Point2i> > contours;
		vector<Point2f> touchPoints;
		findContours(touchRoi, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE, Point2i(xMin, yMin));
		for (unsigned int i=0; i<contours.size(); i++) {
			Mat contourMat(contours[i]);
			// find touch points by area thresholding
			if ( contourArea(contourMat) > touchMinArea ) {
				Scalar center = mean(contourMat);
				Point2i touchPoint(center[0], center[1]);
				touchPoints.push_back(touchPoint);
			}
		}

		// send TUIO cursors
		time = TuioTime::getSessionTime();
		tuio->initFrame(time);

		for (unsigned int i=0; i<touchPoints.size(); i++) { // touch points
			float cursorX = (touchPoints[i].x - xMin) / (xMax - xMin);
			float cursorY = 1 - (touchPoints[i].y - yMin)/(yMax - yMin);
			TuioCursor* cursor = tuio->getClosestTuioCursor(cursorX,cursorY);
			// TODO improve tracking (don't move cursors away, that might be closer to another touch point)
			if (cursor == NULL || cursor->getTuioTime() == time) {
				tuio->addTuioCursor(cursorX,cursorY);
			} else {
				tuio->updateTuioCursor(cursor, cursorX, cursorY);
			}
		}

		tuio->stopUntouchedMovingCursors();
		tuio->removeUntouchedStoppedCursors();
		tuio->commitFrame();

		// draw debug frame
		depth.convertTo(depth8, CV_8U, 255 / debugFrameMaxDepth); // render depth to debug frame
		cvtColor(depth8, debug, CV_GRAY2BGR);
		debug.setTo(debugColor0, touch);  // touch mask
		rectangle(debug, roi, debugColor1, 2); // surface boundaries
		for (unsigned int i=0; i<touchPoints.size(); i++) { // touch points
			circle(debug, touchPoints[i], 5, debugColor2, CV_FILLED);
		}

		// render debug frame (with sliders)
		imshow(windowName, debug);
		//imshow("image", rgb);
	}

	return 0;
}
