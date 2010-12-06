#include "customranges.hpp"
#include "ui_customranges.h"

#include <QColorDialog>
#include <QMessageBox>

CustomRanges::CustomRanges(QWidget *parent, QList<range_spec>& initial_ranges) :
    QDialog(parent),
	ui(new Ui::CustomRanges),
	ranges_(initial_ranges),
	ignore_serializing_events_(false)
{
    ui->setupUi(this);
	post_setup();
}

CustomRanges::~CustomRanges()
{
    delete ui;
}

void CustomRanges::post_setup()
{
	foreach(const range_spec& r, ranges_) {
		ui->rangesList->addItem(new QListWidgetItem(r.name));
	}

	ui->cmdUpdate->setEnabled(false);
	ui->rangesList->setCurrentRow(0);

	if(ranges_.count()) {
		deserialize_range(ranges_.front());
	}
	else {
		deserialize_range(range_spec()); // clear input widgets
	}
}

void CustomRanges::serialize_range(range_spec &range)
{
	range.id = ui->leId->text();
	range.name = ui->leName->text();
	range.def = color_range(
		QColor(ui->leAvg->text()).rgb(),
		QColor(ui->leMax->text()).rgb(),
		QColor(ui->leMin->text()).rgb());
}

void CustomRanges::deserialize_range(range_spec const& range)
{
	ignore_serializing_events_ = true;

	QColor
		avg(range.def.mid()),
		max(range.def.max()),
		min(range.def.min());

	ui->leAvg->setText(avg.name());
	ui->leMax->setText(max.name());
	ui->leMin->setText(min.name());

	ui->leId->setText(range.id);
	ui->leName->setText(range.name);

	ignore_serializing_events_ = false;
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
	range_spec nr;
	serialize_range(nr);

	if(nr.id.isEmpty()) {
		QMessageBox::warning(
			this, tr("Wesnoth RCX"),
			tr("You need to specify a valid, non-empty identifier."));
		return;
	}
	else {
		foreach(const range_spec& r, ranges_) {
			if(r.id == nr.id) {
				QMessageBox::warning(
					this, tr("Wesnoth RCX"),
					tr("You cannot specify multiple ranges with the same identifier."));
				return;
			}
		}
	}

	ranges_.push_back(nr);

	QListWidget& l = *ui->rangesList;
	QListWidgetItem* i = new QListWidgetItem(nr.name + QString(" [") + nr.id+ QString("]"));
	i->setSelected(true);
	// Adds the listbox item and clears the input widgets
	l.addItem(i);
	deserialize_range(range_spec());
}

void CustomRanges::on_cmdUpdate_clicked()
{
	QListWidget& l = *ui->rangesList;
	if(!l.count())
		return;

	range_spec& r = ranges_[l.currentRow()];

	serialize_range(r);
	l.currentItem()->setText(r.name + QString(" [") + r.id + QString("]"));

	ui->cmdUpdate->setEnabled(false);
}

void CustomRanges::on_cmdDelete_clicked()
{
	QListWidget& l = *ui->rangesList;
	if(!l.count())
		return;

	ranges_.removeAt(l.currentRow());
	l.takeItem(l.currentRow());
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

void CustomRanges::on_leId_textChanged(QString)
{
	ui->cmdUpdate->setEnabled(true);
}

void CustomRanges::on_rangesList_itemSelectionChanged()
{
	deserialize_range(ranges_.at(ui->rangesList->currentRow()));
}
