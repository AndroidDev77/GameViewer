#include "GameModel.h"

GameModel::GameModel(QObject* parent)
	: QAbstractListModel(parent), gameCount(0)
{
	gameList = nullptr;
}

GameModel::~GameModel()
{
	delete gameList;
}

int GameModel::rowCount(const QModelIndex& parent) const
{
	return parent.isValid() ? 0 : gameCount;
}

bool GameModel::canFetchMore(const QModelIndex& parent) const
{
	if (parent.isValid())
		return false;
	return (gameCount < gameList->size());
}

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

QVariant GameModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (index.row() >= gameList->size() || index.row() < 0)
		return QVariant();

	if (role == Qt::ForegroundRole)
	{
		return gameList->at(index.row()).image;;
	}
	if (role == Qt::DisplayRole)
	{
		return QVariant(gameList->at(index.row()).headline.c_str());
	}
	if (role == Qt::ToolTipRole)
	{
		return QVariant(gameList->at(index.row()).subheadline.c_str());
	}

	return QVariant();
}

Qt::ItemFlags GameModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return 0;
	return QAbstractItemModel::flags(index);
}

void GameModel::setList(std::vector<Game>* list) 
{
	gameList = list;
}
