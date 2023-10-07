#pragma once
class http_client
{
public:
	http_client();
	~http_client();

	bool Request_Http_Get(__int64 id, std::string url);
	bool Request_Http_Post(__int64 id, std::string url, std::string postfields, std::function<void(std::string str)> func);

private:
	static size_t CallBackWriteMemory(void* ptr, size_t size, size_t nmemb, std::string* string);

public:
	static http_client* Instance();
private:
	static http_client* _instance;
};

