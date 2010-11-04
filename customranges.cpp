#include "customranges.hpp"
#include "ui_customranges.h"

CustomRanges::CustomRanges(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomRanges)
{
    ui->setupUi(this);
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
