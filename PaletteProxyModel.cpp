#include "PaletteProxyModel.h"
#include "CrossModel.h"

PaletteProxyModel::PaletteProxyModel(QObject *parent)
	:QAbstractProxyModel(parent)
	, mRowCount(0)
	, mColumnCount(0)
{
}

PaletteProxyModel::~PaletteProxyModel()
{
}

int PaletteProxyModel::rowCount(const QModelIndex &/*parent*/) const
{
	return mRowCount;
}

int PaletteProxyModel::columnCount(const QModelIndex &/*parent*/) const
{
	return mColumnCount;
}

QVariant PaletteProxyModel::data(const QModelIndex &/*index*/, int /*role*/) const
{
	return QVariant();
}

QVariant PaletteProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Vertical)
	{
		if (role == Qt::DisplayRole)
		{
			return section + 1;
		}
	}

	return QAbstractProxyModel::headerData(section, orientation, role);
}

Qt::ItemFlags PaletteProxyModel::flags(const QModelIndex &index) const
{
	auto flags = QAbstractProxyModel::flags(index);
	flags |= Qt::ItemIsEnabled;
	flags |= Qt::ItemIsSelectable;
	flags |= Qt::ItemIsUserCheckable;
	return flags;
}

QModelIndex PaletteProxyModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
	if (mappingToSourceSortByIndex.empty())
		return QModelIndex();

	auto iter = std::find_if(mappingToSourceSortByIndex.begin(), mappingToSourceSortByIndex.end(), [&row, &column](QPair<QPersistentModelIndex, QPersistentModelIndex> const& elem)
	{
		return ((elem.first.row() == row) && (elem.first.column() == column));
	});

	if (iter != mappingToSourceSortByIndex.end())
		return iter->second;
	else
		return QModelIndex();
}

QModelIndex PaletteProxyModel::parent(const QModelIndex &/*index*/) const
{
	return QModelIndex();
}

QModelIndex PaletteProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
	if (!sourceIndex.isValid())
		return QModelIndex();

	auto iter = std::lower_bound(mappingFromSourceSortByIndex.begin(), mappingFromSourceSortByIndex.end(), sourceIndex,
		[](const QPair<QPersistentModelIndex, QPersistentModelIndex>& left, const QModelIndex& right)->bool
	{
		return left.first < right;
	});

	if (iter != mappingFromSourceSortByIndex.end())
		return iter->second;
	else
		return QModelIndex();
}

QModelIndex PaletteProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
	if (!proxyIndex.isValid())
		return QModelIndex();

	auto iter = std::lower_bound(mappingToSourceSortByIndex.begin(), mappingToSourceSortByIndex.end(), proxyIndex,
		[](const QPair<QPersistentModelIndex, QPersistentModelIndex>& left, const QModelIndex& right)->bool
	{
		return left.first < right;
	});

	if (iter != mappingToSourceSortByIndex.end())
		return iter->second;
	else
		return QModelIndex();
}

void PaletteProxyModel::onFormMaps()
{
	CrossModel* crossModel = dynamic_cast<CrossModel*>(sourceModel());
	if (crossModel == nullptr)
		return;

	emit layoutAboutToBeChanged();

	int rowStartIndex = crossModel->paletteRowStartIndex();
	int rowEndIndex = crossModel->paletteRowEndIndex();
	mRowCount = rowEndIndex - rowStartIndex;
	int columnStartIndex = crossModel->paletteColumnStartIndex();
	int columnEndIndex = crossModel->paletteColumnEndIndex();
	mColumnCount = columnEndIndex - columnStartIndex;

	mappingFromSourceSortByIndex.resize(mRowCount*mColumnCount);
	mappingToSourceSortByIndex.resize(mRowCount*mColumnCount);
	int k = 0;
	for (int row = rowStartIndex; row < rowEndIndex; row++)
	{
		for (int column = columnStartIndex; column < columnEndIndex; column++)
		{
			int currentRow = row - rowStartIndex;
			int currentColumn = column - columnStartIndex;
			QModelIndex sourceIndex = crossModel->index(row, column);
			QModelIndex proxyIndex = createIndex(currentRow, currentColumn, sourceIndex.internalPointer());

			mappingFromSourceSortByIndex[k] = { sourceIndex,proxyIndex };
			mappingToSourceSortByIndex[k] = { proxyIndex,sourceIndex };
			k++;
		}
	}

	std::sort(mappingToSourceSortByIndex.begin(), mappingToSourceSortByIndex.end(),
		[](const QPair<QPersistentModelIndex, QPersistentModelIndex>& left, const QPair<QPersistentModelIndex, QPersistentModelIndex>& right)
	{
		return !(left.first < right.first);
	});

	std::sort(mappingFromSourceSortByIndex.begin(), mappingFromSourceSortByIndex.end(),
		[](const QPair<QPersistentModelIndex, QPersistentModelIndex>& left, const QPair<QPersistentModelIndex, QPersistentModelIndex>& right)
	{
		return !(left.first < right.first);
	});

	emit layoutChanged();
}

void PaletteProxyModel::onLayoutChanged()
{
	emit layoutChanged();
}

bool PaletteProxyModel::isHighLight() const
{
    CrossModel* crossModel = dynamic_cast<CrossModel*>(sourceModel());
    if (crossModel == nullptr)
        return false;

    return crossModel->isHighLight();
}
