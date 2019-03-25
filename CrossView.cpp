#include "CrossView.h"
#include "ui_CrossView.h"
#include "CrossModel.h"
#include "CrossProxyModel.h"
#include "PaletteProxyModel.h"
#include <QFileDialog>

CrossView::CrossView(QWidget *parent)
	: QMainWindow(parent)
{
	ui = new Ui::CrossViewClass();

	ui->setupUi(this);

	CrossModel* sourceModel = new CrossModel(parent);
	sourceModel->loadPalette();

	CrossProxyModel* crossProxyModel = new CrossProxyModel(parent);
	crossProxyModel->setSourceModel(sourceModel);
	ui->CrossTableView->setModel(crossProxyModel);
	ui->CrossTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	PaletteProxyModel* paletteProxyModel = new PaletteProxyModel(parent);
	paletteProxyModel->setSourceModel(sourceModel);
	ui->PaletteTableView->setModel(paletteProxyModel);
	ui->PaletteTableView->horizontalHeader()->setStretchLastSection(true);	
	ui->PaletteTableView->setContextMenuPolicy(Qt::CustomContextMenu);

	ui->mainToolBar->addAction(ui->actionLoadFile);
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(ui->actionShowPalette);
	ui->mainToolBar->addAction(ui->actionZoomIn);
	ui->mainToolBar->addAction(ui->actionZoomOut);

	QObject::connect(ui->actionLoadFile, &QAction::triggered, this, &CrossView::onLoad);
	QObject::connect(ui->actionShowPalette, &QAction::triggered, this, &CrossView::onShowPalette);
	QObject::connect(ui->actionZoomIn, &QAction::triggered, this, &CrossView::onZoomIn);
	QObject::connect(ui->actionZoomOut, &QAction::triggered, this, &CrossView::onZoomOut);

	QObject::connect(ui->CrossTableView, &QTableView::customContextMenuRequested, this, &CrossView::onCrossContextMenuRequested);
    QObject::connect(ui->CrossTableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &CrossView::onCrossCurrentChanged);

    QObject::connect(ui->PaletteTableView->selectionModel(), &QItemSelectionModel::currentChanged, this, &CrossView::onPaletteSelectionChanged);
    QObject::connect(sourceModel,&CrossModel::toChangePaletteCurrent,ui->PaletteTableView->selectionModel(),
                     static_cast<void (QItemSelectionModel::*)(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)>(&QItemSelectionModel::select));
	QObject::connect(ui->PaletteTableView, &QTableView::customContextMenuRequested, this, &CrossView::onPaletteContextMenuRequested);	

	QObject::connect(this, &CrossView::toPaletteLayoutChanged, paletteProxyModel, &PaletteProxyModel::onLayoutChanged);
    QObject::connect(this, &CrossView::toCrossLayoutChanged, crossProxyModel, &CrossProxyModel::onLayoutChanged);

	QObject::connect(sourceModel, &CrossModel::toPaletteLayoutChanged, paletteProxyModel, &PaletteProxyModel::onLayoutChanged);
    QObject::connect(sourceModel, &CrossModel::toCrossLayoutChanged, crossProxyModel, &CrossProxyModel::onLayoutChanged);

	QObject::connect(sourceModel, &CrossModel::toUpdateMaps, crossProxyModel, &CrossProxyModel::onFormMaps);
	QObject::connect(sourceModel, &CrossModel::toUpdateMaps, paletteProxyModel, &PaletteProxyModel::onFormMaps);

	QObject::connect(this, &CrossView::toLoadSheme, sourceModel, &CrossModel::onLoadSheme);	
	QObject::connect(this, &CrossView::toPaletteColorSelected, sourceModel, &CrossModel::onPaletteColorSelected);
	QObject::connect(this, &CrossView::toHighLightColor, sourceModel, &CrossModel::onHighLightColor);
    QObject::connect(this, &CrossView::toSelectColor, sourceModel, &CrossModel::onSelectColor);

	crossSizes.resize(8);
	crossSizes[0] = 4;
	crossSizes[1] = 8;
	crossSizes[2] = 12;
	crossSizes[3] = 16;
	crossSizes[4] = 20;
	crossSizes[5] = 24;
	crossSizes[6] = 28;
	crossSizes[7] = 32;
	currentSizeIndex = 3;
	updateCurrentCrossSize();
}

