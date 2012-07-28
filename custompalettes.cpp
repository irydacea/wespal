#include "custompalettes.hpp"
#include "ui_custompalettes.h"
#include "util.hpp"

#include "paletteitem.hpp"
#include <QColorDialog>
#include <QMessageBox>

CustomPalettes::CustomPalettes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomPalettes)
{
    ui->setupUi(this);

	/*palw = new PaletteWidget(this);
	ui->paletteViewContainer->addWidget(palw);*/

	ui->listColors->setItemDelegate(new PaletteItemDelegate(ui->listPals));
}

CustomPalettes::~CustomPalettes()
{
    delete ui;
}

void CustomPalettes::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void CustomPalettes::updatePaletteUI()
{
	{
		// Make sure to not emit signals while setting up rows.
		const ObjectLock lock(*ui->listPals);

		ui->listPals->clear();

		for(QMap< QString, QList<QRgb> >::const_iterator i = palettes_.constBegin();
			i != palettes_.constEnd(); ++i) {
			addPaletteListEntry(i.key());
		}
	}

	// Notify the palette view widget.
	ui->listPals->setCurrentRow(0);
}

void CustomPalettes::addPaletteListEntry(const QString& name)
{
	QListWidgetItem* lwi = new QListWidgetItem(name, ui->listPals);
	lwi->setData(Qt::UserRole, name);
	lwi->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	const QList<QRgb> palette = palettes_.value(name);

	if(!palette.empty()) {
		lwi->setIcon(createColorIcon(palette.front()));
	}
}

void CustomPalettes::removePaletteListEntry(const QString &name)
{
	QList<QListWidgetItem*> const items = ui->listPals->findItems(name, Qt::MatchFixedString);
	Q_ASSERT(items.size() <= 1);

	foreach(QListWidgetItem* w, items) {
		delete ui->listPals->takeItem(ui->listPals->row(w));
	}
}

void CustomPalettes::on_listPals_currentRowChanged(int currentRow)
{
	QListWidgetItem* const itemw = ui->listPals->item(currentRow);

	Q_ASSERT(itemw);
	if(!itemw)
		return;

	const QString& name = itemw->text();
	QMap< QString, QList<QRgb> >::iterator pal_it = palettes_.find(name);

	if(pal_it == palettes_.end()) {
		QMessageBox::critical(this, tr("Wesnoth RCX"),
			tr("The palette \"%1\" does not exist.").arg(name));
		removePaletteListEntry(name);
		return;
	}

	populatePaletteView(pal_it.value());
}

void CustomPalettes::populatePaletteView(const QList<QRgb> &pal)
{
	QListWidget* const listw = ui->listColors;

	Q_ASSERT(listw);
	if(!listw)
		return;

	{
		// Make sure to not emit signals while setting up rows.
		const ObjectLock lock(*listw);

		listw->clear();

		foreach(QRgb rgb, pal) {
			QListWidgetItem* itemw = new QListWidgetItem("", listw);
			itemw->setData(Qt::UserRole, rgb);
			itemw->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
		}
	}

	// The hex editor box needs to be notified now.
	listw->setCurrentRow(0);
}

void CustomPalettes::setColorEditControlsEnabled(bool enabled)
{
	const bool haveColors = ui->listColors->count() != 0;

	ui->cmdDelCol->setEnabled(enabled && haveColors);
	ui->tbEditColor->setEnabled(enabled && haveColors);
	ui->leColor->setEnabled(enabled && haveColors);
}

void CustomPalettes::setPaletteEditControlsEnabled(bool enabled)
{
	ui->listColors->setEnabled(enabled);
	ui->cmdAddCol->setEnabled(enabled);

	setColorEditControlsEnabled(enabled);
}

void CustomPalettes::setPaletteViewEnabled(bool enabled)
{
	ui->cmdDelPal->setEnabled(enabled);
	ui->cmdRenPal->setEnabled(enabled);

	setPaletteEditControlsEnabled(enabled);
}

void CustomPalettes::clearPaletteView()
{
	ui->listColors->clear();
	ui->leColor->clear();
}

QList<QRgb>& CustomPalettes::getCurrentPalette()
{
	QListWidgetItem const* palItem = ui->listPals->currentItem();
	Q_ASSERT(palItem);

	if(!palItem)
		return palettes_[""];

	const QString& palId = palItem->data(Qt::UserRole).toString();
	// Create a palette if necessary.
	return palettes_[palId];
}

void CustomPalettes::updatePaletteIcon()
{
	QListWidgetItem* const palw = ui->listPals->currentItem();
	Q_ASSERT(palw);

	const QList<QRgb>& palette = palettes_.value(palw->data(Qt::UserRole).toString());

	if(palette.empty())
		return;

	palw->setIcon(createColorIcon(palette.front()));
}

