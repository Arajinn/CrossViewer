#include "CrossModel.h"
#include <QFile>
#include <QTextStream>
#include <QSize>

const double selection_color_coefficient = 0.9;
const double highlight_color_coefficient = 0.3;

CrossModel::CrossModel(QObject *parent)
	:QAbstractItemModel(parent)
{
	mPaletteType = PaletteType::Unknown;	
	mPaletteSelectedIndex = -1;
	mPaletteHighLightIndex = -1;
	mIsHighLight = false;
}

void CrossModel::loadPalette()
{
	QFile file(":/CrossView/Resources/palette/colors_dmc_pm.txt");
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream stream(&file);
		while (!stream.atEnd())
		{
			QString line = stream.readLine();
			QStringList list = line.split("\t");

			if (list.size() != 4)
				continue;

			color_info info;
			info.name = list.at(0);
			info.color = QColor(list.at(1).toInt(), list.at(2).toInt(), list.at(3).toInt());
			mPaletteDmc.push_back(info);
		}

		file.close();
	}
}

void CrossModel::onLoadSheme(const QString& fileName)
{
	QFile file(fileName);
	if (!file.open(QIODevice::ReadOnly))
		return;

	QTextStream stream(&file);
	QString line;

	stream >> line;
	if (line == "DMC")
		mPaletteType = PaletteType::DMC;
	else if (line == "Madeira")
		mPaletteType = PaletteType::Madeira;
	else
	{
		mPaletteType = PaletteType::Unknown;
		return;
	}

	stream >> line;
	QStringList list = line.split("::");
	if (list.size() != 2)
		return;

	int dataWight = list.at(0).toInt();
	int dataHeight = list.at(1).toInt();

	mData.clear();
	mData.resize(dataHeight);
	for (int i = 0; i < dataHeight; i++)
		mData[i].resize(dataWight);

	while (!stream.atEnd())
	{
		stream >> line;
		list = line.split("::");
		if (list.size() != 4)
			continue;

		int column = list.at(0).toInt() - 1;
		int row = list.at(1).toInt() - 1;
		QString name = list.at(2);
        //bool used = (list.at(3) == "true") ? true : false;

		auto iter = std::find_if(mPaletteDmc.begin(), mPaletteDmc.end(), [&name](color_info const& elem)
		{
			return elem.name == name;
		});

		if (iter != mPaletteDmc.end())
		{
			int index = std::distance(mPaletteDmc.begin(), iter);
			mData[row][column] = index;
		}
	}

	file.close();

	mUsedColors.clear();
	for (int i = 0; i < dataHeight; i++)
	{
		for (int j = 0; j < dataWight; j++)
		{
			const int colorIndex = mData[i][j];
			const color_info color = mPaletteDmc.at(colorIndex);
			auto iter = std::find_if(mUsedColors.begin(), mUsedColors.end(), [&color](QPair<color_info, int> const& elem)
			{
				return elem.first.name == color.name;
			});

			if (iter != mUsedColors.end())
			{
				iter->second++;
			}
			else
			{
				mUsedColors.push_back({ color,1 });
			}
		}
	}

	std::sort(mUsedColors.begin(), mUsedColors.end(),
		[](const QPair<color_info, int>& left, const QPair<color_info, int>& right)
	{
		return left.second > right.second;
	});

	emit toUpdateMaps();
}

CrossModel::~CrossModel()
{
}

int CrossModel::rowCount(const QModelIndex &/*parent*/) const
{
	return mData.size();
}

int CrossModel::columnCount(const QModelIndex &/*parent*/) const
{
	if (!mData.empty())
		return mData.at(0).size();
	else
		return 0;
}

