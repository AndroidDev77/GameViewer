#include <QtWidgets/QApplication>
#include "GameViewer.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	GameViewer* gameViewer = new GameViewer();
	
	return app.exec();
}
