#ifndef CUSTOMRANGES_HPP
#define CUSTOMRANGES_HPP

#include <QDialog>

#include "wesnothrc.hpp"

#include <QAbstractButton>
#include <QListWidgetItem>

namespace Ui {
    class CustomRanges;
}

class CustomRanges : public QDialog {
    Q_OBJECT
public:
	CustomRanges(const QMap<QString, color_range>& initialRanges, QWidget *parent);
    ~CustomRanges();

	const QMap<QString, color_range>& ranges() const {
		return ranges_;
	}

protected:
	void changeEvent(QEvent *e);

private:
    Ui::CustomRanges *ui;

	QMap<QString, color_range> ranges_;

	color_range& currentRange();

	void updateColorButton(QAbstractButton* button, const QColor& color);
	void updateRangeEditControls();

	QString generateNewRangeName() const;

	void addRangeListEntry(const QString& name);

private slots:
	void on_cmdAdd_clicked();
	void on_cmdRename_clicked();
	void on_cmdDelete_clicked();

	void on_leMin_textChanged(QString);
	void on_leMax_textChanged(QString);
	void on_leAvg_textChanged(QString);

	void on_tbMin_clicked();
	void on_tbMax_clicked();
	void on_tbAvg_clicked();
	void on_listRanges_currentRowChanged(int);
	void on_listRanges_itemChanged(QListWidgetItem *item);
};

#endif // CUSTOMRANGES_HPP
