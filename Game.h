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

class Game
{
public:
	std::string calendarEventID = "";
	std::string date = "";
	std::string headline = "";
	std::string subheadline = "";
	std::string imageUrl = "";
	bool        isTie = false;

	GameEvent_t away;
	GameEvent_t home;

	Game(Json::Value gameRoot);

	GameEvent_t getWinner();
};

