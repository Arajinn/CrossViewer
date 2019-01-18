#pragma once

#include <QAbstractProxyModel>

class CrossProxyModel : public QAbstractProxyModel
{
	Q_OBJECT

public:
	CrossProxyModel(QObject *parent = nullptr);
	~CrossProxyModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;
	QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
	QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

	
private:
	int mRowCount, mColumnCount;
	
public slots:
	void onFormMaps();
	void onLayoutChanged();
};

