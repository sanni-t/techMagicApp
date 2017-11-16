#include"stdafx.h"
#include"httpService.h"


void httpService::init (serviceName _service)
{
	curl_global_init(CURL_GLOBAL_ALL);
	_curl = curl_easy_init();

	if (_curl)
	{
		if (_service == HUE_LIGHTS)
		{
			_headers = curl_slist_append(_headers, "Accept: application/json");
			_headers = curl_slist_append(_headers, "Content-Type: text/plain");
			_headers = curl_slist_append(_headers, "charsets: utf-8");
			curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
			curl_easy_setopt(_curl, CURLOPT_URL, _hueUrl.data());
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
		}
		else if (_service == SPOTIFY)
		{
#if SAVE_ACCESS_TOKEN
			_checkSavedAccessToken();
#endif

			curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
			curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, false);
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, _responseWriter);
			curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &_responseData);
			//curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
		}
	}
}

size_t httpService::_responseWriter(void *contents, size_t size, size_t nmemb, void *userp)
{
	((string*)userp)->assign((char*)contents, size * nmemb);
	return size * nmemb;
}

bool httpService::isOn()
{
	return _isOn;
}

bool httpService::isPlaying()
{
	return _isPlaying;
}

void httpService::allOn()
{
	if (_curl)
	{
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _JSONonText);
		_response = curl_easy_perform(_curl);
		_isOn = true;	
	}
}

void httpService::allOff()
{
	if (_curl)
	{
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _JSONoffText);
		_response = curl_easy_perform(_curl);
		_isOn = false;
	}
}

void httpService::play()
{
	if (_curl)
	{
		_responseData = "";
		curl_easy_setopt(_curl, CURLOPT_URL, _spotifyPlayUrl.data());
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _playData.data());
		_response = curl_easy_perform(_curl);

		if (_response == CURLE_OK)	//Request successfully sent
		{
			if (_responseData.find("Invalid access token") != string::npos || _responseData.find("access token expired") != string::npos)
			{//_spotifyAccessToken is either expired or isn't correct
				if (_getAccessToken() == 1)
				{	
					//send request once more
					_updateSpotifyHeaders();
					curl_easy_setopt(_curl, CURLOPT_URL, _spotifyPlayUrl.data());
					curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _playData.data());
					_response = curl_easy_perform(_curl);
					if (_response == CURLE_OK)
					{
						_isPlaying = true;	//We're going to assume that play was successful
						return;
					}
				}
			}
			else
			{
				_isPlaying = true;
				return;
			}
		}
		_isPlaying = false;
	}
}

void httpService::pause()
{
	if (_curl)
	{
		_responseData = "";
		curl_easy_setopt(_curl, CURLOPT_URL, _spotifyPauseUrl.data());
		curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, "");
		_response = curl_easy_perform(_curl);
		
		if (_response == CURLE_OK)	//Request successfully sent
		{
			if (_responseData.find("Invalid access token") != string::npos || _responseData.find("access token expired") != string::npos)
			{//_spotifyAccessToken is either expired or isn't correct
				if (_getAccessToken() == 1)
				{
					//send request once more
					_updateSpotifyHeaders();
					curl_easy_setopt(_curl, CURLOPT_URL, _spotifyPauseUrl.data());
					curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, "");
					_response = curl_easy_perform(_curl);
					if (_response == CURLE_OK)
					{
						_isPlaying = false;	//We're going to assume that play was successful
						return;
					}
				}
			}
			else
			{
				_isPlaying = false;
				return;
			}
		}
	}
}

void httpService::_updateSpotifyHeaders()
{
	if (_curl)
	{
		_spotifyAuthorization = "Authorization: Bearer " + _spotifyAccessToken;
		_headers = NULL;
		_headers = curl_slist_append(_headers, "Accept: application/json");
		_headers = curl_slist_append(_headers, "Content-Type: application/json");
		_headers = curl_slist_append(_headers, _spotifyAuthorization.data());
		curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);
	}
}

int httpService::_getAccessToken()
{
	CURL *_curlAlt;
	CURLcode _altResponse;
	_curlAlt = curl_easy_init();
	curl_slist *_headersAlt = NULL;
	string _refreshTokenAuth = "Authorization: Basic " + SPOTIFY_CLIENT_AUTH_ENCODED;
	string _refreshTokenReqData = "grant_type=refresh_token&refresh_token=" + SPOTIFY_REFRESH_TOKEN;
	_headersAlt = curl_slist_append(_headersAlt, _refreshTokenAuth.data());
	_responseData = "";	//Clear previous response
	if (_curlAlt)
	{
		curl_easy_setopt(_curlAlt, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
		curl_easy_setopt(_curlAlt, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(_curlAlt, CURLOPT_HTTPHEADER, _headersAlt);
		curl_easy_setopt(_curlAlt, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(_curlAlt, CURLOPT_URL, "https://accounts.spotify.com/api/token");
		curl_easy_setopt(_curlAlt, CURLOPT_POSTFIELDS, _refreshTokenReqData.data());
		curl_easy_setopt(_curlAlt, CURLOPT_WRITEFUNCTION, _responseWriter);
		curl_easy_setopt(_curlAlt, CURLOPT_WRITEDATA, &_responseData);
		_altResponse = curl_easy_perform(_curlAlt);

		curl_slist_free_all(_headersAlt);
		curl_easy_cleanup(_curlAlt);

		if (_altResponse == CURLE_OK)	//Request successfully sent
		{
			size_t searchResultPos = _responseData.find("\"access_token\":");
			if (searchResultPos != string::npos)
			{	//Received an access token
				_spotifyAccessToken = _responseData.substr(searchResultPos + 16, SPOTIFY_TOKEN_LENGTH);
				cout << "Received access token: \"" << _spotifyAccessToken << "\""<< endl;
#if SAVE_ACCESS_TOKEN
				_saveTokenToFile();
#endif
				return 1;
			}
			else
			{
				cout << "Didn't receive access code";
				return -1;
			}
		}
	}
}

#if SAVE_ACCESS_TOKEN
void httpService::_checkSavedAccessToken()
{
	_spotifyDataFile.open(SPOTIFY_TOKEN_FILE);
	streampos _fileBeg = _spotifyDataFile.tellg();
	_spotifyDataFile.seekg(0, ios::end);
	streampos _fileEnd = _spotifyDataFile.tellg();
	if (_spotifyDataFile.is_open())
	{
		if (_fileEnd - _fileBeg != SPOTIFY_TOKEN_LENGTH)
		{
			_getAccessToken();
		}
		else
		{
			_spotifyDataFile.seekg(0);
			stringstream ss;
			ss << _spotifyDataFile.rdbuf();
			_spotifyAccessToken = ss.str();
		}
		_spotifyDataFile.close();
	}
	else
	{
		cout << "File not opened" << endl;
	}
}
void httpService::_saveTokenToFile()
{
	if (!_spotifyDataFile.is_open())
	{
		_spotifyDataFile.open(SPOTIFY_TOKEN_FILE);
	}
	_spotifyDataFile.seekp(0);
	_spotifyDataFile << _spotifyAccessToken;
	_spotifyDataFile.close();

}
#endif

httpService::~httpService()
{
	curl_slist_free_all(_headers);
	curl_easy_cleanup(_curl);
	curl_global_cleanup();
}