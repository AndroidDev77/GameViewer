![Screen Shot](https://raw.githubusercontent.com/AndroidDev77/GameViewer/master/MLBGamesScreenShot.png)

Gameviewer Install - 32-bit

libcurl with Openssl - https://curl.haxx.se/download.html
	1. cd curl-7.66.0/projects
	2. build-openssl.bat
	
jsoncpp - https://github.com/open-source-parsers/jsoncpp
	1. git clone https://github.com/Microsoft/vcpkg.git
	2. cd vcpkg
	3. bootstrap-vcpkg.bat
	4. vcpkg.exe integrate install
	5. vcpkg.exe install jsoncpp

Qt 5.12.2 - MSVC2017

includes:
Qt\5.12.2\msvc2017\include
Qt\5.12.2\msvc2017\include\QtGui
vcpkg\installed\x86-windows\include
curl-7.66.0\curl-7.66.0\include

lib dirs:
vcpkg\installed\x86-windows\lib
curl-7.66.0\curl-7.66.0\build\Win32\VC15\DLL Debug
Qt\5.12.2\msvc2017\lib

linked Libs:
libcurld.lib
jsoncpp.lib
libeay32.lib
Qt5Cored.lib
Qt5Guid.lib
Qt5Widgetsd.lib
Qt5Gamepadd.lib

Justification

Libcurl - Pro: Libcurl has wide support on many platforms Windows/Linux/MSOX. Libraries are compact;
Documentation is abundant;

jsoncpp - Pro: Simple JSON library. Small and easy.

Qt - Pro: Wide Support, detailed documentation. Easy to use but has advanced features.

	 
Use:
1. GameViewer.exe  - To Pull todays games
2. GameViewer.exe "http://statsapi.mlb.com/api/v1/schedule?hydrate=game(content(editorial(recap))),decisions&date=2018-06-10&sportId=1" for custome date







Notes: 
Unit Test code can be found in https://github.com/AndroidDev77/GameViewerTest
