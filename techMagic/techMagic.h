#pragma once

#include"config.h"
#include"KinectHandler.h"
#include"ImageProcessor.h"
#include"httpService.h"
#include"Serial.h"

using namespace std;
//using namespace std::chrono;

#define ESC_KEY 27
#define SPACE_KEY 32
#define ALT_KEY 18

KinectHandler m_kinect;
ImageProcessor m_imageProcessor;
httpService hueLights;
httpService spotify;
Serial* serialPort = new Serial("COM9");
String windowName = "Wand Trace Window";
//C++11 Style:***
//steady_clock::time_point lastKeyPoint = steady_clock::now();
//std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();


void musicToggle();
void blindsToggle();
void botToggle();
void lightsToggle();
void updateSimblee();