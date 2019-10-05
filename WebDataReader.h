#pragma once
#include <iostream>
#include <QPixmap>
#include <json/json.h>
#include <curl/curl.h>
#include <QtCore/QDebug>


class WebDataReader
{
public:
	WebDataReader();
	int ReadJSONFromURL(std::string url, Json::Value* root);
	QImage* ReadImageFromURL(std::string url);

private:
	CURL* curl;
	CURLcode res;

};

