#pragma once

#include <QAbstractItemModel>
#include <QColor>

class CrossModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	struct color_info
	{
		QString name;
		QColor color;
		bool used;
	};

	enum class PaletteType
	{
		DMC,
		Madeira,
		Unknown
	};

	CrossModel(QObject *parent = nullptr);
	~CrossModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &index) const;

	int dataRowStartIndex() const;
	int dataRowEndIndex() const;
	int dataColumnStartIndex() const;
	int dataColumnEndIndex() const;

	int paletteRowStartIndex() const;
	int paletteRowEndIndex() const;
	int paletteColumnStartIndex() const;
	int paletteColumnEndIndex() const;

	void loadPalette();	
		
	void changeHighLight();
	void setHighLight(const bool& value);
private:
	QVector<QVector<int>> mData;
	QVector<QPair<color_info,int>> mUsedColors;

	PaletteType mPaletteType;
	QVector<color_info> mPaletteDmc;

	int mPaletteSelectedIndex, mPaletteHighLightIndex;
	bool mIsHighLight;

public slots:
	void onLoadSheme(const QString& fileName);
	void onPaletteColorSelected(const int& row);
	void onHighLightColor(const QModelIndex& index);
signals:
	void toUpdateMaps();
	void toPaletteLayoutChanged();
};

