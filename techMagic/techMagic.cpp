// techMagic.cpp : Defines the entry point for the console application.

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
			switch (m_imageProcessor.recognizeSpell())
			{
			case 0:
				musicToggle();
				break;
			case 1:
				blindsToggle();
				break;
			case 2:
				botToggle();
				break;
			case 3:
				lightsToggle();
				break;
			default:
				cout << "That's not a spell" << endl;
			}
		}
#endif

		int keyPressed = waitKey(10);

		if (keyPressed == ESC_KEY)
		{
			cout << "Exiting";
			break;
		}

#if DEBUG
		else if (keyPressed == 'l' || keyPressed == 'L')
		{ 
			lightsToggle();
		}
		else if (keyPressed == 'b' || keyPressed == 'B')
		{
			blindsToggle();
		}
		else if (keyPressed == 'm' || keyPressed == 'M')
		{
			musicToggle();
		}
		else if (keyPressed == 't' || keyPressed == 'T')
		{
			botToggle();
		}
#else
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

void lightsToggle() {
	if (hueLights.isOn())
		hueLights.allOff();
	else
		hueLights.allOn();
}

void blindsToggle()
{
	bool dataSent =	serialPort->sendCommand(FLIP_BLINDS);
	cout << "Bytes sent:" << dataSent << endl;
}

void botToggle()
{
	bool dataSent = serialPort->sendCommand(LOCOMOTOR_BOT);
}

void musicToggle()
{
	if (spotify.isPlaying())
	{
		spotify.pause();
	}
	else
	{
		spotify.play();
	}
}