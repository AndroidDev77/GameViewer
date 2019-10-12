////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	main.cpp
//
// summary:	Implements the main class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include <QtWidgets/QApplication>
#include "GameViewer.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Main entry-point for this application. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="argc">	The number of command-line arguments provided. </param>
/// <param name="argv">	An array of command-line argument strings. </param>
///
/// <returns>	Exit-code for the process - 0 for success, else an error code. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);
	std::string testUrl = "";
	if (argc > 1)
	{
		testUrl = argv[1];
	}
	GameViewer* gameViewer = new GameViewer(testUrl);
	
	int rtv = app.exec();

	return rtv;
}
