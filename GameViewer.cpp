// GameViewer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "GameViewer.h"

// Enlarges and custimizes Item on select

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Paints. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="painter">	[in,out] If non-null, the painter. </param>
/// <param name="option"> 	The option. </param>
/// <param name="index">  	Zero-based index of the. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

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
		rectTopText.moveBottom(imageRect.top() - 5);
		painter->drawText(rectTopText, Qt::AlignCenter | Qt::TextWordWrap, headline);
		font.setBold(false);
		int bottomTextTop = imageTop + heightBlock + imageHeight;
		painter->drawText(QRect(rect.left(), bottomTextTop, rect.width(), heightBlock),
			Qt::AlignCenter | Qt::TextWordWrap, subheadline);
	}
	// Draw Image in Lists
	painter->drawImage(imageRect, image);

	// Fill out Details view
	if (opt.state & QStyle::State_Selected)
	{
		
		QString blurb = index.model()->data(index.model()->index(index.row(), 0), Qt::UserRole).toString(); // Blurb

		if (gameViewerHandle != nullptr)
		{
			gameViewerHandle->updateTextView(blurb);
			gameViewerHandle->updateDetailImage(&image);
		}
	}

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Size hint. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="option">	The option. </param>
/// <param name="index"> 	Zero-based index of the. </param>
///
/// <returns>	A QSize. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

QSize GameViewDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	QSize result = QStyledItemDelegate::sizeHint(option, index);
	result.setHeight(result.height());
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Constructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="testUrl">	URL of the test. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

