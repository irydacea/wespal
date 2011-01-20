#ifndef CUSTOMPALETTES_HPP
#define CUSTOMPALETTES_HPP

#include <QDialog>

namespace Ui {
    class CustomPalettes;
}

class CustomPalettes : public QDialog {
    Q_OBJECT
public:
    CustomPalettes(QWidget *parent = 0);
    ~CustomPalettes();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CustomPalettes *ui;
};

#endif // CUSTOMPALETTES_HPP
