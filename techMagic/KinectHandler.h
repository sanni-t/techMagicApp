#pragma once

#include <Kinect.h>
#include "config.h"

class KinectHandler
{

private:
	IKinectSensor* _kinectSensor = nullptr;
	IInfraredFrameReader* _irFrameReader = nullptr;
	ILongExposureInfraredFrameReader* _leirFrameReader = nullptr;

	USHORT* irData;
	IFrameDescription* _irFrameDescription;

public:
	int frameWidth;
	int frameHeight;
	UINT numPixels;
	HRESULT init();
	USHORT * grabLongIRFrame();
	void close();
};