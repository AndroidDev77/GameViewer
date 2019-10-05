// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GameViewer.h"

// Enlarges and custimizes Item on select

void GameViewDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QStyleOptionViewItem opt = option;

	initStyleOption(&opt, index);

	QString headline = index.model()->data(index.model()->index(index.row(), 0), Qt::DisplayRole).toString(); // Headline
	QString subheadline = index.model()->data(index.model()->index(index.row(), 0), Qt::ToolTipRole).toString(); // Subheadline
	QImage  image = index.model()->data(index.model()->index(index.row(), 0), Qt::ForegroundRole).value<QImage>(); // Image

	opt.text = "";
	QStyle* style = opt.widget ? opt.widget->style() : QApplication::style();
	style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);
	
	QRect rect = opt.rect;
	
	QPalette::ColorGroup cg = opt.state & QStyle::State_Enabled ? QPalette::Normal : QPalette::Disabled;
	if (cg == QPalette::Normal && !(opt.state & QStyle::State_Active))
		cg = QPalette::Inactive;

	double zoomFactor = 1.0;

	// Disable Text When not Selected
	if (opt.state & QStyle::State_Selected)
	{
		painter->setPen(opt.palette.color(cg, QPalette::BrightText));
	}
	else
	{
		painter->setPen(opt.palette.color(QPalette::Inactive, QPalette::Text));
		zoomFactor = 0.6666f; // Scale to get 150%
	}
	// Do calculations to center image
	QPoint center = rect.center();
	// draw headline and subheadline text
	int heightBlock = rect.height() / 8.0; // 1 block per text and 6 blocks for image

	int imageWidth = rect.width() * zoomFactor;
	int imageHeight = imageWidth * (9.0f / 16.0f);
	int imageLeft = (rect.width() - imageWidth) / 2.0f + rect.left();
	int imageTop = heightBlock;

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
	image.
	painter->drawImage(imageRect, image);

	// Fill out Details view
	QString blurb = index.model()->data(index.model()->index(index.row(), 0), Qt::UserRole).toString(); // Blurb

	if (gameViewerHandle != nullptr)
	{
		gameViewerHandle->updateTextView(blurb);
		gameViewerHandle->updateDetailImage(&image);
	}

}

QSize GameViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize result = QStyledItemDelegate::sizeHint(option, index);
	result.setHeight(result.height());
	return result;
}


GameViewer::GameViewer(std::string testUrl)
{
	//TODO: combine readers and use curl_multi
	reader[0] = new WebDataReader();
	reader[1] = new WebDataReader();
	gameModel[0] = new GameModel();
	gameModel[1] = new GameModel();
	std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=YYYY-MM-DD&sportId=1";
	//std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";

	std::future<int> gamesCount1;
	std::future<int> gamesCount2;
	int secDay = 0;
	// Use todays date if testUrl is empty
	if (testUrl.empty())
	{
		//Get Todays date
		time_t     now = time(0);
		// Load Last Day that has games

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

		// Load Games Async
		gamesCount1 = std::async(std::launch::async,loadGames,reader[0],currentUrl,gameModel[0], &gameList[0]);
		secDay = 86400;

		time_t dayBefore = now - secDay;

		gmtime_s(&timeinfo, &dayBefore);

		// Replace YYYY-MM-DD with the day before date
		strftime(dateChar, sizeof(dateChar), "%Y-%m-%d", &timeinfo);
		index = 0;
		currentUrl = url;
		index = currentUrl.find("YYYY-MM-DD", index);

		currentUrl.replace(index, 10, dateChar);
		gamesCount2 = std::async(std::launch::async, loadGames,reader[1],currentUrl,gameModel[1], &gameList[1]);

	}
	else 
	{
		// Testing
		std::string testUrl2("http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-09&sportId=1");
		gamesCount1 = std::async(std::launch::async, loadGames, reader[0], testUrl, gameModel[0], &gameList[0]);
		gamesCount2 = std::async(std::launch::async, loadGames, reader[1], testUrl2, gameModel[1], &gameList[1]);
	}
	setupUI();
	
	// Setup gamepad
	QGamepadManager* gamepad_manager = QGamepadManager::instance();
	QList<int> gamepads;
	int i = 0;
	while (i < 10)
	{
		//QApplication::processEvents();
		qInfo() << "get connected gamepads iteration : " << i;
		gamepads = gamepad_manager->connectedGamepads();
		if (!gamepads.isEmpty())
		{
			i = 10;
		}
		i++;
	}

	//auto gamepads = QGamepadManager::instance()->connectedGamepads();
	if (gamepads.isEmpty()) {
		qInfo() << "No Gamepads Detected";
		return;
	}
	else {

		gamepad = new QGamepad(gamepads[0], this);

		connect(gamepad, &QGamepad::buttonUpChanged, this, [](bool value) {
			if (value == true)
			{
				QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier, "", false, 1);
				QCoreApplication::postEvent(QApplication::focusWidget(), event);
			}
			});
		connect(gamepad, &QGamepad::buttonDownChanged, this, [](bool value) {
			if (value == true)
			{
				QKeyEvent* event = new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier, "", false, 1);
				QCoreApplication::postEvent(QApplication::focusWidget(), event);
			}
			});
	}

}

