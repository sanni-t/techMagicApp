#pragma once
#include"curl.h"
#include"config.h"

#define SPOTIFY_URL (string)"https://api.spotify.com"

class httpService
{ 
private:
	CURL *_curl;
	CURLcode _response;
	curl_slist *headers = NULL;

	string hueUrl = HUE_ADDRESS + "/api/" + HUE_USERNAME + "/lights/1/state";
	string spotifyPlayUrl = SPOTIFY_URL + "/v1/me/player/play";
	string spotifyPauseUrl = SPOTIFY_URL + "/v1/me/player/pause";
	string spotifyAuthorization = "Authorization: Bearer "+ SPOTIFY_ACCESS_TOKEN;
	char *_JSONonText = "{ \"on\" : true}";
	char *_JSONoffText = "{ \"on\" : false}";
	char *_playData = "{\"context_uri\":\"spotify:user:1151035268:playlist:66akWBDmzVkfuKHSgbuens\",\"offset\":{\"position\":5}}";
	bool _isOn = false; //Change this to reflect true light status
	bool _isPlaying = false;

public:
	bool isOn()
	{
		return _isOn;
	}

	bool isPlaying()
	{
		return _isPlaying;
	}
	httpService(serviceName _service)
	{
		curl_global_init(CURL_GLOBAL_ALL);
		_curl = curl_easy_init();
	
		if (_curl)
		{
			if (_service == HUE_LIGHTS)
			{
				headers = curl_slist_append(headers, "Accept: application/json");
				headers = curl_slist_append(headers, "Content-Type: text/plain");
				headers = curl_slist_append(headers, "charsets: utf-8");
				curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
				curl_easy_setopt(_curl, CURLOPT_URL, hueUrl.data());
				curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
			}
			else if (_service == SPOTIFY)
			{
				cout << "Initializing Spotify service" << endl;
				headers = curl_slist_append(headers, "Accept: application/json");
				headers = curl_slist_append(headers, "Content-Type: application/json");
				headers = curl_slist_append(headers, spotifyAuthorization.data());
				curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY); 
				curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, false);
				curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
				cout << "Header:" << headers << endl;
			} 
		}
	}

	void allOn()
	{
		if (_curl)
		{
			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _JSONonText);
			curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);

			_response = curl_easy_perform(_curl);
			_isOn = true;	//TODO: true on successful status change
			cout << _curl << endl;
		}
	}

	void allOff()
	{
		if (_curl)
		{
			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _JSONoffText);
			_response = curl_easy_perform(_curl);
			_isOn = false;	//TODO: false on successful status change
		}
	}

	void play()
	{
		if (_curl)
		{
			cout << "Sending play " << endl;
			cout << spotifyPlayUrl.data() << endl;
			cout << _playData << endl;

			curl_easy_setopt(_curl, CURLOPT_URL, spotifyPlayUrl.data());
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _playData);
			//curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
			_response = curl_easy_perform(_curl);
			_isPlaying = true;
		}
	}

	void pause()
	{
		if (_curl)
		{
			cout << "Sending pause" << endl;
			curl_easy_setopt(_curl, CURLOPT_URL, spotifyPauseUrl.data());
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, "");
			//curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
			cout << _curl << endl;
			_response = curl_easy_perform(_curl);
			_isPlaying = false; 

		}
	}

	~httpService()
	{
		curl_slist_free_all(headers);
		curl_easy_cleanup(_curl);
		curl_global_cleanup();
	}
};