GameViewer::GameViewer(std::string testUrl)
{
	//TODO: combine readers and use curl_multi
	reader[0] = new WebDataReader();
	reader[1] = new WebDataReader();
	gameModel[0] = new GameModel();
	gameModel[1] = new GameModel();

	// Get Screen Size to adjust Image quality
	QRect rec = QApplication::desktop()->screenGeometry();
	qInfo() << "Screen Height: " << rec.height() << " Screen Width: " << rec.width();

	url[0] = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=YYYY-MM-DD&sportId=1";
	//std::string url = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1";

	int secDay = 0;
	// Use todays date if testUrl is empty
	if (testUrl.empty())
	{
		std::string staticURL = url[0];
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
		std::string currentUrl = url[0];
		index = currentUrl.find("YYYY-MM-DD", index);

		currentUrl.replace(index, 10, dateChar);
		url[0] = currentUrl;
		// Load Games Async
		gamesCount1 = std::async(std::launch::async,loadGames,reader[0],currentUrl,gameModel[0], &gameList[0]);
		secDay = 86400;

		time_t dayBefore = now - secDay;

		gmtime_s(&timeinfo, &dayBefore);

		// Replace YYYY-MM-DD with the day before date
		strftime(dateChar, sizeof(dateChar), "%Y-%m-%d", &timeinfo);
		index = 0;
		currentUrl = staticURL;
		index = currentUrl.find("YYYY-MM-DD", index);

		currentUrl.replace(index, 10, dateChar);
		url[1] = currentUrl;
		gamesCount2 = std::async(std::launch::async, loadGames,reader[1], currentUrl,gameModel[1], &gameList[1]);

	}
	else 
	{
		// Testing
		url[0] = testUrl;
		url[1] = "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-09&sportId=1";
		gamesCount1 = std::async(std::launch::async, loadGames, reader[0], url[0], gameModel[0], &gameList[0]);
		gamesCount2 = std::async(std::launch::async, loadGames, reader[1], url[1], gameModel[1], &gameList[1]);
	}

	// Get Date Strings to display later
	int dateIndex = url[0].find("date=");
	date[0] = url[0].substr(dateIndex + 5, 10);
	date[1] = url[1].substr(dateIndex + 5, 10);

	setupUI();
	
	// Setup gamepad
	/*QGamepadManager* gamepad_manager = QGamepadManager::instance();
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
	}*/

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

GameViewer::~GameViewer()
{
	delete reader[0];
	delete reader[1];
	delete gameModel[0];
	delete gameModel[1];
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sets up the user interface. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

void GameViewer::setupUI()
{
	// Set UI Items
	try {
		//mainWindow = new QMainWindow();
		this->setWindowTitle(QString("Today's MLB games"));

		// rework to get screensize
		this->resize(1920, 1080);
		QPixmap bkgnd("mlbBackground.jpg");
		// If we can't find background image create a black one
		if (bkgnd.isNull())
		{
			qInfo() << "Background Image not Loaded";
			bkgnd = QPixmap(1920, 1080);
			bkgnd.fill(Qt::darkBlue);
		}
		bkgnd = bkgnd.scaled(this->size(), Qt::IgnoreAspectRatio);
		QPalette palette;
		palette.setBrush(QPalette::Background, bkgnd);
		this->setPalette(palette);

		// Create Custom Item Painter
		gameViewDelegate = new GameViewDelegate();
		gameViewDelegate->gameViewerHandle = this;


		// if No games were loaded display that
		// This is actually a blocking call until loadgames returns
		// We can move this later
		std::string gameList1Label = date[0];
		std::string gameList2Label = date[1];
		try
		{
			int gameCount = gamesCount1.get();
			if (gameCount == 0)
			{
				gameList1Label += " - No Games On This Date";
			}
			gameCount = gamesCount2.get();
			if (gameCount == 0)
			{
				gameList2Label +=  " - No Games On This Date";
			}
		}
		catch (std::exception & ex)
		{
			qInfo() << ex.what();
		}

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
		bottomImageWidget->setScaledContents(true);

		// Set Size policy so text is 66% of screen and image is 33%
		QSizePolicy imageSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		imageSizePolicy.setHorizontalStretch(1);
		bottomImageWidget->setSizePolicy(imageSizePolicy);

	
		QSizePolicy textSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
		textSizePolicy.setHorizontalStretch(2);
		textBrowser->setSizePolicy(textSizePolicy);

		hLayout->addWidget(textBrowser);
		hLayout->addWidget(bottomImageWidget);


		bottomWidget->setLayout(hLayout);

		// Create Date Labels
		QFont dateTitleFont("Verdana", 16);
		QLabel* listDate1 = new QLabel();
		listDate1->setFont(dateTitleFont);
		listDate1->setStyleSheet("QLabel{color: gray}");
		listDate1->setText(gameList1Label.c_str());

		QLabel* listDate2 = new QLabel();
		listDate2->setFont(dateTitleFont);
		listDate2->setStyleSheet("QLabel{color: gray}");
		listDate2->setText(gameList2Label.c_str());

		layout = new QVBoxLayout();
		layout->addWidget(listDate1);
		layout->addWidget(listView[0]);
		layout->addWidget(listDate2);
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

int GameViewer::loadGames(WebDataReader* reader,std::string url, GameModel* gameModel,std::vector<Game> *gameList)
{
	try{
		Json::Value root;
		int res;
		int totalGames = 0;

		res = reader->ReadJSONFromURL(url, &root);
		if (res == CURLE_OK)
		{
			totalGames = root["totalGames"].asInt();

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
					res = reader->ReadImageFromURL(game.imageUrl, game.image);
				}
				if (res != CURLE_OK || game.image->isNull())
				{
					bool imageLoaded = game.image->load("genericLogo.jpg");

					if (!imageLoaded)
					{
						
						game.image->fill(Qt::gray);
					}
				}
				gameList->push_back(game);
			}
			gameModel->setList(gameList);
		}
		return totalGames;
	}
	catch (const std::exception& e)
	{
		std::cout << e.what() << std::endl;
		return 0;
	}
	

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Updates the text view described by text. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="text">	The text. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void GameViewer::updateTextView(QString text)
{
	textBrowser->setText(text);

}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Updates the detail image described by image. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="image">	[in,out] If non-null, the image. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void GameViewer::updateDetailImage(QImage* image)
{
	// from Image doesn't copy
	QSize windowSize = this->size();
	QImage scaledImage = image->scaled(windowSize.height() / 2, windowSize.width() / 2, Qt::AspectRatioMode::KeepAspectRatio);
	bottomImageWidget->setPixmap(QPixmap::fromImage(scaledImage));
	
}

// Navigates between Lists views

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Key press event. Used to Move from Different ListViews </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="event">	[in,out] If non-null, the event. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

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

