#pragma once

#include <QTableView>

class PaletteTableView : public QTableView
{
public:
	PaletteTableView(QWidget *parent = nullptr);
	~PaletteTableView();

	Qt::ItemFlags flags(const QModelIndex & index) const;
};

