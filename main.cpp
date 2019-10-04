#include <QtWidgets/QApplication>
#include "GameViewer.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	std::string testUrl = "";
	if (argc > 1)
	{
		testUrl = argv[1];
	}
	GameViewer* gameViewer = new GameViewer(testUrl);
	
	return app.exec();
}
