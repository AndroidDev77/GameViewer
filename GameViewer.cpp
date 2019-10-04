// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
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
#include "WebDataReader.h"
#include "Game.h"
#include "GameModel.h"

// Enlarges and custimizes Item on select
class GameViewDelegate : public QStyledItemDelegate
{
protected:
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QStyleOptionViewItem opt = option;
		
		initStyleOption(&opt, index);

		QString headline    = index.model()->data(index.model()->index(index.row(), 0), Qt::DisplayRole).toString(); // Headline
		QString subheadline = index.model()->data(index.model()->index(index.row(), 0), Qt::ToolTipRole).toString(); // Subheadline
		QImage image = index.model()->data(index.model()->index(index.row(), 0), Qt::ForegroundRole).value<QImage>();
		
		opt.text = "";
		QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
		style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

		QRect rect = opt.rect;
		QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
		if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
			cg = QPalette::Inactive;

		float zoomFactor = 1.0;

		// Disable Text When not Selected
		if (opt.state & QStyle::State_Selected)
		{
			painter->setPen(opt.palette.color(cg, QPalette::HighlightedText));
		}
		else 
		{
			painter->setPen(opt.palette.color(QPalette::Inactive, QPalette::Text));
			zoomFactor = 0.6666; // Scale to get 150%
		}

		// draw headline and subheadline text
		int heightBlock = rect.height() / 8; // 1 block per text and 6 blocks for image
		int widthBlock = rect.width() * (16 / 9); // 16x9

		// Set font
		QFont font("Verdana", 12);
		painter->setFont(font);

		if (opt.state & QStyle::State_Selected)
		{
			font.setBold(true);
			painter->drawText(QRect(rect.left(), rect.top(), rect.width(), heightBlock),
				Qt::AlignCenter, headline);
			font.setBold(false);
			painter->drawText(QRect(rect.left(), rect.top() + heightBlock * 7, rect.width(), heightBlock),
				Qt::AlignCenter, subheadline);
		}
		painter->drawImage(QRect(rect.left(), rect.top() + heightBlock, image.width() *zoomFactor, image.height()* zoomFactor), image);
		
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QSize result = QStyledItemDelegate::sizeHint(option, index);
		result.setHeight(result.height() * 1.5);
		return result;
	}
};

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

	QApplication app(argc, argv);

	Json::Value root;
	WebDataReader reader;
	
	std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";
	reader.ReadJSONFromURL(url, &root);

	const std::string copyright = root["copyright"].asString();
	const int totalGames        = root["totalGames"].asInt();

	std::cout << copyright << std::endl;
	std::cout << totalGames << std::endl;


	Json::Value games = root["dates"][0]["games"];
	std::vector<Game> gameList;
	Json::Value::iterator it;
	for (it = games.begin(); it != games.end(); ++it)
	{
		// Create Game
		Game game(*it);

		// Check Url is valid
		if (game.imageUrl != "")
		{
			game.image = reader.ReadImageFromURL(game.imageUrl);
		}
		gameList.push_back(game);
	}

	// Set UI Items
	QMainWindow *mainWindow = new QMainWindow();
	mainWindow->setWindowTitle(QString("Today's MLB games"));
	mainWindow->resize(1920, 1080);
	QPixmap bkgnd("mlbBackground.jpg");
	bkgnd = bkgnd.scaled(mainWindow->size(), Qt::IgnoreAspectRatio);
	QPalette palette;
	palette.setBrush(QPalette::Background, bkgnd);
	mainWindow->setPalette(palette);

	QWidget* centralWidget = new QWidget(mainWindow);

	QVBoxLayout* layout = new QVBoxLayout();
	
	GameModel* model = new GameModel();
	model->setList(&gameList);

	QListView* listView = new QListView();
	listView->setItemDelegate(new GameViewDelegate());
	listView->setModel(model);
	listView->setAutoFillBackground(false);  /* make backgrounds transparent */
	listView->viewport()->setAutoFillBackground(false);

	// Make it Horizontal
	listView->setFlow(QListView::LeftToRight);


	QScrollArea* scrollArea = new  QScrollArea();
	scrollArea->resize(1920, 300);
	scrollArea->setAutoFillBackground(false);  /* make backgrounds transparent */
	scrollArea->viewport()->setAutoFillBackground(false); 

	layout->addWidget(listView);
	layout->addWidget(scrollArea);

	centralWidget->setLayout(layout);
	mainWindow->setCentralWidget(centralWidget);

	// Set properties for selection Model
	QItemSelectionModel* selectionModel = listView->selectionModel();
	
	mainWindow->show();
	
	return app.exec();





}
