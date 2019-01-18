#include "PaletteTableView.h"

PaletteTableView::PaletteTableView(QWidget *parent)
	:QTableView(parent)
{
}

PaletteTableView::~PaletteTableView()
{
}

Qt::ItemFlags PaletteTableView::flags(const QModelIndex & index) const
{
	return QTableView::fla
}