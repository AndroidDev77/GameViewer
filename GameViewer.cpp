// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GameViewer.h"

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
		// Do calculations to center image
		QPoint center = rect.center();
		// draw headline and subheadline text
		int heightBlock = rect.height() / 8.0; // 1 block per text and 6 blocks for image

		int imageWidth = rect.width() * zoomFactor;
		int imageHeight = imageWidth * (9.0 / 16.0);
		int imageLeft = (rect.width() - imageWidth) / 2.0 + rect.left();
		int imageTop = heightBlock;//(rect.height() - imageHeight) / 2.0 + rect.top();

		QRect imageRect(imageLeft, imageTop + heightBlock, imageWidth, imageHeight);
		// Set font
		QFont font("Verdana", 8);
		painter->setFont(font);
		if (opt.state & QStyle::State_Selected)
		{
			font.setBold(true);
			QRect rectTopText(rect.left(), rect.top(), rect.width(), heightBlock);
			rectTopText.moveBottom(imageRect.top());
			painter->drawText(rectTopText, Qt::AlignCenter | Qt::TextWordWrap, headline);
			font.setBold(false);
			int bottomTextTop = imageTop + heightBlock + imageHeight;
			painter->drawText(QRect(rect.left(), bottomTextTop, rect.width(), heightBlock),
				Qt::AlignCenter | Qt::TextWordWrap, subheadline);
		}

		painter->drawImage(imageRect, image);
		
	}

	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
	{
		QSize result = QStyledItemDelegate::sizeHint(option, index);
		result.setHeight(result.height() );
		return result;
	}
};

GameViewer::GameViewer()
{
	reader = new WebDataReader();
	model = new GameModel();
	std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=YYYY-MM-DD&sportId=1";
	//std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";

	int gamesCount = 0;
	int secDay = 0;
	//Get Todays date
	time_t     now = time(0);
	// Load Last Day that has games
	while (gamesCount <= 0)
	{
		// Substract day if no games today
		now -= secDay;
		char       dateChar[11];
		gmtime_s(&timeinfo, &now);

		// Replace YYYY-MM-DD with todays date
		strftime(dateChar, sizeof(dateChar), "%Y-%m-%d", &timeinfo);
		size_t index = 0;
		std::string currentUrl = url;
		index = currentUrl.find("YYYY-MM-DD", index);

		currentUrl.replace(index, 10, dateChar);
		gamesCount = loadGames(currentUrl);
		secDay = 86400;
	}
	//gamesCount = loadGames(url);
	setupUI();
	

}

GameViewer::~GameViewer()
{
	delete reader;
	delete model;
}

void GameViewer::setupUI()
{
	// Set UI Items
	try {
		QMainWindow* mainWindow = new QMainWindow();
		mainWindow->setWindowTitle(QString("Today's MLB games"));

		// rework to get screensize
		mainWindow->resize(1920, 1080);
		QPixmap bkgnd("mlbBackground.jpg");
		bkgnd = bkgnd.scaled(mainWindow->size(), Qt::IgnoreAspectRatio);
		QPalette palette;
		palette.setBrush(QPalette::Background, bkgnd);
		mainWindow->setPalette(palette);

		// Create ListView
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

		QVBoxLayout* layout = new QVBoxLayout();
		layout->addWidget(listView);
		layout->addWidget(scrollArea);

		QWidget* centralWidget = new QWidget(mainWindow);
		centralWidget->setLayout(layout);
		mainWindow->setCentralWidget(centralWidget);

		// Set properties for selection Model
		QItemSelectionModel* selectionModel = listView->selectionModel();

		mainWindow->show();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int GameViewer::loadGames(std::string url)
{
	try{
		Json::Value root;
		reader->ReadJSONFromURL(url, &root);

		std::string copyright = root["copyright"].asString();
		int totalGames        = root["totalGames"].asInt();

		std::cout << copyright << std::endl;
		std::cout <<"Total Games: "<< totalGames << std::endl;

		Json::Value games = root["dates"][0]["games"];
	
		gameList.clear();
		Json::Value::iterator it;
		for (it = games.begin(); it != games.end(); ++it)
		{
			// Create Game
			Game game(*it);

			// Check Url is valid
			if (game.imageUrl != "")
			{
				game.image = reader->ReadImageFromURL(game.imageUrl);
			}
			if (game.image == nullptr)
			{
				game.image = new QImage("genericLogo.jpg");
			}
			gameList.push_back(game);
		}
		model->setList(&gameList);

		return totalGames;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

}