GameViewer::~GameViewer()
{
	delete reader[0];
	delete reader[1];
	delete gameModel[0];
	delete gameModel[1];
}

void GameViewer::setupUI()
{
	// Set UI Items
	try {
		//mainWindow = new QMainWindow();
		this->setWindowTitle(QString("Today's MLB games"));

		// rework to get screensize
		this->resize(1920, 1080);
		QPixmap bkgnd("mlbBackground.jpg");
		bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
		QPalette palette;
		palette.setBrush(QPalette::Background, bkgnd);
		this->setPalette(palette);

		// Create Custom Item Painter
		gameViewDelegate = new GameViewDelegate();
		gameViewDelegate->gameViewerHandle = this;

		// Create First ListView
		listView[0] = new QListView();
		listView[0]->setItemDelegate(gameViewDelegate);
		listView[0]->setModel(gameModel[0]);
		listView[0]->setAutoFillBackground(false);  /* make backgrounds transparent */
		listView[0]->viewport()->setAutoFillBackground(false);
		listView[0]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listView[0]->setLayoutMode(QListView::LayoutMode::Batched);
		listView[0]->setBatchSize(5);
		listView[0]->setStyleSheet("QListView::item:selected{background: rgb(0,0,0,0);} QListView{outline:0;}");
		listView[0]->setFrameStyle(QFrame::NoFrame);
		// Make it Horizontal
		listView[0]->setFlow(QListView::LeftToRight);

		// Create Second ListView
		listView[1] = new QListView();

		listView[1]->setItemDelegate(gameViewDelegate);
		listView[1]->setModel(gameModel[1]);
		listView[1]->setAutoFillBackground(false);  /* make backgrounds transparent */
		listView[1]->viewport()->setAutoFillBackground(false);
		listView[1]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
		listView[1]->setLayoutMode(QListView::LayoutMode::Batched);
		listView[1]->setBatchSize(5);
		listView[1]->setStyleSheet("QListView::item:selected{background: rgb(0,0,0,0);} QListView{outline:0;}");
		listView[1]->setFrameStyle(QFrame::NoFrame);

		// Make it Horizontal
		listView[1]->setFlow(QListView::LeftToRight);

		bottomWidget = new  QWidget();
		bottomWidget->resize(1920, 300);
		bottomWidget->setAutoFillBackground(false);  /* make backgrounds transparent */
		//scrollArea->viewport()->setAutoFillBackground(false);

		hLayout = new QHBoxLayout();

		textBrowser = new QTextBrowser(); /* Text panel to view game details */
		textBrowser->viewport()->setAutoFillBackground(false);
		QFont font("Verdana", 14);
		textBrowser->setCurrentFont(font);
		textBrowser->setTextColor(QColor(Qt::gray));

		bottomImageWidget = new QLabel();

		hLayout->addWidget(textBrowser);
		hLayout->addWidget(bottomImageWidget);

		bottomWidget->setLayout(hLayout);

		layout = new QVBoxLayout();
		layout->addWidget(listView[0]);
		layout->addWidget(listView[1]);
		layout->addWidget(bottomWidget);

		centralWidget = new QWidget(this);
		centralWidget->setLayout(layout);
		this->setCentralWidget(centralWidget);

		// Set properties for selection Model
		selectionModel = listView[0]->selectionModel();
		this->show();
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}

int GameViewer::loadGames(WebDataReader* reader,std::string url, GameModel* gameModel,std::vector<Game> *gameList)
{
	try{
		Json::Value root;
		reader->ReadJSONFromURL(url, &root);

		//std::string copyright = root["copyright"].asString();
		int totalGames        = root["totalGames"].asInt();

		//qInfo() << copyright.c_str() ;
		//qInfo() <<"Total Games: "<< totalGames;

		Json::Value games = root["dates"][0]["games"];
	
		gameList->clear();
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
			gameList->push_back(game);
		}
		gameModel->setList(gameList);

		return totalGames;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 0;
	}
	

}

void GameViewer::updateTextView(QString text)
{
	textBrowser->setText(text);

}

void GameViewer::updateDetailImage(QImage* image)
{
	bottomImageWidget->setPixmap(QPixmap::fromImage(*image));
}

// Navigates between Lists views
void GameViewer::keyPressEvent(QKeyEvent* event)
{
	if (event->key()== Qt::Key_Up)
	{
		listView[1]->clearSelection();
		listView[0]->setFocus();
	}
	else if (event->key() == Qt::Key_Down)
	{
		listView[0]->clearSelection();
		listView[1]->setFocus();
	}
}

