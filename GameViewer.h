#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator> 
#include <QtWidgets/QApplication>
#include <QGuiApplication>
#include <QtWidgets/QListView>  
#include <QtWidgets/QSplitter> 
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QScrollArea>
#include <QtCore/QStringListModel>  
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStyledItemDelegate>
#include <Qpainter>


#include <json/json.h>
#include <time.h>
#include "WebDataReader.h"
#include "Game.h"
#include "GameModel.h"

class GameViewer
{
public:

	GameViewer();
	~GameViewer();
	int loadGames(std::string url);
	void setupUI();

private:
	GameModel* model;
	WebDataReader* reader;
	
	// Games are stored in simple vector 
	// Most amount of games in a is ~30
	std::vector<Game> gameList;
	struct tm  timeinfo;
	
};
