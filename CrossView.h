#pragma once

#include <QtWidgets/QMainWindow>

class CrossModel;
class CrossProxyModel;
class PaletteProxyModel;

namespace Ui
{
	class CrossViewClass;
}

class CrossView : public QMainWindow
{
	Q_OBJECT

public:
	CrossView(QWidget *parent = Q_NULLPTR);

private:
	Ui::CrossViewClass* ui;

	void onHighlightColor(const QModelIndex& highLightIndex);

	QVector<int> crossSizes;
	int currentSizeIndex;
	void updateCurrentCrossSize();
private slots:
	void onLoad();
	void onPaletteSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
	void onPaletteContextMenuRequested(const QPoint &pos);
	void onCrossContextMenuRequested(const QPoint &pos);	
	void onHighlightColorFromCrossView();
	void onHighlightColorFromPaletteView();
	void onShowPalette();
	void onZoomIn();
	void onZoomOut();
signals:
	void toLoadSheme(const QString& filename);
	void toPaletteColorSelected(const int& row);
	void toPaletteLayoutChanged();
	void toCrossLayoutChanged();
	void toHighLightColor(const QModelIndex& index);
};
