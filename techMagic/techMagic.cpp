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
		Mat wandTraceFrame = m_imageProcessor.getWandTrace(m_kinect.grabLongIRFrame(), m_kinect.numPixels);
		imshow(windowName, wandTraceFrame);

		int keyPressed = waitKey(10);

		if (keyPressed == ESC_KEY)
		{
			cout << "Exiting";
			break;
		}
#if ENABLE_SAVE_IMAGE
		else if(keyPressed == SPACE_KEY)
		{

			String fileName = "Image" + to_string(fileNum) + ".png";
			imwrite(fileName, wandTraceFrame);
			fileNum++;
		}
#endif
	}

	m_kinect.close();
	destroyAllWindows();

	return 0;
}

