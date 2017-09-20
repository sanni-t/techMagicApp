#pragma once

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/video.hpp"

using namespace cv;
using namespace std;

#define DEQUE_BUFFER 32
#define TRACE_THICKNESS 4
#define BGS_HISTORY_FRAMES 250

class ImageProcessor
{
private:
	int _frameWidth;
	int _frameHeight;
	Mat _fgMaskMOG2;
	Ptr<BackgroundSubtractor> _pMOG2;
	
	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params _params;
	Ptr<SimpleBlobDetector> _blobDetector;

	std::vector<KeyPoint> wandDetect(ushort[], int _numpixels);
	std::deque<KeyPoint> tracePoints;
	Mat wandMoveTracingFrame;
	Mat cameraFrame;

public:
	void init(int frameWidth, int frameHeight);
	Mat getWandTrace(ushort[], int _numpixels);

};