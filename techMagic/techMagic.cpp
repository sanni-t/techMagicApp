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
	hueLights.init(HUE_LIGHTS);
	spotify.init(SPOTIFY);
	
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
#if !ENABLE_SPELL_TRAINING && !ENABLE_SAVE_IMAGE
		if (m_imageProcessor.checkTraceValidity())
		{
			cout << "Trace valid for spell rcognition" << endl;

			switch (m_imageProcessor.recognizeSpell())
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
				cout << "That's not a spell" << endl;
			}
			m_imageProcessor.eraseTrace();
		}
#elif ENABLE_SPELL_TRAINING
		cout << "Perform spell recognition training on " << GESTURE_TRAINER_IMAGE << "? (Y/N) : ";
		char input;
		cin >> input;
		if (input == 'Y' || input == 'y')
		{
			m_imageProcessor.spellRecognitionTrainer();
			cout << "Spell training complete. Saved to file: " << TRAINED_SPELL_MODEL << endl;
			cout << "Press any key to exit\n";
			waitKey(0);
			cout << "Exiting";
			break;
		}
		else if (input == 'N' || input == 'n')
		{
			break;
		}
#endif
#endif

		int keyPressed = waitKey(10);
		if(keyPressed != -1)
			cout << keyPressed <<endl;
		
		if (keyPressed == ESC_KEY)
		{
			cout << "Exiting";
			break;
		}
#if DEBUG
		
		else if (keyPressed == 'u' || keyPressed == 'U')
		{
			updateSimblee();
		}
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
		else if (keyPressed >= '!' && keyPressed <= '@')
		{
			serialPort->blindsConfig(keyPressed);
		}
#endif
#if ENABLE_SAVE_IMAGE
		else if(keyPressed == SPACE_KEY)
		{
			String fileName = "Image" + to_string(fileNum) + ".png";
			Mat traceToSave = m_imageProcessor.cropResizeTrace();
			imwrite(fileName, traceToSave);
			fileNum++;
			m_imageProcessor.eraseTrace();
		}
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