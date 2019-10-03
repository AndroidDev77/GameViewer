// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <QGuiApplication>
#include <QtWidgets/QListView.h>  
#include <QtWidgets/QSplitter.h> 
#include <QtWidgets/QFileSystemModel.h>  

#include <iostream>
#include <string>
#include <list>
#include <iterator> 
#include <json/json.h>
#include "JSONReader.h"
#include "Game.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";
	
	Json::Value root;
	JSONReader reader;
	
	std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";
	reader.ReadJSONFromURL(url, &root);

	const std::string copyright = root["copyright"].asString();
	const int totalGames        = root["totalGames"].asInt();

	std::cout << copyright << std::endl;
	std::cout << totalGames << std::endl;


	Json::Value games = root["dates"][0]["games"];
	std::list<Game> gameList;
	Json::Value::iterator it;
	for (it = games.begin(); it != games.end(); ++it)
	{
		// Create Game
		Game game(*it);
		gameList.push_back(game);
	}

	QGuiApplication app(argc, argv);
	QSplitter* splitter = new QSplitter;

	QFileSystemModel* model = new QFileSystemModel();
	model->setRootPath(QDir::currentPath());

	QListView* list = new QListView(splitter);
	list->setModel(model);
	list->setRootIndex(model->index(QDir::currentPath()));

	return app.exec();





}
