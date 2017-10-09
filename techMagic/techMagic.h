#pragma once

#include"config.h"
#include"KinectHandler.h"
#include"ImageProcessor.h"
#include"httpService.h"
#include"Serial.h"

using namespace std;

#define ESC_KEY 27
#define SPACE_KEY 32

KinectHandler m_kinect;
ImageProcessor m_imageProcessor;
httpService hueLights;
Serial* serialPort = new Serial("COM3");
String windowName = "Wand Trace Window";