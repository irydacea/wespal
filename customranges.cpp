#include "customranges.hpp"
#include "ui_customranges.h"

#include "codesnippetdialog.hpp"
#include "paletteitem.hpp"
#include "util.hpp"

#include <QColorDialog>
#include <QMessageBox>

CustomRanges::CustomRanges(const QMap<QString, color_range>& initialRanges, QWidget *parent) :
    QDialog(parent),
	ui(new Ui::CustomRanges),
	ranges_(initialRanges)
{
    ui->setupUi(this);

	{
		const ObjectLock lock(ui->listRanges);

		for(QMap< QString, color_range >::const_iterator ci = ranges_.constBegin();
			ci != ranges_.constEnd();
			++ci)
		{
			addRangeListEntry(ci.key());
		}
	}

	if(!ranges_.empty()) {
		ui->listRanges->setCurrentRow(0);
	}

	updateRangeEditControls();
}

CustomRanges::~CustomRanges()
{
    delete ui;
}

void CustomRanges::changeEvent(QEvent *e)
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

QString CustomRanges::generateNewRangeName() const
{
	QString name;
	int i = 0;

	do {
		name = tr("New Color Range #%1").arg(++i);
	} while(ranges_.find(name) != ranges_.end());

	return name;

}

color_range& CustomRanges::currentRange()
{
	QListWidget* const listw = ui->listRanges;
	QListWidgetItem* const itemw = listw->currentItem();

	Q_ASSERT(itemw);

	if(!itemw)
		return ranges_[""];

	const QString& name = itemw->data(Qt::UserRole).toString();

	// Create a new range if it doesn't exist already.
	return ranges_[name];
}

void CustomRanges::updateColorButton(QAbstractButton* button, const QColor& color)
{
	Q_ASSERT(button);

	static const QSize colorIconSize(48, 16);

	button->setIconSize(colorIconSize);
	button->setIcon(createColorIcon(color, colorIconSize.width(), colorIconSize.height()));
}

void CustomRanges::addRangeListEntry(const QString& name)
{
	QListWidget* const listw = ui->listRanges;
	QListWidgetItem* const itemw = new QListWidgetItem(name, listw);

	itemw->setData(Qt::UserRole, name);
	itemw->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable | Qt::ItemIsEnabled);
}

void CustomRanges::updateRangeEditControls()
{
	QListWidget* const listw = ui->listRanges;

	const bool noMoreRanges = listw->count() == 0;
	ui->boxRangeEditor->setEnabled(!noMoreRanges);

	// Do not send textChanged signals, since that would alter
	// the ranges_ map (adding a range with an empty key in the
	// noMoreRanges == true case) and either cause problems or
	// waste everyone's time doing color parsing. We'll update
	// the color picker buttons directly instead.
	const ObjectLock lock(this);

	if(noMoreRanges) {
		const QColor& color = this->palette().color(QPalette::Button);

		ui->leAvg->clear();
		updateColorButton(ui->tbAvg, color);
		ui->leMin->clear();
		updateColorButton(ui->tbMax, color);
		ui->leMax->clear();
		updateColorButton(ui->tbMin, color);
	} else {
		color_range& range = currentRange();
		const QColor avg = range.mid(), min = range.min(), max = range.max();

		ui->leAvg->setText(avg.name());
		updateColorButton(ui->tbAvg, avg);
		ui->leMax->setText(max.name());
		updateColorButton(ui->tbMax, max);
		ui->leMin->setText(min.name());
		updateColorButton(ui->tbMin, min);
	}
}

void CustomRanges::on_tbAvg_clicked()
{
	QColor newc(ui->leAvg->text());
	newc = QColorDialog::getColor(newc, this);
	if(newc.isValid()) {
		ui->leAvg->setText(newc.name());
	}
}

void CustomRanges::on_tbMax_clicked()
{
	QColor newc(ui->leMax->text());
	newc = QColorDialog::getColor(newc, this);
	if(newc.isValid()) {
		ui->leMax->setText(newc.name());
	}
}

