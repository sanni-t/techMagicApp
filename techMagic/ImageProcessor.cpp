#include "stdafx.h"
#include"ImageProcessor.h" 

void ImageProcessor::init(int width, int height)
{
	_frameWidth = width;
	_frameHeight = height;

	wandMoveTracingFrame = Mat(_frameHeight, _frameWidth, CV_8U);
	cameraFrame = Mat(_frameHeight, _frameWidth, CV_8U);

	_pMOG2 = createBackgroundSubtractorMOG2(BGS_HISTORY_FRAMES);

	// Change thresholds
	_params.minThreshold = 150;
	_params.maxThreshold = 255;

	// Fliter by color
	_params.filterByColor = true;
	_params.blobColor = 255;

	// Filter by Area.
	_params.filterByArea = true;
	_params.minArea = 0.05f;
	_params.maxArea = 20;

	// Filter by Circularity
	_params.filterByCircularity = true;
	_params.minCircularity = 0.5;

	// Filter by Convexity
	_params.filterByConvexity = true;
	_params.minConvexity = 0.5;

	// Filter by Inertia
	_params.filterByInertia = false;
	//params.minInertiaRatio = 0.01;

	_blobDetector = SimpleBlobDetector::create(_params);

	return;
}

std::vector<KeyPoint> ImageProcessor::wandDetect(ushort frameData[], int numPixels)
{
	// Storage for blobs
	std::vector<KeyPoint> keypoints;
	
	for (uint i = 0; i < numPixels; i++)
	{
		uint8_t thisPixelIntensity = (uint8_t)(frameData[i] >> 8);
		cameraFrame.at<unsigned char>(i) = thisPixelIntensity;
		wandMoveTracingFrame.at<unsigned char>(i) = 0;
	}
	if (cameraFrame.empty())
		return keypoints;

	_pMOG2->apply(cameraFrame, _fgMaskMOG2);
	Mat bgSubtractedFrame;
	cameraFrame.copyTo(bgSubtractedFrame, _fgMaskMOG2);

	// Detect blobs
	_blobDetector->detect(bgSubtractedFrame, keypoints);
	
	return keypoints;
}

Mat ImageProcessor::getWandTrace(ushort frameData[], int numpixels)
{
	std::vector<KeyPoint> keypoints = wandDetect(frameData, numpixels);


	for (int i = 0; i < keypoints.size(); i++)
	{
		if (i == 0)
		{
			if (tracePoints.size() >= DEQUE_BUFFER)
				tracePoints.pop_front();
			tracePoints.push_back(keypoints[i]);
		}
	}
	if (keypoints.size() == 0 && tracePoints.size() > 0)
	{
		tracePoints.pop_front();
	}

	for (int i = 1; i < tracePoints.size(); i++)
	{
		if (tracePoints[i].size == -99.0)
			continue;
		Point pt1(tracePoints[i - 1].pt.x, tracePoints[i - 1].pt.y);
		Point pt2(tracePoints[i].pt.x, tracePoints[i].pt.y);
		line(wandMoveTracingFrame, pt1, pt2, Scalar(255), TRACE_THICKNESS);
	}

	return wandMoveTracingFrame;
}

