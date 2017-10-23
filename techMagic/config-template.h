#pragma once
#pragma once

#define DEBUG true

enum MAGIC_RESULT
{
	HTTP_ERROR,
	KINECT_ERROR,
	IMAGE_PROCESSING_ERROR
};

//---------- ImageProcessor --------

#define DEQUE_BUFFER			32
#define TRACE_THICKNESS			4
#define BGS_HISTORY_FRAMES		250
#define ENABLE_SAVE_IMAGE		true
#define ENABLE_SPELL_TRAINING	true
#define TRAINED_SPELL_MODEL		"spellsModel.yml"

#if ENABLE_SPELL_TRAINING
#define GESTURE_TRAINER_IMAGE		"spellChars_64wide_grayscale.png"	//YOUR SPELL/GESTURE TRAINING IMAGE (STORED IN THE PROJECT FOLDER)
#define TRAINER_IMAGE_WIN_SIZE		64
#define NO_OF_IMAGES_PER_ELEMENT	20
#endif

//---------- httpService -------------

#define HUE_ADDRESS (string)"http://<YOUR HUE IP ADDRESS>"
#define HUE_USERNAME (string)"<YOUR HUE USERNAME>"
#define SPOTIFY_APP_ID (string)"<YOUR APP ID>"
#define SPOTIFY_APP_SECRET (string)"<YOUR APP SECRET KEY>"
#define SPOTIFY_ACCESS_TOKEN (string)"<YOUR ACCESS TOKEN>"
#define SPOTIFY_REFRESH_TOKEN (string)"<YOUR REFRESH TOKEN>"

typedef enum
{
	HUE_LIGHTS,
	SPOTIFY
}serviceName;

//---------- serialComm -------------
#define ARDUINO_WAIT_TIME 2000

typedef enum
{
	FLIP_BLINDS,
	LOCOMOTOR_BOT,
	IMMOBULUS_BOT
}SERIAL_CMD;