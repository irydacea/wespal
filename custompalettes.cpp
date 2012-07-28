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
}

void CustomPalettes::on_cmdRenPal_clicked()
{
	QListWidget* const listw = ui->listPals;
	listw->editItem(listw->currentItem());

	// TODO: update palette definition accordingly!
}

void CustomPalettes::on_tbEditColor_clicked()
{
	QListWidget* const listw = ui->listColors;
	QListWidgetItem* const lwi = listw->currentItem();

	if(!lwi)
		return;

	QColor color = lwi->data(Qt::UserRole).toInt();
	color = QColorDialog::getColor(color);

	if(color.isValid()) {
		lwi->setData(Qt::UserRole, color.rgb());
	}

	// TODO: update palette definition accordingly!
}

void CustomPalettes::on_cmdAddCol_clicked()
{
	QListWidget* const listw = ui->listColors;
	QListWidgetItem* const itemw = new QListWidgetItem("", listw);

	// We should already have a first row with the first
	// color of the palette. If we don't, then something is
	// wrong but we can still use pure black.

	QListWidgetItem* const first = listw->item(0);
	Q_ASSERT(first != NULL);

	itemw->setData(Qt::UserRole, first ? first->data(Qt::UserRole).toInt() : qRgb(0,0,0));
	itemw->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);

	listw->setCurrentItem(itemw);

	// TODO: update palette definition accordingly!
}

void CustomPalettes::on_cmdDelCol_clicked()
{
	QListWidget* const listw = ui->listColors;
	const int remaining = listw->count();

	Q_ASSERT(remaining > 0);

	if(remaining == 1) {
		QMessageBox::critical(this, tr("Wesnoth RCX"), tr("You cannot remove the last color in the palette!"));
		// TODO: delete the palette itself in this case
		//       or just delete the color and disable widgets
		return;
	}

	delete listw->takeItem(listw->currentRow());

	// TODO: update palette definition accordingly!
}