void CrossView::onLoad()
{
	const QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), QApplication::applicationDirPath(), tr("Text file (*.txt)"));

	if (fileName.isEmpty())
		return;

	emit toLoadSheme(fileName);
}

void CrossView::onPaletteSelectionChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
	emit toPaletteColorSelected(current.row());
	emit toPaletteLayoutChanged();
}

void CrossView::onPaletteContextMenuRequested(const QPoint &pos)
{
	const QModelIndex index = ui->PaletteTableView->indexAt(pos);
	if (index.isValid())
	{		
		QMenu* contextMenu = new QMenu(this);
		QAction* highlightAction = new QAction("Highlight color", this);		
        highlightAction->setCheckable(true);

        PaletteProxyModel* model = dynamic_cast<PaletteProxyModel*>(ui->PaletteTableView->model());
        if (model)
            highlightAction->setChecked(model->isHighLight());

        contextMenu->addAction(highlightAction);
		QObject::connect(highlightAction, &QAction::triggered, this, &CrossView::onHighlightColorFromPaletteView);

		const QPoint point = ui->PaletteTableView->viewport()->mapToGlobal(pos);
		contextMenu->popup(point);
	}
}

void CrossView::onHighlightColorFromCrossView()
{	
	onHighlightColor(ui->CrossTableView->selectionModel()->currentIndex());
}

void CrossView::onHighlightColorFromPaletteView()
{	
	onHighlightColor(ui->PaletteTableView->selectionModel()->currentIndex());
}

void CrossView::onHighlightColor(const QModelIndex& highLightIndex)
{
	emit toHighLightColor(highLightIndex);
	emit toCrossLayoutChanged();
}

void CrossView::onShowPalette()
{
	bool isPaletteVisible = ui->PaletteTableView->isVisible();
	ui->PaletteTableView->setVisible(!isPaletteVisible);
}

void CrossView::onCrossContextMenuRequested(const QPoint &pos)
{
	const QModelIndex index = ui->CrossTableView->indexAt(pos);
	if (index.isValid())
	{
		QMenu* contextMenu = new QMenu(this);
		QAction* highlightAction = new QAction("Highlight color", this);

        highlightAction->setCheckable(true);

        CrossProxyModel* model = dynamic_cast<CrossProxyModel*>(ui->CrossTableView->model());
        if (model)
            highlightAction->setChecked(model->isHighLight());

		contextMenu->addAction(highlightAction);
		QObject::connect(highlightAction, &QAction::triggered, this, &CrossView::onHighlightColorFromCrossView);

		const QPoint point = ui->CrossTableView->viewport()->mapToGlobal(pos);
		contextMenu->popup(point);
	}
}

void CrossView::onZoomIn()
{
	currentSizeIndex++;

	if (currentSizeIndex >= crossSizes.size())
		currentSizeIndex = crossSizes.size() - 1;

	updateCurrentCrossSize();
}

void CrossView::onZoomOut()
{
	currentSizeIndex--;

	if (currentSizeIndex < 0)
		currentSizeIndex = 0;

	updateCurrentCrossSize();
}

void CrossView::updateCurrentCrossSize()
{
	const int size = crossSizes.at(currentSizeIndex);
	ui->CrossTableView->horizontalHeader()->setDefaultSectionSize(size);
	ui->CrossTableView->verticalHeader()->setDefaultSectionSize(size);
}

void CrossView::onCrossCurrentChanged(const QModelIndex &current, const QModelIndex &/*previous*/)
{
    emit toSelectColor(current);
}
