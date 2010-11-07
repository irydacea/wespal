#include "customranges.hpp"
#include "ui_customranges.h"

#include <QColorDialog>

CustomRanges::CustomRanges(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::CustomRanges),
	ranges_(),
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
		deserialize_range(r);
	}

	ui->cmdUpdate->setEnabled(false);
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

void CustomRanges::on_cmdUpdate_clicked()
{
	QListWidget& l = *ui->rangesList;
	if(!l.count())
		return;

	serialize_range(ranges_[l.currentRow()]);

	ui->cmdUpdate->setEnabled(false);
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
