////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Game.h
//
// summary:	Declares the game class
////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include <QImage>
#include <json/json.h>

#include <string>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A league record t. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

struct LeagueRecord_t {
	/// <summary>	The wins. </summary>
	int wins;
	/// <summary>	The losses. </summary>
	int losses;
	/// <summary>	The pct. </summary>
	std::string pct;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A team t. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

struct Team_t {
	/// <summary>	The identifier. </summary>
	int id;
	/// <summary>	The name. </summary>
	std::string name;
	/// <summary>	The link. </summary>
	std::string link;
};

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A game event. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

struct GameEvent_t {
	/// <summary>	The score. </summary>
	int score;
	/// <summary>	The series number. </summary>
	int seriesNumber;
	/// <summary>	True if is winner, false if not. </summary>
	bool isWinner;
	/// <summary>	True to split squad. </summary>
	bool splitSquad;
	/// <summary>	The team. </summary>
	Team_t team;
	/// <summary>	The league record. </summary>
	LeagueRecord_t leagueRecord;
};

// Keeping Game Simple for fast access

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	A game. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

class Game 
{
public:

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Gets the winner. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <returns>	The winner. </returns>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	GameEvent_t getWinner();

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Constructor. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	///
	/// <param name="gameValue">	The game value. </param>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Game(Json::Value gameValue);

	////////////////////////////////////////////////////////////////////////////////////////////////////
	/// <summary>	Destructor. </summary>
	///
	/// <remarks>	Chris, 10/6/2019. </remarks>
	////////////////////////////////////////////////////////////////////////////////////////////////////

	~Game();

	std::string getCalendarEventID();

	std::string getDate();

	std::string getHeadline();

	std::string getSubheadline();

	std::string getBlurb();

	std::string getImageUrl();

	QImage* getImage();



private:

	/// <summary>	Identifier for the calendar event. </summary>
	std::string calendarEventID = "";
	/// <summary>	The date. </summary>
	std::string date = "";
	/// <summary>	The headline. </summary>
	std::string headline = "";
	/// <summary>	The subheadline. </summary>
	std::string subheadline = "";
	/// <summary>	URL of the image. </summary>
	std::string imageUrl = "";
	/// <summary>	The blurb. </summary>
	std::string blurb = "";

	/// <summary>	The away. </summary>
	GameEvent_t away;
	/// <summary>	The home. </summary>
	GameEvent_t home;

	/// <summary>	True if is tie, false if not. </summary>
	bool        isTie = false;
	// 14298 Bytes = 320x180
	/// <summary>	The image. </summary>
	QImage* image;
	


};

