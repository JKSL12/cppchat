#include "pch.h"
#include "http_client.h"

http_client* http_client::_instance = NULL;
//CRITICAL_SECTION* http_client::_lockArray = NULL;

http_client* http_client::Instance()
{
	if (!_instance) {
		_instance = new http_client();
	}

	return _instance;
}

http_client::http_client()
{
	curl_global_init(CURL_GLOBAL_ALL);
}


http_client::~http_client()
{
	curl_global_cleanup();
}

bool http_client::Request_Http_Get(__int64 id, std::string url)
{
	HRESULT hResult = E_FAIL;

	CURL* curl;
	CURLcode res;

	std::string resultString;

	try
	{
		curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_client::CallBackWriteMemory);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resultString);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK)
		{
			throw E_FAIL;
		}

		hResult = S_OK;
	}
	catch (HRESULT hr)
	{
		hResult = hr;
	}

	if (hResult != S_OK)
		return false;

	//_callbackresult(id, resultString);

	return true;
}

bool http_client::Request_Http_Post(__int64 id, std::string url, std::string postfields, std::function<void(std::string str)> func)
{
	HRESULT hResult = E_FAIL;

	CURL* curl;
	CURLcode res;

	std::string resultString;

	try
	{
		curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);

		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postfields.c_str());

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, http_client::CallBackWriteMemory);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&resultString);

		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		if (res != CURLE_OK)
		{
			throw E_FAIL;
		}

		hResult = S_OK;
	}
	catch (HRESULT hr)
	{
		hResult = hr;
	}

	if (hResult != S_OK)
		return false;

	func(resultString);
	//_callbackresult(id, resultString);

	return true;
}

size_t http_client::CallBackWriteMemory(void* ptr, size_t size, size_t nmemb, std::string* string)
{
	size_t realsize = size * nmemb;

	if (realsize <= 0)
		return 0;

	if (string != NULL)
	{
		string->append((char*)ptr, size * nmemb);
		realsize = size * nmemb;
	}

	return realsize;
}