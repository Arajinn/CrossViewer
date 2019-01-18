#pragma once

#include <QAbstractProxyModel>

class PaletteProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	PaletteProxyModel(QObject *parent = nullptr);
	~PaletteProxyModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;	
private:
	QVector<QPair<QPersistentModelIndex, QPersistentModelIndex>> mappingFromSourceSortByIndex;
	QVector<QPair<QPersistentModelIndex, QPersistentModelIndex>> mappingToSourceSortByIndex;

	int mRowCount, mColumnCount;

public slots:
	void onFormMaps();
	void onLayoutChanged();
};

