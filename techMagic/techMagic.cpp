// techMagic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "techMagic.h"


int main()
{
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	if (FAILED(m_kinect.init()))
	{
		cout << "Failed to initialize Kinect";
		return 0;
	}

	m_imageProcessor.init(m_kinect.frameWidth, m_kinect.frameHeight);

#if ENABLE_SAVE_IMAGE
	int fileNum = 0;
#endif

	while (1)
	{
#if !DEBUG
		vector<KeyPoint> blobKeypoints;
		int trailPointsDequeSize;
		Point traceUpperCorner;
		Point traceLowerCorner;
		Mat wandTraceFrame = m_imageProcessor.getWandTrace(m_kinect.grabLongIRFrame(), m_kinect.numPixels);
		
		imshow(windowName, wandTraceFrame);

		if(m_imageProcessor.checkTraceValidity())
		{
			cout << "Trace Valid" << endl;
			m_imageProcessor.recognizeSpell();
		}
#endif

		int keyPressed = waitKey(10);

		if (keyPressed == ESC_KEY)
		{
			cout << "Exiting";
			break;
		}
		else if (keyPressed == SPACE_KEY)
		{ 
			if (hueLights.isOn())
				hueLights.allOff();
			else
				hueLights.allOn();
		}
		else if (keyPressed == 's')
		{
			bool dataSent =serialPort->sendCommand(OPEN_BLINDS);
			cout << "Bytes sent:" << dataSent << endl;
		}
#if !DEBUG
#if ENABLE_SAVE_IMAGE

		else if(keyPressed == 'L')
		{

			String fileName = "Image" + to_string(fileNum) + ".png";
			imwrite(fileName, wandTraceFrame);
			fileNum++;
		}
#endif
#endif
	}
#if !DEBUG
	m_kinect.close();
#endif
	destroyAllWindows();

	return 0;
}

