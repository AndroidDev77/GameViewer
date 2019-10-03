// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <list>
#include <iterator> 
#include <json/json.h>
#include "JSONReader.h"
#include "Game.h"

int main()
{
    std::cout << "Hello World!\n";
	
	Json::Value root;
	JSONReader reader;
	
	std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";
	reader.ReadJSONFromURL(url, &root);

	const std::string copyright = root["copyright"].asString();
	const int totalGames        = root["totalGames"].asInt();
	

	Json::Value games = root["dates"][0]["games"];
	std::list<Game> gameList;
	Json::Value::iterator it;
	for (it = games.begin(); it != games.end(); ++it)
	{
		// Create Game
		Game game(*it);
		gameList.push_back(game);
	}


	std::cout << copyright << std::endl;
	std::cout << totalGames << std::endl;

}
