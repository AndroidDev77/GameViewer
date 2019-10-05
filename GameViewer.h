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
#include <QKeyEvent>
#include <QWindow>
#include <QtWidgets/qlabel.h>
#include <QtGamepad/QGamepad>
#include <QtCore/QDebug>


#include <json/json.h>
#include <time.h>
#include <future>
#include "WebDataReader.h"
#include "Game.h"
#include "GameModel.h"

class GameViewDelegate;

class GameViewer : QMainWindow
{
public:

	GameViewer(std::string testUrl);
	~GameViewer();
	static int loadGames(WebDataReader* reader, std::string url, GameModel* gameModel, std::vector<Game>* gameList);
	void setupUI();
	void updateTextView(QString text);
	void updateDetailImage(QImage* image);
	void keyPressEvent(QKeyEvent* event);


private:
	GameModel*           gameModel[2];
	GameViewDelegate*    gameViewDelegate;
	WebDataReader*       reader[2];

	QListView*           listView[2];
	QVBoxLayout*         layout;
	QHBoxLayout*         hLayout;
	QTextBrowser*        textBrowser;
	QWidget*             centralWidget;
	QWidget*             bottomWidget;
	QLabel*              bottomImageWidget;
	QItemSelectionModel* selectionModel;

	QGamepad* gamepad;

	

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

