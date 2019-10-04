#pragma once

#include <list>
#include "Game.h"
#include <QtCore\QAbstractItemModel>
#include <QOpenGLTexture>
#include <QPixmap>

class GameModel : public QAbstractListModel
{

public:
	GameModel(QObject* parent = nullptr);
	~GameModel();

	int rowCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

	Qt::ItemFlags flags(const QModelIndex& index) const;
	void setList(std::vector<Game>* list);

protected:
	bool canFetchMore(const QModelIndex& parent) const override;
	void fetchMore(const QModelIndex& parent) override;

private:

	std::vector<Game>* gameList;
	int gameCount;
};

