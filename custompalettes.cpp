#include "custompalettes.hpp"
#include "ui_custompalettes.h"

#include "paletteitem.hpp"
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
	//ui->listColors->clear();
	ui->listPals->clear();

	for(QMap< QString, QList<QRgb> >::const_iterator i = palettes_.constBegin();
		i != palettes_.constEnd(); ++i) {
		addPaletteListEntry(i.key());
	}
}

void CustomPalettes::addPaletteListEntry(const QString& name)
{
	ui->listPals->addItem(name);
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

	listw->clear();

	foreach(QRgb rgb, pal) {
		QListWidgetItem* itemw = new QListWidgetItem("", listw);
		itemw->setData(Qt::UserRole, rgb);
		itemw->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
	}
}
