#pragma once
//Used to send HTTP REST requests to Hue lights and Spotify service

#include"curl.h"
#include"config.h"

#define SPOTIFY_URL (string)"https://api.spotify.com"
#define SPOTIFY_TOKEN_FILE (string)"__spfy.txt"
#define SPOTIFY_TOKEN_LENGTH (int)147

using namespace std;

class httpService
{ 
private:
	CURL *_curl;
	CURLcode _response;
	curl_slist *_headers = NULL;

	string _hueUrl = HUE_ADDRESS + "/api/" + HUE_USERNAME + "/lights/1/state";
	string _spotifyAccessToken = "";
	string _spotifyPlayUrl = SPOTIFY_URL + "/v1/me/player/play";
	string _spotifyPauseUrl = SPOTIFY_URL + "/v1/me/player/pause";
	string _spotifyAuthorization;
	string _responseData;
	string _playData = "{\"context_uri\":\"" + SPOTIFY_PLAY_CONTENT + "\",\"offset\":{\"position\":" + SPOTIFY_CONTENT_OFFSET + "}}";
	
	fstream _spotifyDataFile;
	char *_spotifyClientAuthEncoded;
	char *_JSONonText = "{ \"on\" : true, \"bri\": 254}";
	char *_JSONoffText = "{ \"on\" : false}";
	bool _isOn = false; //Change this to reflect true light status
	bool _isPlaying = false;
	
	static size_t _responseWriter(void *contents, size_t size, size_t nmemb, void *userp);
	int _getAccessToken();

	void _updateSpotifyHeaders();
#if SAVE_ACCESS_TOKEN
	void _checkSavedAccessToken();
	void _saveTokenToFile();
#endif

public:
	void init(serviceName _service);
	bool isOn();
	bool isPlaying();
	void allOn();
	void allOff();
	void play();
	void pause();
	~httpService();
};

