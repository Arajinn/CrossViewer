#pragma once

#include <QAbstractItemModel>
#include <QItemSelectionModel>
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
    QModelIndex index(int row, int column=0, const QModelIndex &parent = QModelIndex()) const;
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

    bool isHighLight() const;
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
    void onHighLightColor(const QModelIndex& select_index);
    void onSelectColor(const QModelIndex& select_index);
signals:
	void toUpdateMaps();
	void toPaletteLayoutChanged();
    void toChangePaletteCurrent(const QModelIndex& index, QItemSelectionModel::SelectionFlags command);
    void toCrossLayoutChanged();
};

