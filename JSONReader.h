#pragma once
#include <iostream>
#include <json/json.h>
#include <curl/curl.h>


class JSONReader
{
public:
	
	int ReadJSONFromURL(std::string url, Json::Value* root);
};

