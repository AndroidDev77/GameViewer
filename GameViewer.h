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
#include <QtWidgets/QTextBrowser>
#include <Qpainter>
#include <QtWidgets/qlabel.h>


#include <json/json.h>
#include <time.h>
#include "WebDataReader.h"
#include "Game.h"
#include "GameModel.h"

class GameViewDelegate;

class GameViewer
{
public:

	GameViewer(std::string testUrl);
	~GameViewer();
	int loadGames(std::string url, GameModel* gameModel, std::vector<Game>* gameList);
	void setupUI();
	void updateTextView(QString text);
	void updateDetailImage(QImage* image);


private:
	GameModel*           gameModel[2];
	GameViewDelegate*    gameViewDelegate;
	WebDataReader*       reader;

	QMainWindow*         mainWindow;
	QListView*           listView[2];
	QVBoxLayout*         layout;
	QHBoxLayout*         hLayout;
	QTextBrowser*        textBrowser;
	QWidget*             centralWidget;
	QWidget*             bottomWidget;
	QLabel*              bottomImageWidget;
	QItemSelectionModel* selectionModel;

	

	// Games are stored in simple vector 
	// Most amount of games in a is ~30
	std::vector<Game> gameList[2];
	struct tm         timeinfo;

};

// Class to customize how Game Objects are drawn
class GameViewDelegate : public QStyledItemDelegate
{

public:
	GameViewer* gameViewerHandle;

protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

