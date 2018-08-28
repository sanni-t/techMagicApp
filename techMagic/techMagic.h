#pragma once

#include"config.h"
#include"KinectHandler.h"
#include"ImageProcessor.h"
#if !ENABLE_SAVE_IMAGE && !ENABLE_SPELL_TRAINING
#include"httpService.h"
#include"Serial.h"
#endif

using namespace std;

#define ESC_KEY 27
#define SPACE_KEY 32
#define ALT_KEY 18

KinectHandler m_kinect;
ImageProcessor m_imageProcessor;

#if !ENABLE_SAVE_IMAGE && !ENABLE_SPELL_TRAINING
httpService hueLights;
httpService spotify;
Serial* serialPort = new Serial("COM8");
#endif

String windowName = "Wand Trace Window";

void triggerAction(int);
void musicToggle();
void blindsToggle();
void botToggle();
void lightsToggle();
void updateSimblee();