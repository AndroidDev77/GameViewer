////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	WebDataReader.cpp
//
// summary:	Implements the web data reader class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "WebDataReader.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A memory structure. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

struct MemoryStruct {
	/// <summary>	The memory poiter. </summary>
	char* memory;
	/// <summary>	The size. </summary>
	size_t size;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Function callback, that writes data to userp from contents. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="contents">	[in,out] Memory from curl call. </param>
/// <param name="size">	   	The size. </param>
/// <param name="nmemb">   	Number of memory address. </param>
/// <param name="userp">   	[in,out] User provided memory address. </param>
///
/// <returns>	A size_t. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

static size_t WriteStringCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Writes an image memory callback. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="contents">	[in,out] Memory from curl call. </param>
/// <param name="size">	   	The size. </param>
/// <param name="nmemb">   	Number of memory address. </param>
/// <param name="userp">   	[in,out] User provided memory address. </param>
///
/// <returns>	A size_t. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

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

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Default constructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

WebDataReader::WebDataReader()
{
	curl = curl_easy_init();
	res = CURLE_OK;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Reads JSON from URL. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="url"> 	URL of the resource. </param>
/// <param name="root">	[in,out] If non-null, the root. </param>
///
/// <returns>	The JSON from URL. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int WebDataReader::ReadJSONFromURL(std::string url, Json::Value* root)
{

	std::string readBuffer;
	// Not Thread safe
	curl_global_init(CURL_GLOBAL_ALL);
	
	curl = curl_easy_init();
	if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteStringCallback);
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
	return res;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Reads image from URL. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="url">	URL of the resource. </param>
///  <param name="image">	QImage that's allocated. </param>
///
/// <returns>	curl response code </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int WebDataReader::ReadImageFromURL(std::string url, QImage* image)
{
	MemoryStruct chunk;

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

	// Clean up
	curl_easy_cleanup(curl);

	if (res == CURLE_OK)
	{
		// Create Image
		image->loadFromData((const uchar*)chunk.memory, chunk.size, "JPG");
	}
	// Free Memory
	free(chunk.memory);

	return res;
}