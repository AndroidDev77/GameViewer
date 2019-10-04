#pragma once
#include <QImage>
#include <json/json.h>

#include <string>

struct LeagueRecord_t {
	int wins;
	int losses;
	std::string pct;
};

struct Team_t {
	int id;
	std::string name;
	std::string link;
};

struct GameEvent_t {
	int score;
	int seriesNumber;
	bool isWinner;
	bool splitSquad;
	Team_t team;
	LeagueRecord_t leagueRecord;
};

// Keeping Game Simple for fast access
class Game
{
public:
	std::string calendarEventID = "";
	std::string date            = "";
	std::string headline        = "";
	std::string subheadline     = "";
	std::string imageUrl        = "";
	std::string blurb           = "";

	bool        isTie           = false;
	// 14298 Bytes = 320x180
	QImage*      image;

	GameEvent_t away;
	GameEvent_t home;

	Game(Json::Value gameValue);
	~Game();

private:
	GameEvent_t getWinner();


};

