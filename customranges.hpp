#ifndef CUSTOMRANGES_HPP
#define CUSTOMRANGES_HPP

#include <QDialog>

#include "specs.hpp"

namespace Ui {
    class CustomRanges;
}

class CustomRanges : public QDialog {
    Q_OBJECT
public:
    CustomRanges(QWidget *parent = 0);
    ~CustomRanges();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CustomRanges *ui;
};

#endif // CUSTOMRANGES_HPP
