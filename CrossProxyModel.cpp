#include "CrossProxyModel.h"
#include "CrossModel.h"
#include <QDebug>

CrossProxyModel::CrossProxyModel(QObject *parent)
	:QAbstractProxyModel(parent)
	,mRowCount(0)
	,mColumnCount(0)
{
	
}

CrossProxyModel::~CrossProxyModel()
{
}

int CrossProxyModel::rowCount(const QModelIndex &/*parent*/) const
{
	return mRowCount;
}

int CrossProxyModel::columnCount(const QModelIndex &/*parent*/) const
{
	return mColumnCount;
}

QVariant CrossProxyModel::data(const QModelIndex &/*index*/, int /*role*/) const
{
	return QVariant();
}

Qt::ItemFlags CrossProxyModel::flags(const QModelIndex &/*index*/) const
{
    Qt::ItemFlags flags = 0;
	flags |= Qt::ItemIsEnabled;
	return flags;
}

QModelIndex CrossProxyModel::index(int row, int column, const QModelIndex &parent) const
{
	return sourceModel()->index(row, column, parent);
}

QModelIndex CrossProxyModel::parent(const QModelIndex &/*index*/) const
{
	return QModelIndex();
}

QModelIndex CrossProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	return sourceIndex;
}

QModelIndex CrossProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	return proxyIndex;
}

void CrossProxyModel::onFormMaps()
{
	CrossModel* crossModel = dynamic_cast<CrossModel*>(sourceModel());
	if (crossModel == nullptr)
		return;

	emit layoutAboutToBeChanged();

	int rowStartIndex = crossModel->dataRowStartIndex();
	int rowEndIndex = crossModel->dataRowEndIndex();
	mRowCount = rowEndIndex - rowStartIndex;
	int columnStartIndex = crossModel->dataColumnStartIndex();
	int columnEndIndex = crossModel->dataColumnEndIndex();
	mColumnCount = columnEndIndex - columnStartIndex;

	emit layoutChanged();
}

void CrossProxyModel::onLayoutChanged()
{
    emit layoutChanged();
}

bool CrossProxyModel::isHighLight() const
{
    CrossModel* crossModel = dynamic_cast<CrossModel*>(sourceModel());
    if (crossModel == nullptr)
        return false;

    return crossModel->isHighLight();
}
