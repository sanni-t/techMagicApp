/** Reads frame data from Kinect's Long exposure IR camera **/
/** to be sent to ImageProcessor						   **/

#include "stdafx.h"
#include"KinectHandler.h"


/*fn: Initialize Kinect, use Long Exposure Infrared Frame Source	*/
/*rn: Code for result of operation									*/
HRESULT KinectHandler::init()
{

	if (FAILED(GetDefaultKinectSensor(&_kinectSensor)))
	{
		printf("Failed to find Kinect sensor");
		return KINECT_ERROR;
	}

	if (FAILED(_kinectSensor->Open()))
	{
		printf("Failed to open Kinect sensor");
		return KINECT_ERROR;
	}

	ILongExposureInfraredFrameSource* _leirFrameSource;

	if (FAILED(_kinectSensor->get_LongExposureInfraredFrameSource(&_leirFrameSource)))
	{
		printf("Error fetching IR frame source");
		return KINECT_ERROR;
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

//Release all used resources on exit
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