#pragma once
#include <iostream>
#include <QPixmap>
#include <json/json.h>
#include <curl/curl.h>


class WebDataReader
{
public:
	
	int ReadJSONFromURL(std::string url, Json::Value* root);
	QImage ReadImageFromURL(std::string url);
};

