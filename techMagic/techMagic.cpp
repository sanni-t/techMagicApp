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

	while (1)
	{
		Mat wandTraceFrame = m_imageProcessor.getWandTrace(m_kinect.grabLongIRFrame(), m_kinect.numPixels);
		imshow(windowName, wandTraceFrame);

		if (waitKey(10) == ESC_KEY)
		{
			cout << "Exiting";
			break;
		}
	}

	m_kinect.close();
	destroyAllWindows();

	return 0;
}