QString CustomPalettes::generateNewPaletteName() const
{
	QString name;
	int i = 0;

	do {
		name = tr("New Palette #%1").arg(++i);
	} while(palettes_.find(name) != palettes_.end());

	return name;
}

void CustomPalettes::on_listColors_currentRowChanged(int currentRow)
{
	QLineEdit* const textw = ui->leColor;
	const QColor currentColor =
		ui->listColors->item(currentRow)->data(Qt::UserRole).toInt();

	textw->setText(currentColor.name());
}

void CustomPalettes::on_listColors_itemChanged(QListWidgetItem *item)
{
	QLineEdit* const textw = ui->leColor;
	const QColor currentColor =
		item->data(Qt::UserRole).toInt();

	textw->setText(currentColor.name());

	// Update palette definition.

	QListWidget* const listw = ui->listColors;

	QList<QRgb>& pal = getCurrentPalette();
	const int index = listw->currentRow();
	Q_ASSERT(index < pal.size());
	pal[index] = currentColor.rgb();

	// If this is the first row we might as well update
	// the palette icon.

	if(listw->currentRow() == 0) {
		updatePaletteIcon();
	}
}

void CustomPalettes::on_cmdRenPal_clicked()
{
	QListWidget* const listw = ui->listPals;
	// An edit slot should take care of updating the
	// palette definition afterwards.
	listw->editItem(listw->currentItem());
}

void CustomPalettes::on_tbEditColor_clicked()
{
	QListWidget* const listw = ui->listColors;
	QListWidgetItem* const lwi = listw->currentItem();

	if(!lwi)
		return;

	QColor color = lwi->data(Qt::UserRole).toInt();
	color = QColorDialog::getColor(color);

	if(!color.isValid())
		return;

	const QRgb rgb = color.rgb();
	lwi->setData(Qt::UserRole, rgb);
}

void CustomPalettes::on_cmdAddCol_clicked()
{
	QListWidget* const listw = ui->listColors;

	// Make sure to actually add the item only later, once
	// it is safe to notify other widgets!

	QListWidgetItem* const itemw = new QListWidgetItem("");

	// We might already have a first row with the first
	// color of the palette. If we don't we use pure black.

	QListWidgetItem* const first = listw->item(0);

	const QRgb rgb = first ? first->data(Qt::UserRole).toInt() : qRgb(0,0,0);

	itemw->setData(Qt::UserRole, rgb);
	itemw->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	// Update palette definition.

	QList<QRgb>& pal = getCurrentPalette();
	pal.push_back(rgb);

	// Notify widgets.

	listw->addItem(itemw);
	listw->setCurrentItem(itemw);

	setColorEditControlsEnabled(true);

	// If this is the first row we might as well update
	// the palette icon.

	if(first == NULL) {
		updatePaletteIcon();
	}
}

void CustomPalettes::on_cmdDelCol_clicked()
{
	QListWidget* const listw = ui->listColors;

	ObjectLock lock(*listw);

	const int remaining = listw->count();

	if(remaining == 0)
		return;

	const int index = listw->currentRow();

	delete listw->takeItem(listw->currentRow());

	if(remaining == 1) {
		// No more colors!
		setColorEditControlsEnabled(false);
	}

	// Update palette definition.

	QList<QRgb>& pal = getCurrentPalette();

	Q_ASSERT(index < pal.count());
	pal.removeAt(index);

	// If this was the first row and we still have
	// more colors, update the palette's color icon.
	if(index == 0 && remaining > 1) {
		updatePaletteIcon();
	}
}

void CustomPalettes::on_cmdAddPal_clicked()
{
	QListWidget* const listw = ui->listPals;

	{
		ObjectLock lockPals(*listw);

		const QString& palName = generateNewPaletteName();
		palettes_[palName].push_back(qRgb(0,0,0));
		addPaletteListEntry(palName);

		setPaletteViewEnabled(true);
	}

	listw->setCurrentRow(listw->count() - 1);
}

void CustomPalettes::on_cmdDelPal_clicked()
{
	QListWidget* const listw = ui->listPals;

	ObjectLock lockPals(*listw);
	ObjectLock lockColors(*ui->listColors);

	const int remaining = listw->count();

	if(remaining == 0)
		return;

	QListWidgetItem* const itemw = listw->takeItem(listw->currentRow());
	Q_ASSERT(itemw);
	const QString& palId = itemw->data(Qt::UserRole).toString();
	delete itemw;

	if(remaining == 1) {
		// No more palettes!
		setPaletteViewEnabled(false);
		clearPaletteView();
	}

	// Delete palette definition.

	QMap< QString, QList<QRgb> >::iterator palItem = palettes_.find(palId);

	if(palItem != palettes_.end()) {
		palettes_.erase(palItem);
	}
}
