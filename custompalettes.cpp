#include "custompalettes.hpp"
#include "ui_custompalettes.h"

CustomPalettes::CustomPalettes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CustomPalettes)
{
    ui->setupUi(this);
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