void CustomRanges::on_tbMin_clicked()
{
	QColor newc(ui->leMin->text());
	newc = QColorDialog::getColor(newc, this);
	if(newc.isValid()) {
		ui->leMin->setText(newc.name());
	}
}

void CustomRanges::on_cmdAdd_clicked()
{
	QListWidget* const listw = ui->listRanges;

	const QString& raName = generateNewRangeName();
	ranges_.insert(raName, color_range());
	addRangeListEntry(raName);

	listw->setCurrentRow(listw->count() - 1);
	listw->editItem(listw->currentItem());
}

void CustomRanges::on_cmdRename_clicked()
{
	QListWidget* const listw = ui->listRanges;
	// An edit slot should take care of updating the
	// range definition afterwards.
	listw->editItem(listw->currentItem());
}

void CustomRanges::on_cmdDelete_clicked()
{
	QListWidget* const listw = ui->listRanges;

	ObjectLock lock(listw);

	const int remaining = listw->count();
	if(remaining == 0)
		return;

	QListWidgetItem* const itemw = listw->takeItem(listw->currentRow());
	Q_ASSERT(itemw);
	const QString& raName = itemw->data(Qt::UserRole).toString();
	delete itemw;

	if(remaining == 1) {
		// No more ranges!
		updateRangeEditControls();
	}

	// Delete the actual color range.

	QMap<QString, color_range>::iterator raIt = ranges_.find(raName);

	if(raIt != ranges_.end()) {
		ranges_.erase(raIt);
	}
}

void CustomRanges::on_leAvg_textChanged(QString)
{
	QColor newColor(ui->leAvg->text());

	if(!newColor.isValid())
		return;

	currentRange().setMid(newColor.rgb());
	updateColorButton(ui->tbAvg, newColor);
}

void CustomRanges::on_leMax_textChanged(QString)
{
	QColor newColor(ui->leMax->text());

	if(!newColor.isValid())
		return;

	currentRange().setMax(newColor.rgb());
	updateColorButton(ui->tbMax, newColor);
}

void CustomRanges::on_leMin_textChanged(QString)
{
	QColor newColor(ui->leMin->text());

	if(!newColor.isValid())
		return;

	currentRange().setMin(newColor.rgb());
	updateColorButton(ui->tbMin, newColor);
}

void CustomRanges::on_listRanges_currentRowChanged(int /*currentRow*/)
{
	updateRangeEditControls();
}

void CustomRanges::on_listRanges_itemChanged(QListWidgetItem *item)
{
	Q_ASSERT(item);

	const QString& newName = item->text();
	const QString& oldName = item->data(Qt::UserRole).toString();

	if(newName == oldName)
		return;

	QMap<QString, color_range>::iterator
		oldIt = ranges_.find(oldName),
		newIt = ranges_.find(newName);
	Q_ASSERT(oldIt != ranges_.end());
	Q_ASSERT(oldIt != newIt);

	if(oldIt == ranges_.end())
		return;

	if(newIt != ranges_.end()) {
		if(!JobUi::prompt(this, tr("The range '%1' already exists. Do you wish to overwrite it?").arg(newName))) {
			item->setText(oldName);
			return;
		}

		for(int i = 0; i < ui->listRanges->count(); ++i) {
			QListWidgetItem* const ii = ui->listRanges->item(i);
			if(ii->data(Qt::UserRole).toString() == newName) {
				delete ui->listRanges->takeItem(i);
				break;
			}
		}
	} else {
		newIt = ranges_.insert(newName, color_range());
	}

	newIt.value() = oldIt.value();
	ranges_.erase(oldIt);

	item->setData(Qt::UserRole, newName);
}

void CustomRanges::on_cmdWml_clicked()
{
	QListWidget* const listw = ui->listRanges;
	QListWidgetItem* const itemw = listw->currentItem();

	if(!itemw)
		return;

	const QString& raName = itemw->data(Qt::UserRole).toString();
	const color_range& range = ranges_.value(raName);
	const QString& wml = generate_color_range_wml(raName, range);

	CodeSnippetDialog dlg(wml, this);
	dlg.setWindowTitle(tr("Color Range WML"));
	dlg.exec();
}
