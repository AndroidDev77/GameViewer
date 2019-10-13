////////////////////////////////////////////////////////////////////////////////////////////////////
// file:	GameModel.cpp
//
// summary:	Implements the game model class
////////////////////////////////////////////////////////////////////////////////////////////////////

#include "GameModel.h"

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Constructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="parent">	[in,out] If non-null, the parent. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

GameModel::GameModel(QObject* parent)
	: QAbstractListModel(parent), gameCount(0)
{
	gameList = new std::vector<Game*>();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Destructor. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
////////////////////////////////////////////////////////////////////////////////////////////////////

GameModel::~GameModel()
{
	for (auto it = gameList->begin(); it != gameList->end(); )
	{
		delete *it;
		it = gameList->erase(it);
	}
	delete gameList;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Row count. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="parent">	The parent. </param>
///
/// <returns>	An int. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

int GameModel::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : gameCount;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Determine if we can fetch more. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="parent">	The parent. </param>
///
/// <returns>	True if we can fetch more, false if not. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

bool GameModel::canFetchMore(const QModelIndex& parent) const
{
	if (parent.isValid())
		return false;
	return (gameCount < gameList->size());
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Fetches a more. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="parent">	The parent. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void GameModel::fetchMore(const QModelIndex& parent)
{
	if (parent.isValid())
		return;
	int remainder = gameList->size() - gameCount;
	int itemsToFetch = qMin(10, remainder);

	if (itemsToFetch <= 0)
		return;

	beginInsertRows(QModelIndex(), gameCount, gameCount + itemsToFetch - 1);

	gameCount += itemsToFetch;

	endInsertRows();

	//emit numberPopulated(itemsToFetch);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Data. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="index">	Zero-based index of the. </param>
/// <param name="role"> 	The role. </param>
///
/// <returns>	A QVariant. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

QVariant GameModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= gameList->size() || index.row() < 0)
	{
		return QVariant(); 
	}
	// Using Enum to fetch image
	if (role == Qt::BackgroundRole)
	{	
		return *(gameList->at(index.row())->getImage());
	}
	if (role == Qt::DisplayRole)
	{
		return QVariant(gameList->at(index.row())->getHeadline().c_str());
	}
	// Using Enum to fetch subheadline
	if (role == Qt::ToolTipRole)
	{
		return QVariant(gameList->at(index.row())->getSubheadline().c_str());
	}

	if (role == Qt::UserRole)
	{
		return QVariant(gameList->at(index.row())->getBlurb().c_str());
	}

	return QVariant();
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Flags the given index. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="index">	Zero-based index of the. </param>
///
/// <returns>	The Qt::ItemFlags. </returns>
////////////////////////////////////////////////////////////////////////////////////////////////////

Qt::ItemFlags GameModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;
	return QAbstractItemModel::flags(index);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Sets a list. </summary>
///
/// <remarks>	Chris, 10/6/2019. </remarks>
///
/// <param name="list">	[in,out] If non-null, the list. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

std::vector<Game*>* GameModel::getList()
{
	return gameList;
}