QVariant CrossModel::data(const QModelIndex &index, int role) const
{
	int row = index.row();
	int column = index.column();

	if ((row >= dataRowStartIndex()) && (row < dataRowEndIndex()) && (column >= dataColumnStartIndex()) && (column < dataColumnEndIndex()))
	{
		if (role == Qt::BackgroundRole)
		{
			const int colorIndex = mData[row][column];
			QColor color = mPaletteDmc.at(colorIndex).color;

			if (mIsHighLight)
			{
				if (colorIndex != mPaletteHighLightIndex)
				{
					double deltaRed = 1.0 - color.redF();
					double deltaGreen = 1.0 - color.greenF();
					double deltaBlue = 1.0 - color.blueF();

					color.setRedF(1.0 - deltaRed * highlight_color_coefficient);
					color.setGreenF(1.0 - deltaGreen * highlight_color_coefficient);
					color.setBlueF(1.0 - deltaBlue * highlight_color_coefficient);
				}
			}

			return color;
		}
		else if (role == Qt::ToolTipRole)
		{
			const int colorIndex = mData[row][column];
			return mPaletteDmc.at(colorIndex).name;
		}
	}
	else if ((row >= paletteRowStartIndex()) && (row < paletteRowEndIndex()) && (column >= paletteColumnStartIndex()) && (column < paletteColumnEndIndex()))
	{
		int paletteIndex = row - paletteRowStartIndex();

		if (role == Qt::BackgroundRole)
		{
			QColor color;
			if (column == 0)
				color = mUsedColors.at(paletteIndex).first.color;
			else if ((column == 1) || (column == 2))
				color = Qt::white;

			if (paletteIndex == mPaletteSelectedIndex)
			{
				color.setRedF(color.redF()*selection_color_coefficient);
				color.setGreenF(color.greenF()*selection_color_coefficient);
				color.setBlueF(color.blueF()*selection_color_coefficient);
			}

			return color;			
		}
		else if (role == Qt::DisplayRole)
		{
			if (column == 1)
				return mUsedColors.at(paletteIndex).first.name;
			else if (column == 2)
				return mUsedColors.at(paletteIndex).second;
		}
	}

	return QVariant();
}

QModelIndex CrossModel::index(int row, int column, const QModelIndex &/*parent*/) const
{
	if ((row >= dataRowStartIndex()) && (row < dataRowEndIndex()) && (column >= dataColumnStartIndex()) && (column < dataColumnEndIndex()))
	{
		return createIndex(row, column, (void*)&mData[row][column]);
	}
	else if ((row >= paletteRowStartIndex()) && (row < paletteRowEndIndex()) && (column >= paletteColumnStartIndex()) && (column < paletteColumnEndIndex()))
	{
		return createIndex(row, column, (void*)&mUsedColors[row - paletteRowStartIndex()]);
	}

	return QModelIndex();
}

QModelIndex CrossModel::parent(const QModelIndex &/*index*/) const
{
	return QModelIndex();
}

int CrossModel::dataRowStartIndex() const
{
	return 0;
}

int CrossModel::dataRowEndIndex() const
{
	return mData.size();
}

int CrossModel::dataColumnStartIndex() const
{
	return 0;
}

int CrossModel::dataColumnEndIndex() const
{
	if (!mData.empty())
		return mData.at(0).size();
	else
		return 0;
}

int CrossModel::paletteRowStartIndex() const
{
	return mData.size();
}

int CrossModel::paletteRowEndIndex() const
{
	return mData.size() + mUsedColors.size();
}

int CrossModel::paletteColumnStartIndex() const
{
	return 0;
}

int CrossModel::paletteColumnEndIndex() const
{
	return 3;
}

void CrossModel::onPaletteColorSelected(const int& row)
{
	mPaletteSelectedIndex = row - paletteRowStartIndex();
}

void CrossModel::changeHighLight()
{
	mIsHighLight = !mIsHighLight;
}

void CrossModel::setHighLight(const bool& value)
{
	mIsHighLight = value;
}

void CrossModel::onHighLightColor(const QModelIndex& index)
{
	const int row = index.row();

	if (row >= paletteRowStartIndex())
	{
		const int colorIndex = row - paletteRowStartIndex();
		const QString colorName = mUsedColors.at(colorIndex).first.name;

		const auto iter = std::find_if(mPaletteDmc.begin(), mPaletteDmc.end(), [&colorName](color_info const& elem)
		{
			return elem.name == colorName;
		});

		if (iter != mPaletteDmc.end())
		{
			mPaletteHighLightIndex = std::distance(mPaletteDmc.begin(), iter);
		}
	}
	else
	{
		const int column = index.column();
		mPaletteHighLightIndex = mData.at(row).at(column);

		const QString colorName = mPaletteDmc.at(mPaletteHighLightIndex).name;
		const auto iter = std::find_if(mUsedColors.begin(), mUsedColors.end(), [&colorName](QPair<color_info, int> const& elem)
		{
			return elem.first.name == colorName;
		});

		if (iter != mUsedColors.end())
		{
			mPaletteSelectedIndex = std::distance(mUsedColors.begin(), iter);
			emit toPaletteLayoutChanged();
		}
	}	

	mIsHighLight = true;
}
