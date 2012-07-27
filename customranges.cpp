#include "customranges.hpp"
#include "ui_customranges.h"

#include <QColorDialog>
#include <QMessageBox>

CustomRanges::CustomRanges(QWidget *parent, QMap<QString, color_range>& initial_ranges) :
    QDialog(parent),
	ui(new Ui::CustomRanges),
	ranges_(initial_ranges),
	ignore_serializing_events_(false)
{
    ui->setupUi(this);

	for(QMap< QString, color_range >::const_iterator ci = ranges_.constBegin();
		ci != ranges_.constEnd();
		++ci)
	{
		// TODO: capitalize display name for built-ins!
		QListWidgetItem* lwi = new QListWidgetItem(ci.key(), ui->rangesList);
		lwi->setData(Qt::UserRole, ci.key());
	}

	ui->cmdUpdate->setEnabled(false);
	ui->rangesList->setCurrentRow(0);

	if(ranges_.count()) {
		deserialize_range(ranges_.begin().key(), ranges_.begin().value());
	} else {
		deserialize_default_range();
	}
}

CustomRanges::~CustomRanges()
{
    delete ui;
}

void CustomRanges::serialize_range(QString& name, color_range& range)
{
	name = ui->leName->text();
	range = color_range(
		QColor(ui->leAvg->text()).rgb(),
		QColor(ui->leMax->text()).rgb(),
		QColor(ui->leMin->text()).rgb());
}

void CustomRanges::deserialize_range(const QString& name, const color_range& range)
{
	ignore_serializing_events_ = true;

	QColor
		avg(range.mid()),
		max(range.max()),
		min(range.min());

	ui->leAvg->setText(avg.name());
	ui->leMax->setText(max.name());
	ui->leMin->setText(min.name());

	ui->leName->setText(name);

	ignore_serializing_events_ = false;
}

void CustomRanges::deserialize_default_range()
{
	deserialize_range(tr("New Range"), color_range());
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
	QString id;
	color_range range;
	serialize_range(id, range);

	if(id.isEmpty()) {
		QMessageBox::warning(
			this, tr("Wesnoth RCX"),
			tr("You need to specify a valid, non-empty identifier."));
		return;
	} else if(ranges_.find(id) != ranges_.end()) {
		QMessageBox::warning(
			this, tr("Wesnoth RCX"),
			tr("You cannot specify multiple ranges with the same identifier."));
		return;
	}

	ranges_.insert(id, range);

	QListWidgetItem* lwi = new QListWidgetItem(id, ui->rangesList);
	ui->rangesList->setCurrentRow(ui->rangesList->count() - 1);
}

void CustomRanges::on_cmdUpdate_clicked()
{
	if(!ui->rangesList->count())
		return;

	const QString& old_id = ui->rangesList->currentItem()->data(Qt::UserRole).toString();

	QMap<QString, color_range>::iterator range_it = ranges_.find(old_id);
	if(range_it == ranges_.end())
		return;

	QString new_id;
	color_range new_range;

	serialize_range(new_id, new_range);

	if(old_id != new_id) {
		ranges_.erase(range_it);
		ui->rangesList->currentItem()->setText(new_id);
	}

	ranges_.insert(new_id, new_range);

	ui->cmdUpdate->setEnabled(false);
}

void CustomRanges::on_cmdDelete_clicked()
{
	if(!ui->rangesList->count())
		return;

	const QString& id = ui->rangesList->currentItem()->data(Qt::UserRole).toString();

	QMap<QString, color_range>::iterator range_it = ranges_.find(id);
	if(range_it == ranges_.end())
		return;

	ranges_.erase(range_it);
	delete ui->rangesList->takeItem(ui->rangesList->currentRow());
}

void CustomRanges::on_leAvg_textChanged(QString)
{
	ui->cmdUpdate->setEnabled(true);
}

void CustomRanges::on_leMax_textChanged(QString)
{
	ui->cmdUpdate->setEnabled(true);
}

void CustomRanges::on_leMin_textChanged(QString)
{
	ui->cmdUpdate->setEnabled(true);
}

void CustomRanges::on_leName_textChanged(QString)
{
	ui->cmdUpdate->setEnabled(true);
}

void CustomRanges::on_rangesList_currentRowChanged(int /*currentRow*/)
{
	if(!ui->rangesList->count())
		return;

	const QString& id = ui->rangesList->currentItem()->data(Qt::UserRole).toString();

	QMap<QString, color_range>::iterator range_it = ranges_.find(id);
	if(range_it == ranges_.end())
		return;

	deserialize_range(id, range_it.value());

	ui->cmdUpdate->setEnabled(false);
}
