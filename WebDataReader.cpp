#include "WebDataReader.h"


struct MemoryStruct {
	char* memory;
	size_t size;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

static size_t WriteImageMemoryCallback(void* contents, size_t size, size_t nmemb,
	void* userp) {
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*) userp;

	mem->memory = (char*)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		/* out of memory! */
		qInfo() <<"not enough memory (realloc returned NULL)\n";
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

WebDataReader::WebDataReader()
{
	curl = curl_easy_init();
	res = CURLE_OK;
}

int WebDataReader::ReadJSONFromURL(std::string url, Json::Value* root)
{

	std::string readBuffer;
	// Not Thread safe
	curl_global_init(CURL_GLOBAL_ALL);
	
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	JSONCPP_STRING err;
	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());
	if (!reader->parse(readBuffer.c_str(), readBuffer.c_str() + readBuffer.length(), root,
		&err)) {
		qInfo() << "Error Reading Image from " << url.c_str();
		return EXIT_FAILURE;

	}
	return 0;
}

QImage* WebDataReader::ReadImageFromURL(std::string url)
{
	MemoryStruct chunk;

	// Change to c++ later
	chunk.memory = (char*)malloc(1);
	chunk.size = 0;

	// Not Thread safe
	curl_global_init(CURL_GLOBAL_ALL);

	// init curl 
	curl = curl_easy_init();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

	// set callback function
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteImageMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)& chunk);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcurl-agent/1.0");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);

	// Needed to bypass https check
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);

	// perform action
	res = curl_easy_perform(curl);

	// Create Image
	QImage* image = new QImage();
	image->loadFromData((const uchar*)chunk.memory, chunk.size, "JPG");

	// Free Memory
	free(chunk.memory);

	return image;
}