Gameviewer Install - 32-bit

libcurl - https://curl.haxx.se/download.html
	1. cd curl-7.66.0/projects
	2. build-openssl.bat
	
jsoncpp - https://github.com/open-source-parsers/jsoncpp
	1. git clone https://github.com/Microsoft/vcpkg.git
	2. cd vcpkg
	3. bootstrap-vcpkg.bat
	4. vcpkg.exe integrate install
	5. vcpkg.exe install jsoncpp

Qt 5.12.2 - MSVC2017



Justification

Libcurl - Pro: Libcurl has wide support on many platforms Windows/Linux/MSOX. Libraries are compact;
Documentation is abundant;

jsoncpp - Pro: Simple JSON library. Small and easy.

Qt - Pro: Wide Support, detailed documentation. Easy to use but has advanced features.
	 Con: It is a little heavy. Commercial License.
