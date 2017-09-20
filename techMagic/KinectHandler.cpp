#include "stdafx.h"
#include"KinectHandler.h"


//Returns HRESULT of 
HRESULT KinectHandler::init()
{

	if (FAILED(GetDefaultKinectSensor(&_kinectSensor)))
	{
		printf("Failed to find Kinect sensor");
		return -3;
	}

	if (FAILED(_kinectSensor->Open()))
	{
		printf("Failed to open Kinect sensor");
		return -2;
	}

	ILongExposureInfraredFrameSource* _leirFrameSource;

	if (FAILED(_kinectSensor->get_LongExposureInfraredFrameSource(&_leirFrameSource)))
	{
		printf("Error fetching IR frame source");
		return -3;
	}

	_leirFrameSource->get_FrameDescription(&_irFrameDescription);
	_irFrameDescription->get_LengthInPixels(&numPixels);
	_irFrameDescription->get_Width(&frameWidth);
	_irFrameDescription->get_Height(&frameHeight);

	irData = new USHORT[numPixels];

	HRESULT _hResult;
	_hResult = _leirFrameSource->OpenReader(&_leirFrameReader);
	SafeRelease(_leirFrameSource);

	return _hResult;
}

void KinectHandler::close()
{
	_kinectSensor->Close();
	SafeRelease(_leirFrameReader);
	SafeRelease(_kinectSensor);
}

USHORT* KinectHandler::grabLongIRFrame()
{
	ILongExposureInfraredFrame* leirFrame;

	if (FAILED(_leirFrameReader->AcquireLatestFrame(&leirFrame)))
		return irData;

	leirFrame->CopyFrameDataToArray(numPixels, irData);

	SafeRelease(leirFrame);

	return irData;
}