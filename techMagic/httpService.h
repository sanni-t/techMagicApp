#pragma once
#include"curl.h"

class httpService
{ 
private:
	CURL *_curl;
	CURLcode _response;
	curl_slist *headers = NULL;

	string url = HUE_ADDRESS + "/api/" + HUE_USERNAME + "/lights/1/state";
	char *_JSONonText = "{ \"on\" : true}";
	char *_JSONoffText = "{ \"on\" : false}";
	bool _isOn = false; //Change this to reflect true light status
	
public:
	bool isOn()
	{
		return _isOn;
	}

	httpService()
	{
		curl_global_init(CURL_GLOBAL_ALL);
		_curl = curl_easy_init();
		

		headers = curl_slist_append(headers, "Accept: application/json");
		headers = curl_slist_append(headers, "Content-Type: text/plain");
		headers = curl_slist_append(headers, "charsets: utf-8");

		if (_curl)
		{
			curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
			curl_easy_setopt(_curl, CURLOPT_URL, url.data());
			curl_easy_setopt(_curl, CURLOPT_CUSTOMREQUEST, "PUT");
		}
	}

	void allOn()
	{
		if (_curl)
		{
			curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _JSONonText);
			_response = curl_easy_perform(_curl);
			_isOn = true;	//TODO: true on successful status change
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

	~httpService()
	{
		curl_slist_free_all(headers);
		curl_easy_cleanup(_curl);
		curl_global_cleanup();
	}
};

