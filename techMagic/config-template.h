#pragma once

#define DEBUG false

enum MAGIC_RESULT
{
	HTTP_ERROR = -4,
	SERIAL_ERROR,
	KINECT_ERROR,
	IMAGE_PROCESSING_ERROR
};

//---------- ImageProcessor --------

#define DEQUE_BUFFER_SIZE		40	// Increase this value when saving imges for spell training
#define TRACE_THICKNESS			4
#define BGS_HISTORY_FRAMES		200
#define ENABLE_SPELL_TRAINING	false	//Do not turn on ENABLE_SAVE_IMAGE when ENABLE_SPELL_TRAINING is true & vice versa
#if !ENABLE_SPELL_TRAINING
#define ENABLE_SAVE_IMAGE		false
#endif
#define TRAINED_SPELL_MODEL		"spellsModel.yml"
#define TRAINER_IMAGE_WIN_SIZE	64

#if ENABLE_SPELL_TRAINING
#define GESTURE_TRAINER_IMAGE		"spellChars_64wide_grayscale.png"	//YOUR SPELL/GESTURE TRAINING IMAGE (STORED IN THE PROJECT FOLDER)
#define NO_OF_IMAGES_PER_ELEMENT	20
#endif

//---------- httpService -------------
#define SAVE_ACCESS_TOKEN true	
#define HUE_ADDRESS (string)"http://<YOUR HUE IP ADDRESS>"
#define HUE_USERNAME (string)"<YOUR HUE USERNAME>"
#define SPOTIFY_CLIENT_AUTH_ENCODED (string)"<YOUR ENCODED AUTH STRING>"
#define SPOTIFY_REFRESH_TOKEN (string)"<YOUR REFRESH TOKEN>"
#define SPOTIFY_PLAY_CONTENT (string)"spotify:album:7DDls7RWrCLvZgTVCJgfcq"	//Add your playlist/album spotify URI. Default- Prisoner of Azkaban OST 
#define SPOTIFY_CONTENT_OFFSET (string)"20"	//Track number. Default- Mischief managed

typedef enum
{
	HUE_LIGHTS,
	SPOTIFY
}serviceName;

//---------- serialComm -------------
#define ARDUINO_WAIT_TIME 2000

typedef enum
{
	UPDATE_SIMBLEE,
	TURN_BLINDS,
	LOCOMOTOR
}SERIAL_CMD;