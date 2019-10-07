////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	GameViewer.h
//
// summary:	Declares the game viewer class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator> 
#include <time.h>
#include <future>
#include <ctime>

#include <QtWidgets/QApplication>
#include <QtWidgets/qdesktopwidget.h>
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
#include <QtWidgets/QSizePolicy.h>
#include <QKeyEvent>
#include <QWindow>
#include <QtWidgets/qlabel.h>
#include <QtGamepad/QGamepad>
#include <QtCore/QDebug>

#include <json/json.h>

#include "WebDataReader.h"
#include "Game.h"
#include "GameModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A game view delegate. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class GameViewDelegate;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A game viewer. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class GameViewer : QMainWindow
{
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="testUrl">	URL of the test. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	GameViewer(std::string testUrl);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Destructor. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~GameViewer();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Loads the games. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="reader">   	[in,out] If non-null, the reader. </param>
	/// <param name="url">			URL of the resource. </param>
	/// <param name="gameModel">	[in,out] If non-null, the game model. </param>
	/// <param name="gameList"> 	[in,out] If non-null, list of games. </param>
	///
	/// <returns>	The games. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	static int loadGames(WebDataReader* reader, std::string url, GameModel* gameModel, std::vector<Game>* gameList);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Sets up the user interface. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void setupUI();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Updates the text view described by text. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="text">	The text. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void updateTextView(QString text);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Updates the detail image described by image. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="image">	[in,out] If non-null, the image. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void updateDetailImage(QImage* image);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Key press event. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="event">	[in,out] If non-null, the event. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	void keyPressEvent(QKeyEvent* event);


private:
	/// <summary>	The game model[ 2]. </summary>
	GameModel*           gameModel[2];
	/// <summary>	The game view delegate that provides a custom paint. </summary>
	GameViewDelegate*    gameViewDelegate;
	/// <summary>	The reader[ 2]. </summary>
	WebDataReader*       reader[2];

	/// <summary>	The list view[ 2]. </summary>
	QListView*           listView[2];
	/// <summary>	The vertical layout. </summary>
	QVBoxLayout*         layout;
	/// <summary>	The horizontal layout. </summary>
	QHBoxLayout*         hLayout;
	/// <summary>	The text browser at bottom of window. </summary>
	QTextBrowser*        textBrowser;
	/// <summary>	The central widget of ListViews. </summary>
	QWidget*             centralWidget;
	/// <summary>	The bottom widget. </summary>
	QWidget*             bottomWidget;
	/// <summary>	The bottom image widget. </summary>
	QLabel*              bottomImageWidget;
	/// <summary>	The selection model. </summary>
	QItemSelectionModel* selectionModel;

	/// <summary>	The gamepad. </summary>
	QGamepad* gamepad;

	

	// Games are stored in simple vector 
	// Most amount of games in a is ~30
	/// <summary>	The game list[ 2]. </summary>
	std::vector<Game> gameList[2];

	// Today and Yesterdays date in MM-DD-YYYY
	std::string date[2];
	std::string url[2];

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	A time. </summary>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	struct tm         timeinfo;

	// Number of Games for Day listed and day before
	std::future<int> gamesCount1;
	std::future<int> gamesCount2;

};

// 

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Class to customize how Game Objects are drawn </summary>
///
////////////////////////////////////////////////////////////////////////////////////////////////////

class GameViewDelegate : public QStyledItemDelegate
{

public:
	/// <summary>	Handle of the game viewer. </summary>
	GameViewer* gameViewerHandle;

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Paints. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="painter">	[in,out] If non-null, the painter. </param>
	/// <param name="option"> 	The option. </param>
	/// <param name="index">  	Zero-based index of the. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const;

};

