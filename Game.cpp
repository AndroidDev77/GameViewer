////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	Game.cpp
//
// summary:	Implements the Game class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Game.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Constructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="gameValue">	The Json node game value. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

Game::Game(Json::Value gameValue)
{
	calendarEventID = gameValue["calendarEventID"].asString();
	date            = gameValue["gameDate"].asString();
	headline        = gameValue["content"]["editorial"]["recap"]["home"]["headline"].asString();
	subheadline     = gameValue["content"]["editorial"]["recap"]["home"]["subhead"].asString();

	blurb            = gameValue["content"]["editorial"]["recap"]["home"]["blurb"].asString();
	isTie            = gameValue["isTie"].asBool();

	// Image 0  is 1920x1080
	// Image 3  is 960x640
	// Image 9  is 480x270
	// Image 12 is 320x180
	// Could Costimize this for different platforms
	imageUrl         = gameValue["content"]["editorial"]["recap"]["mlb"]["image"]["cuts"][12]["src"].asString();

	//Set Away Team
	away.score        = gameValue["teams"]["away"]["score"].asInt();
	away.seriesNumber = gameValue["teams"]["away"]["seriesNumber"].asInt();
	away.isWinner     = gameValue["teams"]["away"]["isWinner"].asBool();
	//away.splitSquad   = gameValue["teams"]["away"]["splitSquad"].asBool();

	away.team.id      = gameValue["teams"]["away"]["team"]["id"].asInt();
	away.team.name    = gameValue["teams"]["away"]["team"]["name"].asString();
	away.team.link    = gameValue["teams"]["away"]["team"]["link"].asString();

	away.leagueRecord.wins   = gameValue["teams"]["away"]["leagueRecord"]["wins"].asInt();
	away.leagueRecord.losses = gameValue["teams"]["away"]["leagueRecord"]["losses"].asInt();
	away.leagueRecord.pct    = gameValue["teams"]["away"]["leagueRecord"]["pct"].asString();

	//Set Home Team
	home.score        = gameValue["teams"]["home"]["score"].asInt();
	home.seriesNumber = gameValue["teams"]["home"]["seriesNumber"].asInt();
	home.isWinner     = gameValue["teams"]["home"]["isWinner"].asBool();
	home.splitSquad   = gameValue["teams"]["home"]["splitSquad"].asBool();

	home.team.id      = gameValue["teams"]["home"]["team"]["id"].asInt();
	home.team.name    = gameValue["teams"]["home"]["team"]["name"].asString();
	home.team.link    = gameValue["teams"]["home"]["team"]["link"].asString();

	home.leagueRecord.wins   = gameValue["teams"]["home"]["leagueRecord"]["wins"].asInt();
	home.leagueRecord.losses = gameValue["teams"]["home"]["leagueRecord"]["losses"].asInt();
	home.leagueRecord.pct    = gameValue["teams"]["home"]["leagueRecord"]["pct"].asString();

	// Validate Data

	// subheadline may sometimes be empty
	if (subheadline.empty())
	{
		subheadline = gameValue["description"].asString();
	}

	// If headline is empty make it generic
	if (headline.empty())
	{
		headline = away.team.name + " Vs " + home.team.name;
	}

	image = new QImage(320,180,QImage::Format_RGB32);


}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////


Game::~Game()
{
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets the winner. </summary>
///
/// <remarks>	Not used right now </remarks>
///
/// <returns>	The winner. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

GameEvent_t Game::getWinner()
{
	if (!isTie)
	{
		if (home.isWinner)
		{
			return home;
		}
		else
		{
			return away;
		}
	}
	else
	{
		// If tie return empty GameEvent_t
		GameEvent_t tie;
		tie.team.id = 0;
		return tie;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets CalendarEventID. </summary>
///
/// <returns>	The a Unique ID for Game based on date. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getCalendarEventID()
{
	return this->calendarEventID;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets date. </summary>
///
///
/// <returns>	The Date the game took place. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getDate()
{
	return this->date;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets headline. </summary>
///
///
/// <returns>	The headline set in Game Context JSON text. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getHeadline()
{
	return this->headline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets subheadline. </summary>
///
///
/// <returns>	The bottom headline. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getSubheadline()
{
	return this->subheadline;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets Blurb. </summary>
///
///
/// <returns>	The detailed summary of the game. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getBlurb()
{
	return this->blurb;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets Image URL. </summary>
///
///
/// <returns>	A string for Image URL. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::string Game::getImageUrl()
{
	return this->imageUrl;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Gets Image. </summary>
///
///
/// <returns>	The QImage pointer. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

QImage* Game::getImage()
{
	return image;
}


