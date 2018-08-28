// techMagic.cpp : Defines the entry point for the console application.

#include "stdafx.h"
#include "techMagic.h"


int main()
{
	m_imageProcessor.init(m_kinect.frameWidth, m_kinect.frameHeight);

#if ENABLE_SPELL_TRAINING
	cout << "Perform spell recognition training on " << GESTURE_TRAINER_IMAGE << "? (Y for yes) : ";
	char input;
	cin >> input;
	if (input == 'Y' || input == 'y')
	{
		m_imageProcessor.spellRecognitionTrainer();
		cout << "Spell training complete. Saved to file: " << TRAINED_SPELL_MODEL << endl;
		cout << "Enter any key to exit\n";
		char keyIn;
		cin >> keyIn;
	}
#else
	namedWindow(windowName, CV_WINDOW_AUTOSIZE);

	if (FAILED(m_kinect.init()))
	{
		cout << "Failed to initialize Kinect";
		return 0;
	}

#if ENABLE_SAVE_IMAGE
	int fileNum = 0;
#else
	hueLights.init(HUE_LIGHTS);
	spotify.init(SPOTIFY);
#endif

	while (1)
	{
	int keyPressed = waitKey(10);
	if (keyPressed != -1)
		cout << keyPressed << endl;

	if (keyPressed == ESC_KEY)
	{
		cout << "Exiting";
		break;
	}
#if !DEBUG
	vector<KeyPoint> blobKeypoints;
	int trailPointsDequeSize;
	Point traceUpperCorner;
	Point traceLowerCorner;
	Mat wandTraceFrame = m_imageProcessor.getWandTrace(m_kinect.grabLongIRFrame(), m_kinect.numPixels);
		
	imshow(windowName, wandTraceFrame);
		
#if ENABLE_SAVE_IMAGE
	if (keyPressed == SPACE_KEY)
	{
		String fileName = "Image" + to_string(fileNum) + ".png";
		Mat traceToSave = m_imageProcessor.cropResizeTrace();
		imwrite(fileName, traceToSave);
		fileNum++;
		m_imageProcessor.eraseTrace();
	}
	else if (keyPressed == 'x' || keyPressed == 'X')
	{
		m_imageProcessor.eraseTrace();
	}
#else
	if (m_imageProcessor.checkTraceValidity())
	{
		cout << "Trace valid for spell rcognition" << endl;
		triggerAction(m_imageProcessor.recognizeSpell());
		m_imageProcessor.eraseTrace();
	}
#endif	//ENABLE_SAVE_IMAGE
#else
	triggerAction(keyPressed - 0x30);
#endif	//DEBUG
}
	m_kinect.close();
	destroyAllWindows();
#endif	//!ENABLE_SPELL_TRAINER
	return 0;
}

#if !ENABLE_SAVE_IMAGE && !ENABLE_SPELL_TRAINING
void lightsToggle() {
	if (hueLights.isOn())
		hueLights.allOff();
	else
		hueLights.allOn();
}

void blindsToggle()
{
	bool dataSent =	serialPort->sendCommand(TURN_BLINDS);
}

void updateSimblee()
{
	bool dataSent = serialPort->sendCommand(UPDATE_SIMBLEE);
}
void botToggle()
{
	bool dataSent = serialPort->sendCommand(LOCOMOTOR);
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

void triggerAction(int index)
{
	switch (index)
	{
	case 0:
		cout << "*** 0: Music Toggle ***" << endl;
		musicToggle();
		break;
	case 1:
		cout << "*** 1: Blinds Toggle ***" << endl;
		blindsToggle();
		break;
	case 2:
		cout << "*** 2: Bot Move ***" << endl;
		botToggle();
		break;
	case 3:
		cout << "*** 3: Lights Toggle ***" << endl;
		lightsToggle();
		break;
	default:
		break;
		//Not a spell. Do nothing
	}
}
#endif